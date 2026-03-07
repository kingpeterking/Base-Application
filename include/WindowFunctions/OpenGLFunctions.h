#pragma once

#include <GLFW/glfw3.h>
#include <cstdint>

// OpenGL function pointers obtained via glfwGetProcAddress
// This avoids conflicts with system OpenGL headers while still providing
// the functions we need when using IMGUI_IMPL_OPENGL_LOADER_NONE

namespace GLFunc
{
    // Core OpenGL functions we need
    typedef void (APIENTRY* PFNGLVIEWPORTPROC)(int x, int y, int width, int height);
    typedef void (APIENTRY* PFNGLCLEARCOLORPROC)(float red, float green, float blue, float alpha);
    typedef void (APIENTRY* PFNGLCLEARPROC)(unsigned int mask);

    // Function pointers
    inline PFNGLVIEWPORTPROC glViewport_ptr = nullptr;
    inline PFNGLCLEARCOLORPROC glClearColor_ptr = nullptr;
    inline PFNGLCLEARPROC glClear_ptr = nullptr;

    // Initialize function pointers
    inline void InitFunctions()
    {
        glViewport_ptr = (PFNGLVIEWPORTPROC)glfwGetProcAddress("glViewport");
        glClearColor_ptr = (PFNGLCLEARCOLORPROC)glfwGetProcAddress("glClearColor");
        glClear_ptr = (PFNGLCLEARPROC)glfwGetProcAddress("glClear");
    }

    // Wrapper functions
    inline void glViewport(int x, int y, int width, int height)
    {
        if (glViewport_ptr) glViewport_ptr(x, y, width, height);
    }

    inline void glClearColor(float red, float green, float blue, float alpha)
    {
        if (glClearColor_ptr) glClearColor_ptr(red, green, blue, alpha);
    }

    // OpenGL constants
    constexpr unsigned int GL_COLOR_BUFFER_BIT = 0x00004000;
    constexpr unsigned int GL_DEPTH_BUFFER_BIT = 0x00000100;

    inline void glClear(unsigned int mask)
    {
        if (glClear_ptr) glClear_ptr(mask);
    }
}
