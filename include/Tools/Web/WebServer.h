#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>
#include <memory>

// Forward declare httplib to avoid including in header
namespace httplib {
    class Server;
}

// Represents an HTTP request received by the server
struct WebRequest
{
    std::string method;        // GET, POST, etc.
    std::string path;          // /api/endpoint
    std::string body;          // Request body
    std::string remoteAddr;    // Client IP address
    std::string timestamp;     // When received
    int id;                    // Unique ID for this request

    WebRequest() : id(0) {}
};

// Web server that runs in a separate thread
class WebServer
{
public:
    WebServer();
    ~WebServer();

    // Server control
    bool Start(int port = 8080);
    void Stop();
    bool IsRunning() const { return m_running.load(); }
    int GetPort() const { return m_port; }

    // Get received requests (call from main thread)
    std::vector<WebRequest> GetRecentRequests(size_t maxCount = 100);
    void ClearRequests();
    size_t GetRequestCount() const;

    // Statistics
    size_t GetTotalRequestsReceived() const { return m_totalRequests.load(); }

private:
    void ServerThreadFunc();
    void SetupRoutes();
    void AddRequestToQueue(const WebRequest& request);

    std::unique_ptr<httplib::Server> m_server;
    std::thread m_serverThread;
    std::atomic<bool> m_running;
    std::atomic<bool> m_shouldStop;
    int m_port;

    // Thread-safe request queue
    std::queue<WebRequest> m_requestQueue;
    mutable std::mutex m_queueMutex;
    
    // Statistics
    std::atomic<size_t> m_totalRequests;
    std::atomic<int> m_nextRequestId;
};
