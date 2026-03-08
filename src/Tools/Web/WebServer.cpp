#include "pch.h"
#include "Tools/Web/WebServer.h"
#include "Tools/Logger.h"
#include <httplib.h>
#include <chrono>
#include <sstream>
#include <iomanip>

WebServer::WebServer()
    : m_running(false)
    , m_shouldStop(false)
    , m_port(8080)
    , m_totalRequests(0)
    , m_nextRequestId(1)
{
}

WebServer::~WebServer()
{
    Stop();
}

bool WebServer::Start(int port)
{
    if (m_running.load())
    {
        LOG_WARNING_SRC("Server is already running", "WebServer");
        return false;
    }

    m_port = port;
    m_shouldStop = false;

    try
    {
        // Create server instance
        m_server = std::make_unique<httplib::Server>();
        
        // Setup routes
        SetupRoutes();

        // Start server in separate thread
        m_serverThread = std::thread(&WebServer::ServerThreadFunc, this);

        LOG_INFO_SRC("WebServer starting on port " + std::to_string(m_port), "WebServer");
        
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR_SRC("Failed to start WebServer: " + std::string(e.what()), "WebServer");
        return false;
    }
}

void WebServer::Stop()
{
    if (!m_running.load())
        return;

    LOG_INFO_SRC("Stopping WebServer...", "WebServer");
    
    m_shouldStop = true;
    
    if (m_server)
    {
        m_server->stop();
    }

    if (m_serverThread.joinable())
    {
        m_serverThread.join();
    }

    m_running = false;
    m_server.reset();
    
    LOG_INFO_SRC("WebServer stopped", "WebServer");
}

void WebServer::ServerThreadFunc()
{
    m_running = true;
    
    LOG_INFO_SRC("Server thread started, binding to 0.0.0.0:" + std::to_string(m_port), "WebServer");

    // Listen blocks until Stop() is called
    if (!m_server->listen("0.0.0.0", m_port))
    {
        if (!m_shouldStop.load())
        {
            LOG_ERROR_SRC("Failed to bind to port " + std::to_string(m_port) + ". Port may be in use.", "WebServer");
        }
    }
    
    m_running = false;
}

void WebServer::SetupRoutes()
{
    // Example GET route: /
    m_server->Get("/", [this](const httplib::Request& req, httplib::Response& res) {
        // Create request record
        WebRequest webReq;
        webReq.method = "GET";
        webReq.path = "/";
        webReq.remoteAddr = req.remote_addr;
        webReq.body = "";
        webReq.id = m_nextRequestId.fetch_add(1);

        // Timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        oss << std::put_time(&tm_buf, "%H:%M:%S");
        webReq.timestamp = oss.str();

        // Add to queue
        AddRequestToQueue(webReq);

        // Send response
        res.set_content("Hello from Base Application WebServer!", "text/plain");
        res.status = 200;
    });

    // Example GET route: /api/status
    m_server->Get("/api/status", [this](const httplib::Request& req, httplib::Response& res) {
        WebRequest webReq;
        webReq.method = "GET";
        webReq.path = "/api/status";
        webReq.remoteAddr = req.remote_addr;
        webReq.body = "";
        webReq.id = m_nextRequestId.fetch_add(1);

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        oss << std::put_time(&tm_buf, "%H:%M:%S");
        webReq.timestamp = oss.str();

        AddRequestToQueue(webReq);

        // JSON response
        std::string json = "{ \"status\": \"ok\", \"server\": \"Base Application\", \"requests\": " 
                         + std::to_string(m_totalRequests.load()) + " }";
        res.set_content(json, "application/json");
        res.status = 200;
    });

    // Example POST route: /webhook
    m_server->Post("/webhook", [this](const httplib::Request& req, httplib::Response& res) {
        WebRequest webReq;
        webReq.method = "POST";
        webReq.path = "/webhook";
        webReq.remoteAddr = req.remote_addr;
        webReq.body = req.body;
        webReq.id = m_nextRequestId.fetch_add(1);

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        oss << std::put_time(&tm_buf, "%H:%M:%S");
        webReq.timestamp = oss.str();

        AddRequestToQueue(webReq);

        // Send response
        res.set_content("Webhook received", "text/plain");
        res.status = 200;
    });

    // Catch-all route for undefined endpoints
    m_server->set_error_handler([this](const httplib::Request& req, httplib::Response& res) {
        WebRequest webReq;
        webReq.method = req.method;
        webReq.path = req.path;
        webReq.remoteAddr = req.remote_addr;
        webReq.body = req.body;
        webReq.id = m_nextRequestId.fetch_add(1);

        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        oss << std::put_time(&tm_buf, "%H:%M:%S");
        webReq.timestamp = oss.str();

        AddRequestToQueue(webReq);

        res.set_content("404 Not Found", "text/plain");
        res.status = 404;
    });
}

void WebServer::AddRequestToQueue(const WebRequest& request)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    m_requestQueue.push(request);
    m_totalRequests.fetch_add(1);
    
    LOG_DEBUG_SRC(request.method + " " + request.path + " from " + request.remoteAddr, "WebServer");
}

std::vector<WebRequest> WebServer::GetRecentRequests(size_t maxCount)
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::vector<WebRequest> requests;
    
    // Get up to maxCount requests from the queue
    size_t count = std::min(maxCount, m_requestQueue.size());
    
    // Copy queue to vector (preserving order)
    std::queue<WebRequest> tempQueue = m_requestQueue;
    while (!tempQueue.empty() && requests.size() < count)
    {
        requests.push_back(tempQueue.front());
        tempQueue.pop();
    }
    
    return requests;
}

void WebServer::ClearRequests()
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    std::queue<WebRequest> emptyQueue;
    std::swap(m_requestQueue, emptyQueue);
}

size_t WebServer::GetRequestCount() const
{
    std::lock_guard<std::mutex> lock(m_queueMutex);
    return m_requestQueue.size();
}
