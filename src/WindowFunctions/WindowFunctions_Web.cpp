#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "Tools/Web/HTTPClient.h"
#include "Tools/Logger.h"

// ============================================================================
// WEB/NETWORK WINDOW FUNCTIONS
// ============================================================================
// URL Request, Web Server Control, Web Server Request Monitor
// ============================================================================

void WindowFunctions::RenderURLRequestWindow(bool* isOpen)
{
    static const char* httpMethods[] = { "GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS" };

    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("URL Request", isOpen))
    {
        ImGui::Text("HTTP Request Tool");
        ImGui::Separator();

        // HTTP Method selection
        ImGui::Text("Method:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::Combo("##http_method", &m_app->m_selectedHTTPMethod, httpMethods, IM_ARRAYSIZE(httpMethods));

        // URL input
        ImGui::Text("URL:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##url_input", m_app->m_urlBuffer, m_app->URL_BUFFER_SIZE);

        ImGui::Separator();

        // Collapsible section for URL Parameters
        if (ImGui::CollapsingHeader("URL Parameters", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Add Parameter:");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##param_key", m_app->m_paramKeyBuffer, m_app->PARAM_BUFFER_SIZE);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##param_value", m_app->m_paramValueBuffer, m_app->PARAM_BUFFER_SIZE);
            ImGui::SameLine();
            if (ImGui::Button("Add Param", ImVec2(100, 0)))
            {
                std::string key = m_app->m_paramKeyBuffer;
                std::string value = m_app->m_paramValueBuffer;
                if (!key.empty() && !value.empty())
                {
                    m_app->m_requestParameters[key] = value;
                    memset(m_app->m_paramKeyBuffer, 0, m_app->PARAM_BUFFER_SIZE);
                    memset(m_app->m_paramValueBuffer, 0, m_app->PARAM_BUFFER_SIZE);
                }
            }

            // Display existing parameters
            if (!m_app->m_requestParameters.empty())
            {
                ImGui::Text("Current Parameters:");
                std::vector<std::string> keysToRemove;
                for (auto& [key, value] : m_app->m_requestParameters)
                {
                    ImGui::BulletText("%s = %s", key.c_str(), value.c_str());
                    ImGui::SameLine();
                    std::string removeButtonId = "Remove##" + key;
                    if (ImGui::SmallButton(removeButtonId.c_str()))
                    {
                        keysToRemove.push_back(key);
                    }
                }
                for (const auto& key : keysToRemove)
                {
                    m_app->m_requestParameters.erase(key);
                }
            }
        }

        // Collapsible section for Request Payload
        if (ImGui::CollapsingHeader("Request Payload", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextDisabled("(For POST, PUT, PATCH, DELETE requests)");
            ImGui::InputTextMultiline("##payload_input", m_app->m_payloadBuffer, m_app->PAYLOAD_BUFFER_SIZE,
                ImVec2(-1, 100), ImGuiInputTextFlags_AllowTabInput);
        }

        ImGui::Separator();

        // Send button
        if (ImGui::Button("Send Request", ImVec2(150, 0)) && !m_app->m_httpClient.IsLoading())
        {
            HTTPMethod method = static_cast<HTTPMethod>(m_app->m_selectedHTTPMethod);
            std::string payload = m_app->m_payloadBuffer;
            m_app->m_httpClient.PerformRequest(m_app->m_urlBuffer, method, payload, m_app->m_requestParameters);
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear All", ImVec2(100, 0)))
        {
            m_app->m_httpClient.ClearAll();
            m_app->m_requestParameters.clear();
            memset(m_app->m_payloadBuffer, 0, m_app->PAYLOAD_BUFFER_SIZE);
        }

        ImGui::Separator();

        // Status and Response Code
        if (m_app->m_httpClient.IsLoading())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loading...");
        }
        else
        {
            if (m_app->m_httpClient.WasSuccessful())
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 
                    "Success | Status Code: %ld | Response: %zu bytes", 
                    m_app->m_httpClient.GetStatusCode(), m_app->m_httpClient.GetResponse().size());
            }
            else if (m_app->m_httpClient.GetStatusCode() > 0)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.0f, 1.0f), 
                    "HTTP %ld - %s", 
                    m_app->m_httpClient.GetStatusCode(), 
                    m_app->m_httpClient.GetError().c_str());
            }
            else if (!m_app->m_httpClient.GetError().empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
                    "Error: %s", m_app->m_httpClient.GetError().c_str());
            }
        }

        ImGui::Separator();
        ImGui::Text("Response:");

        // Response display with scrollbar
        ImGui::BeginChild("response_scroll", ImVec2(0, -50), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(m_app->m_httpClient.GetResponse().c_str(), 
            m_app->m_httpClient.GetResponse().c_str() + m_app->m_httpClient.GetResponse().size());
        ImGui::EndChild();

        // Copy button
        if (!m_app->m_httpClient.GetResponse().empty())
        {
            if (ImGui::Button("Copy Response"))
            {
                ImGui::SetClipboardText(m_app->m_httpClient.GetResponse().c_str());
            }
        }
    }

    ImGui::End();
}

