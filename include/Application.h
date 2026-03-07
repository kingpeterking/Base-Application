#pragma once

#include "pch.h"
#include "Settings.h"
#include "WindowManager.h"

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

    // Window rendering functions
    void RenderMainMenu(bool* isOpen);
    void RenderDemoWindow(bool* isOpen);
    void RenderHelloWorldWindow(bool* isOpen);
    void RenderApplicationInfoWindow(bool* isOpen);
    void RenderImPlotDemoWindow(bool* isOpen);
    void RenderURLRequestWindow(bool* isOpen);

    GLFWwindow* m_window;
    ImVec4 m_clearColor;
    bool m_showMainMenu;

    // URL Request state
    static constexpr size_t URL_BUFFER_SIZE = 512;
    static constexpr size_t RESPONSE_BUFFER_SIZE = 8192;
    char m_urlBuffer[URL_BUFFER_SIZE];
    std::string m_responseBuffer;
    bool m_isLoadingRequest;
    std::string m_requestError;

    // Window management
    WindowManager m_windowManager;

    // Settings management
    Settings m_settings;

    // Settings keys
    void LoadSettings();
    void SaveSettings();
};
