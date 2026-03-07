#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "WindowManager.h"

WindowFunctions::WindowFunctions(Application* app)
    : m_app(app)
{
}

WindowFunctions::~WindowFunctions()
{
}

void WindowFunctions::RenderMainMenu(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Main Menu", isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Available Windows");
        ImGui::Separator();

        // Iterate through all windows and display toggle controls grouped by type
        const auto& allWindows = m_app->m_windowManager.GetAllWindows();

        // Collect unique types
        std::map<std::string, std::vector<std::shared_ptr<WindowFunction>>> windowsByType;
        for (const auto& window : allWindows)
        {
            // Skip the main menu itself
            if (window->GetWindowName() == "Main Menu")
                continue;

            windowsByType[window->GetType()].push_back(window);
        }

        // Render windows grouped by type
        for (const auto& [type, windows] : windowsByType)
        {
            if (ImGui::CollapsingHeader(type.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                for (const auto& window : windows)
                {
                    std::string windowName = window->GetWindowName();
                    bool isEnabled = window->IsEnabled();

                    // Create a unique ID for this checkbox
                    std::string checkboxLabel = windowName + "##window_toggle";

                    if (ImGui::Checkbox(checkboxLabel.c_str(), &isEnabled))
                    {
                        window->SetEnabled(isEnabled);
                    }
                }

                ImGui::Unindent();
            }
        }

        ImGui::Separator();
        ImGui::Text("Total Windows: %zu", allWindows.size() - 1); // -1 for the main menu itself

        ImGui::End();
    }
}

void WindowFunctions::RenderDemoWindow(bool* isOpen)
{
    ImGui::ShowDemoWindow(isOpen);
}

void WindowFunctions::RenderHelloWorldWindow(bool* isOpen)
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Hello, world!", isOpen))
    {
        ImGui::Text("This is some useful text.");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Window Controls"))
        {
            WindowFunction* demoWindow = m_app->m_windowManager.GetWindow("Demo Window");
            if (demoWindow)
            {
                bool demoEnabled = demoWindow->IsEnabled();
                if (ImGui::Checkbox("Show Demo Window", &demoEnabled))
                {
                    demoWindow->SetEnabled(demoEnabled);
                }
            }
        }

        if (ImGui::CollapsingHeader("Display Settings"))
        {
            ImGui::ColorEdit4("Clear Color", (float*)&m_app->m_clearColor);
        }

        if (ImGui::CollapsingHeader("Interaction"))
        {
            ImGui::SliderFloat("Float Slider", &f, 0.0f, 1.0f);
            if (ImGui::Button("Button"))
            {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("Counter = %d", counter);
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Separator();
        ImGui::Text("Performance");
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::End();
    }
}

void WindowFunctions::RenderApplicationInfoWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Application Info", isOpen))
    {
        ImGui::Text("Dear ImGui Application");
        ImGui::Separator();
        ImGui::Text("Windows Managed: %zu", m_app->m_windowManager.GetWindowCount());
        ImGui::Text("Framework: Dear ImGui");
        ImGui::Text("Rendering: OpenGL 3.2");
        ImGui::Text("Build: C++17 with CMake");
        ImGui::End();
    }
}

void WindowFunctions::RenderImPlotDemoWindow(bool* isOpen)
{
    m_app->m_implotClient.RenderDemoWindow(isOpen);
}

void WindowFunctions::RenderURLRequestWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("URL Request", isOpen))
    {
        ImGui::Text("HTTP Request Tool");
        ImGui::Separator();

        // URL input
        ImGui::Text("URL:");
        ImGui::InputText("##url_input", m_app->m_urlBuffer, m_app->URL_BUFFER_SIZE);

        ImGui::SameLine();

        // Send button
        if (ImGui::Button("Send Request") && !m_app->m_httpClient.IsLoading())
        {
            m_app->m_httpClient.PerformRequest(m_app->m_urlBuffer);
        }

        ImGui::Separator();

        // Status
        if (m_app->m_httpClient.IsLoading())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loading...");
        }
        else if (!m_app->m_httpClient.GetError().empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", m_app->m_httpClient.GetError().c_str());
        }
        else if (!m_app->m_httpClient.GetResponse().empty())
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Success! Response: %zu bytes", m_app->m_httpClient.GetResponse().size());
        }

        ImGui::Separator();
        ImGui::Text("Response:");

        // Response display with scrollbar
        ImGui::BeginChild("response_scroll", ImVec2(0, -50), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(m_app->m_httpClient.GetResponse().c_str(), m_app->m_httpClient.GetResponse().c_str() + m_app->m_httpClient.GetResponse().size());
        ImGui::EndChild();

        // Copy button
        if (!m_app->m_httpClient.GetResponse().empty())
        {
            if (ImGui::Button("Copy Response"))
            {
                ImGui::SetClipboardText(m_app->m_httpClient.GetResponse().c_str());
            }
            ImGui::SameLine();
        }

        if (ImGui::Button("Clear"))
        {
            m_app->m_httpClient.ClearAll();
        }

        ImGui::End();
    }
}
