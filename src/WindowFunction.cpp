#include "pch.h"
#include "WindowFunction.h"

WindowFunction::WindowFunction()
    : m_type("Uncategorized"),
      m_menuName("Unnamed"),
      m_windowName("Unnamed"),
      m_isEnabled(false),
      m_renderFunc(nullptr)
{
}

WindowFunction::WindowFunction(const std::string& type, const std::string& menuName, const std::string& windowName, 
                               WindowRenderFunction renderFunc)
    : m_type(type),
      m_menuName(menuName),
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
    fprintf(stdout, "WindowFunction: Type=%s, Menu=%s, Window=%s, Enabled=%s\n",
            m_type.c_str(), m_menuName.c_str(), m_windowName.c_str(), m_isEnabled ? "true" : "false");
}
