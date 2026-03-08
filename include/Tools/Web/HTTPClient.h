#pragma once

#include <string>
#include <map>

enum class HTTPMethod
{
    GET,
    POST,
    PUT,
    DELETE_METHOD,   // Renamed to avoid Windows macro conflict
    PATCH,
    HEAD,
    OPTIONS
};

struct HTTPResponse
{
    long statusCode;
    std::string body;
    std::string error;
    bool success;
};

class HTTPClient
{
public:
    HTTPClient();
    ~HTTPClient();

    // Perform HTTP requests with specified method
    bool PerformRequest(const std::string& url, HTTPMethod method = HTTPMethod::GET, 
                        const std::string& payload = "", const std::map<std::string, std::string>& params = {});

    // Getters
    const std::string& GetResponse() const { return m_response.body; }
    const std::string& GetError() const { return m_response.error; }
    long GetStatusCode() const { return m_response.statusCode; }
    bool IsLoading() const { return m_isLoading; }
    bool WasSuccessful() const { return m_response.success; }

    // Clear buffers
    void ClearResponse();
    void ClearError();
    void ClearAll();

private:
    HTTPResponse m_response;
    bool m_isLoading;

    // Helper method to build query string from parameters
    std::string BuildQueryString(const std::string& baseUrl, const std::map<std::string, std::string>& params);

    // Helper method to get HTTP method string
    const char* GetMethodString(HTTPMethod method) const;

    // Friend function for CURL callback
    friend size_t HTTPClientWriteCallback(void* contents, size_t size, size_t nmemb, HTTPClient* client);
    void AppendResponse(const char* data, size_t size);
};
