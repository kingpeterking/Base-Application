#include "pch.h"
#include "Tools/Web/HTTPClient.h"
#include <sstream>

// CURL write callback
size_t HTTPClientWriteCallback(void* contents, size_t size, size_t nmemb, HTTPClient* client)
{
    client->AppendResponse((char*)contents, size * nmemb);
    return size * nmemb;
}

HTTPClient::HTTPClient()
    : m_isLoading(false)
{
    m_response.statusCode = 0;
    m_response.success = false;
}

HTTPClient::~HTTPClient()
{
}

std::string HTTPClient::BuildQueryString(const std::string& baseUrl, const std::map<std::string, std::string>& params)
{
    if (params.empty())
        return baseUrl;

    std::string result = baseUrl;
    result += (baseUrl.find('?') != std::string::npos) ? "&" : "?";

    bool first = true;
    for (const auto& [key, value] : params)
    {
        if (!first) result += "&";
        result += key + "=" + value;
        first = false;
    }

    return result;
}

const char* HTTPClient::GetMethodString(HTTPMethod method) const
{
    switch (method)
    {
        case HTTPMethod::GET: return "GET";
        case HTTPMethod::POST: return "POST";
        case HTTPMethod::PUT: return "PUT";
        case HTTPMethod::DELETE_METHOD: return "DELETE";
        case HTTPMethod::PATCH: return "PATCH";
        case HTTPMethod::HEAD: return "HEAD";
        case HTTPMethod::OPTIONS: return "OPTIONS";
        default: return "GET";
    }
}

bool HTTPClient::PerformRequest(const std::string& url, HTTPMethod method, 
                                 const std::string& payload, const std::map<std::string, std::string>& params)
{
    if (m_isLoading)
        return false;

    m_isLoading = true;
    m_response.body.clear();
    m_response.error.clear();
    m_response.statusCode = 0;
    m_response.success = false;

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_response.error = "Failed to initialize CURL";
        m_isLoading = false;
        return false;
    }

    try
    {
        // Build URL with query parameters
        std::string fullUrl = BuildQueryString(url, params);

        curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPClientWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        // Set HTTP method
        switch (method)
        {
            case HTTPMethod::POST:
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                if (!payload.empty())
                {
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
                }
                break;

            case HTTPMethod::PUT:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                if (!payload.empty())
                {
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
                }
                break;

            case HTTPMethod::DELETE_METHOD:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                if (!payload.empty())
                {
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
                }
                break;

            case HTTPMethod::PATCH:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                if (!payload.empty())
                {
                    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
                }
                break;

            case HTTPMethod::HEAD:
                curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
                break;

            case HTTPMethod::OPTIONS:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
                break;

            case HTTPMethod::GET:
            default:
                // GET is default, no extra options needed
                break;
        }

        // Perform the request
        CURLcode res = curl_easy_perform(curl);

        // Get status code
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &m_response.statusCode);

        if (res != CURLE_OK)
        {
            m_response.error = std::string("CURL Error: ") + curl_easy_strerror(res);
            m_response.success = false;
        }
        else
        {
            m_response.success = (m_response.statusCode >= 200 && m_response.statusCode < 300);
            if (!m_response.success && m_response.statusCode > 0)
            {
                m_response.error = "HTTP " + std::to_string(m_response.statusCode);
            }
        }
    }
    catch (const std::exception& e)
    {
        m_response.error = std::string("Exception: ") + e.what();
        m_response.success = false;
    }

    curl_easy_cleanup(curl);
    m_isLoading = false;

    return m_response.success;
}

void HTTPClient::ClearResponse()
{
    m_response.body.clear();
}

void HTTPClient::ClearError()
{
    m_response.error.clear();
}

void HTTPClient::ClearAll()
{
    m_response.body.clear();
    m_response.error.clear();
    m_response.statusCode = 0;
    m_response.success = false;
}

void HTTPClient::AppendResponse(const char* data, size_t size)
{
    m_response.body.append(data, size);
}
