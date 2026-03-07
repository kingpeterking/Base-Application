cmake_minimum_required(VERSION 3.14)
project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch and build dependencies
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# GLFW configuration (set before FetchContent_Declare)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target for GLFW" FORCE)

# SimpleIni - header-only library from Brodie Thiesfield
# We'll create local SimpleIni headers

# Dear ImGui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.0
)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)

# ImPlot
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG v0.16
)

# CURL
FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_5_0
)

# Make dependencies available
FetchContent_MakeAvailable(imgui glfw implot curl)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Create the executable
add_executable(ImGuiApp
    src/pch.cpp
    src/main.cpp
    src/Application.cpp
    src/Settings.cpp
    src/WindowFunction.cpp
    src/WindowManager.cpp
    src/Tools/HTTPClient.cpp
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${implot_SOURCE_DIR}/implot.cpp
    ${implot_SOURCE_DIR}/implot_items.cpp
    ${implot_SOURCE_DIR}/implot_demo.cpp
)

# Include directories
target_include_directories(ImGuiApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
    ${implot_SOURCE_DIR}
    ${curl_SOURCE_DIR}/include
)

# Enable precompiled headers
target_precompile_headers(ImGuiApp PRIVATE include/pch.h)

# Platform-specific linkage
if(WIN32)
    # Windows has opengl32.lib by default
    target_link_libraries(ImGuiApp PRIVATE
        opengl32
        user32
        gdi32
        shell32
        glfw
        libcurl
    )
elseif(APPLE)
    target_link_libraries(ImGuiApp PRIVATE
        OpenGL::OpenGL
        glfw
        libcurl
    )
else()
    target_link_libraries(ImGuiApp PRIVATE
        OpenGL::OpenGL
        glfw
        libcurl
    )
endif()

