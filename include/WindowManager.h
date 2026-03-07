#pragma once

#include <vector>
#include <memory>
#include "WindowFunction.h"

class WindowManager
{
public:
    WindowManager();
    ~WindowManager();

    // Add a window function
    void AddWindow(const std::string& menuName, const std::string& windowName, 
                   WindowFunction::WindowRenderFunction renderFunc);

    // Remove a window by name
    void RemoveWindow(const std::string& windowName);

    // Render all enabled windows
    void RenderAllWindows();

    // Get window by name
    WindowFunction* GetWindow(const std::string& windowName);

    // Get all windows
    const std::vector<std::shared_ptr<WindowFunction>>& GetAllWindows() const { return m_windows; }

    // Enable/disable all windows
    void EnableAllWindows();
    void DisableAllWindows();

    // Get window count
    size_t GetWindowCount() const { return m_windows.size(); }

    // Debug
    void PrintAllWindows() const;

private:
    std::vector<std::shared_ptr<WindowFunction>> m_windows;
};
