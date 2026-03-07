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
    void RenderDemoWindow(bool* isOpen);
    void RenderHelloWorldWindow(bool* isOpen);
    void RenderApplicationInfoWindow(bool* isOpen);

    GLFWwindow* m_window;
    ImVec4 m_clearColor;

    // Window management
    WindowManager m_windowManager;

    // Settings management
    Settings m_settings;

    // Settings keys
    void LoadSettings();
    void SaveSettings();
};
