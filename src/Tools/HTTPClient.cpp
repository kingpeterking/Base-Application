#include "pch.h"
#include "Tools/HTTPClient.h"

// CURL write callback
size_t HTTPClientWriteCallback(void* contents, size_t size, size_t nmemb, HTTPClient* client)
{
    client->AppendResponse((char*)contents, size * nmemb);
    return size * nmemb;
}

HTTPClient::HTTPClient()
    : m_isLoading(false)
{
}

HTTPClient::~HTTPClient()
{
}

bool HTTPClient::PerformRequest(const std::string& url)
{
    if (m_isLoading)
        return false;

    m_isLoading = true;
    m_responseBuffer.clear();
    m_requestError.clear();

    CURL* curl = curl_easy_init();
    if (!curl)
    {
        m_requestError = "Failed to initialize CURL";
        m_isLoading = false;
        return false;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HTTPClientWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        m_requestError = std::string("Error: ") + curl_easy_strerror(res);
    }

    curl_easy_cleanup(curl);
    m_isLoading = false;

    return res == CURLE_OK;
}

void HTTPClient::ClearResponse()
{
    m_responseBuffer.clear();
}

void HTTPClient::ClearError()
{
    m_requestError.clear();
}

void HTTPClient::ClearAll()
{
    m_responseBuffer.clear();
    m_requestError.clear();
}

void HTTPClient::AppendResponse(const char* data, size_t size)
{
    m_responseBuffer.append(data, size);
}
