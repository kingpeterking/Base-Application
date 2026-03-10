#pragma once

// Windows headers
#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

// Standard library
#include <cstdio>
#include <string>
#include <sstream>
#include <iomanip>
#include <map>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <numeric>

// GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// SimpleIni
#include "Tools/SimpleIni.h"

// WindowFunctions
#include "WindowFunctions/WindowFunction.h"

// Dear ImGui - define math operators before including imgui.h
#define IMGUI_DEFINE_MATH_OPERATORS

// Tell ImGui OpenGL3 backend not to use any loader
#define IMGUI_IMPL_OPENGL_LOADER_NONE

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// ImPlot
#include "implot.h"

// CURL
#include <curl/curl.h>

// ODBC constants (needed before nanodbc)
#ifdef _WIN32
    #include <sql.h>
    #include <sqlext.h>
#else
    #include <sql.h>
    #include <sqlext.h>
#endif

// nanodbc (ODBC database access)
#include <nanodbc/nanodbc.h>

// Custom OpenGL function wrappers
#include "WindowFunctions/OpenGLFunctions.h"

// Tools
#include "Tools/Web/HTTPClient.h"
#include "Tools/ImPlotClient.h"
#include "Tools/FileSystem.h"
#include "Tools/Database/Field.h"
#include "Tools/Database/Table.h"
#include "Tools/Database/Database.h"
#include "Tools/Database/DatabaseManager.h"

// Forward declaration for WindowFunctions
class WindowFunctions;
