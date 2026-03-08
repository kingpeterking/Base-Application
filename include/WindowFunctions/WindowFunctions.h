#pragma once

class WindowFunctions
{
public:
    WindowFunctions(class Application* app);
    ~WindowFunctions();

    // Window rendering functions
    void RenderMainMenu(bool* isOpen);
    void RenderDemoWindow(bool* isOpen);
    void RenderApplicationWindow(bool* isOpen);
    void RenderImPlotDemoWindow(bool* isOpen);
    void RenderURLRequestWindow(bool* isOpen);
    void RenderFileExplorerWindow(bool* isOpen);

private:
    class Application* m_app;
};
