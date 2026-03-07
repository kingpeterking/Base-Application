#pragma once

#include "pch.h"

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
};
