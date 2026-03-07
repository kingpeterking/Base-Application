#pragma once

#include "pch.h"
#include "Settings.h"
#include "WindowManager.h"
#include "Tools/HTTPClient.h"
#include "Tools/ImPlotClient.h"
#include "WindowFunctions/WindowFunctions.h"

struct GLFWwindow;

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void SetupImGui();
    void RenderFrame();
    void SetupWindows();

    // Window rendering functions (delegated to WindowFunctions)
    void RenderMainMenu(bool* isOpen);
    void RenderDemoWindow(bool* isOpen);
    void RenderHelloWorldWindow(bool* isOpen);
    void RenderApplicationInfoWindow(bool* isOpen);
    void RenderImPlotDemoWindow(bool* isOpen);
    void RenderURLRequestWindow(bool* isOpen);

    GLFWwindow* m_window;
    ImVec4 m_clearColor;
    bool m_showMainMenu;

    // Tool clients
    HTTPClient m_httpClient;
    ImPlotClient m_implotClient;
    std::unique_ptr<WindowFunctions> m_windowFunctions;

    // URL Request state
    static constexpr size_t URL_BUFFER_SIZE = 512;
    char m_urlBuffer[URL_BUFFER_SIZE];

    // Window management
    WindowManager m_windowManager;

    // Settings management
    Settings m_settings;

    // Settings keys
    void LoadSettings();
    void SaveSettings();

    // Friend class for window rendering
    friend class WindowFunctions;
};

