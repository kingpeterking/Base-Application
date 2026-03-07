#pragma once

#include "pch.h"
#include "Settings.h"

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

    GLFWwindow* m_window;
    ImVec4 m_clearColor;
    bool m_showDemoWindow;
    bool m_showAnotherWindow;

    // Settings management
    Settings m_settings;

    // Settings keys
    void LoadSettings();
    void SaveSettings();
};
