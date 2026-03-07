#pragma once

#include <string>
#include <functional>

class WindowFunction
{
public:
    using WindowRenderFunction = std::function<void(bool*)>;

    WindowFunction();
    WindowFunction(const std::string& type, const std::string& menuName, const std::string& windowName, WindowRenderFunction renderFunc);
    ~WindowFunction();

    // Render the window
    void Render();

    // Getters
    const std::string& GetType() const { return m_type; }
    const std::string& GetMenuName() const { return m_menuName; }
    const std::string& GetWindowName() const { return m_windowName; }
    bool IsEnabled() const { return m_isEnabled; }

    // Setters
    void SetEnabled(bool enabled) { m_isEnabled = enabled; }
    void ToggleEnabled() { m_isEnabled = !m_isEnabled; }

    // Debug
    void PrintInfo() const;

private:
    std::string m_type;
    std::string m_menuName;
    std::string m_windowName;
    bool m_isEnabled;
    WindowRenderFunction m_renderFunc;
};
