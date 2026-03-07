#include "pch.h"
#include "WindowFunction.h"

WindowFunction::WindowFunction()
    : m_menuName("Unnamed"),
      m_windowName("Unnamed"),
      m_isEnabled(false),
      m_renderFunc(nullptr)
{
}

WindowFunction::WindowFunction(const std::string& menuName, const std::string& windowName, 
                               WindowRenderFunction renderFunc)
    : m_menuName(menuName),
      m_windowName(windowName),
      m_isEnabled(true),
      m_renderFunc(renderFunc)
{
}

WindowFunction::~WindowFunction()
{
}

void WindowFunction::Render()
{
    if (m_isEnabled && m_renderFunc)
    {
        m_renderFunc(&m_isEnabled);
    }
}

void WindowFunction::PrintInfo() const
{
    fprintf(stdout, "WindowFunction: Menu=%s, Window=%s, Enabled=%s\n",
            m_menuName.c_str(), m_windowName.c_str(), m_isEnabled ? "true" : "false");
}
