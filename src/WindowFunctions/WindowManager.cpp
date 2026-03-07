#include "pch.h"
#include "WindowFunctions/WindowManager.h"

WindowManager::WindowManager()
    : m_windows()
{
}

WindowManager::~WindowManager()
{
    m_windows.clear();
}

void WindowManager::AddWindow(const std::string& type, const std::string& menuName, const std::string& windowName, 
                              WindowFunction::WindowRenderFunction renderFunc)
{
    auto window = std::make_shared<WindowFunction>(type, menuName, windowName, renderFunc);
    m_windows.push_back(window);
}

void WindowManager::RemoveWindow(const std::string& windowName)
{
    auto it = std::find_if(m_windows.begin(), m_windows.end(),
        [&windowName](const std::shared_ptr<WindowFunction>& w) {
            return w->GetWindowName() == windowName;
        });

    if (it != m_windows.end())
    {
        m_windows.erase(it);
    }
}

void WindowManager::RenderAllWindows()
{
    for (auto& window : m_windows)
    {
        window->Render();
    }
}

WindowFunction* WindowManager::GetWindow(const std::string& windowName)
{
    auto it = std::find_if(m_windows.begin(), m_windows.end(),
        [&windowName](const std::shared_ptr<WindowFunction>& w) {
            return w->GetWindowName() == windowName;
        });

    if (it != m_windows.end())
    {
        return it->get();
    }

    return nullptr;
}

void WindowManager::EnableAllWindows()
{
    for (auto& window : m_windows)
    {
        window->SetEnabled(true);
    }
}

void WindowManager::DisableAllWindows()
{
    for (auto& window : m_windows)
    {
        window->SetEnabled(false);
    }
}

void WindowManager::PrintAllWindows() const
{
    fprintf(stdout, "WindowManager: %zu windows registered\n", m_windows.size());
    for (const auto& window : m_windows)
    {
        window->PrintInfo();
    }
}
