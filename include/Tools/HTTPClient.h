#pragma once

#include <string>

class HTTPClient
{
public:
    HTTPClient();
    ~HTTPClient();

    // Perform an HTTP GET request
    bool PerformRequest(const std::string& url);

    // Getters
    const std::string& GetResponse() const { return m_responseBuffer; }
    const std::string& GetError() const { return m_requestError; }
    bool IsLoading() const { return m_isLoading; }

    // Clear buffers
    void ClearResponse();
    void ClearError();
    void ClearAll();

private:
    std::string m_responseBuffer;
    std::string m_requestError;
    bool m_isLoading;

    // Friend function for CURL callback
    friend size_t HTTPClientWriteCallback(void* contents, size_t size, size_t nmemb, HTTPClient* client);
    void AppendResponse(const char* data, size_t size);
};