void WindowFunctions::RenderWebServerControlWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Web Server Control", isOpen))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Web Server Configuration");
        ImGui::Separator();

        // Server status
        bool isRunning = m_app->m_webServer.IsRunning();
        if (isRunning)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: RUNNING");
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Status: STOPPED");
        }

        ImGui::Separator();

        // Port configuration
        static int port = 8080;
        ImGui::Text("Server Port:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (isRunning)
        {
            ImGui::BeginDisabled();
            ImGui::InputInt("##port", &port);
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(Cannot change while running)");
        }
        else
        {
            ImGui::InputInt("##port", &port);
            if (port < 1 || port > 65535)
            {
                port = std::clamp(port, 1, 65535);
            }
        }

        ImGui::Separator();

        // Start/Stop buttons
        if (isRunning)
        {
            if (ImGui::Button("Stop Server", ImVec2(200, 40)))
            {
                m_app->m_webServer.Stop();
                LOG_INFO("Web server stopped by user");
            }
        }
        else
        {
            if (ImGui::Button("Start Server", ImVec2(200, 40)))
            {
                if (m_app->m_webServer.Start(port))
                {
                    LOG_INFO("Web server started on port " + std::to_string(port));
                }
                else
                {
                    LOG_ERROR("Failed to start web server on port " + std::to_string(port));
                }
            }
        }

        ImGui::Separator();

        // Server information
        if (isRunning)
        {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Server Information:");
            ImGui::Text("Listening on: http://0.0.0.0:%d", m_app->m_webServer.GetPort());
            ImGui::Text("Total requests: %zu", m_app->m_webServer.GetTotalRequestsReceived());
            ImGui::Text("Queued requests: %zu", m_app->m_webServer.GetRequestCount());

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Available Endpoints:");
            ImGui::BulletText("GET  /");
            ImGui::BulletText("GET  /api/status");
            ImGui::BulletText("POST /webhook");

            ImGui::Separator();

            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Test from command line:");
            ImGui::TextWrapped("curl http://localhost:%d/", m_app->m_webServer.GetPort());
            ImGui::TextWrapped("curl http://localhost:%d/api/status", m_app->m_webServer.GetPort());
            ImGui::TextWrapped("curl -X POST -d \"test\" http://localhost:%d/webhook", m_app->m_webServer.GetPort());
        }
        else
        {
            ImGui::TextDisabled("Server is not running. Click 'Start Server' to begin.");
        }
    }
    ImGui::End();
}

void WindowFunctions::RenderWebServerRequestsWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Web Server - Request Monitor", isOpen))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "HTTP Request Monitor");
        ImGui::Separator();

        // Statistics bar
        size_t totalRequests = m_app->m_webServer.GetTotalRequestsReceived();
        size_t queuedRequests = m_app->m_webServer.GetRequestCount();

        ImGui::Text("Total Requests: %zu", totalRequests);
        ImGui::SameLine(0, 30);
        ImGui::Text("Queued: %zu", queuedRequests);
        ImGui::SameLine(0, 30);

        if (ImGui::Button("Clear History"))
        {
            m_app->m_webServer.ClearRequests();
        }

        ImGui::Separator();

        // Request table
        if (ImGui::BeginTable("requests_table", 6, 
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
            ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Method", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed, 200);
            ImGui::TableSetupColumn("Client IP", ImGuiTableColumnFlags_WidthFixed, 150);
            ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupScrollFreeze(0, 1); // Freeze header row
            ImGui::TableHeadersRow();

            // Get recent requests
            auto requests = m_app->m_webServer.GetRecentRequests(100);

            // Render each request (most recent first)
            for (auto it = requests.rbegin(); it != requests.rend(); ++it)
            {
                const auto& req = *it;

                ImGui::TableNextRow();

                // ID
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", req.id);

                // Timestamp
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(req.timestamp.c_str());

                // Method with color
                ImGui::TableSetColumnIndex(2);
                ImVec4 methodColor;
                if (req.method == "GET")
                    methodColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);  // Blue
                else if (req.method == "POST")
                    methodColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
                else if (req.method == "PUT")
                    methodColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
                else if (req.method == "DELETE")
                    methodColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);  // Red
                else
                    methodColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Gray

                ImGui::TextColored(methodColor, "%s", req.method.c_str());

                // Path
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(req.path.c_str());

                // Client IP
                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(req.remoteAddr.c_str());

                // Body (truncated if too long)
                ImGui::TableSetColumnIndex(5);
                if (!req.body.empty())
                {
                    std::string displayBody = req.body;
                    if (displayBody.length() > 100)
                    {
                        displayBody = displayBody.substr(0, 100) + "...";
                    }
                    ImGui::TextWrapped("%s", displayBody.c_str());
                }
                else
                {
                    ImGui::TextDisabled("(empty)");
                }
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}
