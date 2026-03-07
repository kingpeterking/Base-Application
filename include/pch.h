#pragma once

// Windows headers
#ifdef _WIN32
    #define NOMINMAX
    #include <windows.h>
#endif

// Standard library
#include <cstdio>

// Tell GLFW not to include OpenGL headers
#define GLFW_INCLUDE_NONE

// GLFW
#include <GLFW/glfw3.h>

// Dear ImGui - define math operators before including imgui.h
#define IMGUI_DEFINE_MATH_OPERATORS

// Tell ImGui OpenGL3 backend not to use any loader
#define IMGUI_IMPL_OPENGL_LOADER_NONE

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
