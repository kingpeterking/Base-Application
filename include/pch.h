#pragma once

// Windows headers
#ifdef _WIN32
    #define NOMINMAX
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

// GLFW
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

// SimpleIni
#include "SimpleIni.h"

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

// Custom OpenGL function wrappers
#include "OpenGLFunctions.h"

// Tools
#include "Tools/HTTPClient.h"
