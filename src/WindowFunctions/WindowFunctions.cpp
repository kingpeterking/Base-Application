#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"

// ============================================================================
// WINDOW FUNCTIONS - MAIN FILE
// ============================================================================
// Constructor and Destructor only
// Individual window rendering functions are split into:
//   - WindowFunctions_Core.cpp (Main Menu, Demo, Application, ImPlot)
//   - WindowFunctions_Web.cpp (URL Request, Web Server)
//   - WindowFunctions_FileSystem.cpp (File Explorer, Log Viewer)
//   - WindowFunctions_Database.cpp (Database Connection, Connections Manager)
// ============================================================================

WindowFunctions::WindowFunctions(Application* app)
    : m_app(app)
{
}

WindowFunctions::~WindowFunctions()
{
}
