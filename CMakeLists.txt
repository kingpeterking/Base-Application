cmake_minimum_required(VERSION 3.14)
project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Build-specific compiler flags
if(MSVC)
    # Visual Studio specific flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        # Debug: Include debug info, no optimization, runtime checks
        set(CMAKE_CXX_FLAGS_DEBUG "/W4 /ZI /Od /RTC1" CACHE STRING "")
        message(STATUS "Building DEBUG configuration with full debugging support")
    else()
        # Release: Optimization, minimal debug info
        set(CMAKE_CXX_FLAGS_RELEASE "/W4 /O2 /Zi" CACHE STRING "")
        message(STATUS "Building RELEASE configuration with optimizations")
    endif()
else()
    # GCC/Clang flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra" CACHE STRING "")
        message(STATUS "Building DEBUG configuration with full debugging support")
    else()
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -Wall -Wextra" CACHE STRING "")
        message(STATUS "Building RELEASE configuration with optimizations")
    endif()
endif()

# Set output directories based on build type
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")

# Fetch and build dependencies
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# GLFW configuration (set before FetchContent_Declare)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW documentation" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW examples" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Generate installation target for GLFW" FORCE)

# CURL configuration (set before FetchContent_Declare) - Enable SSL/TLS support
set(CURL_ENABLE_SSL ON CACHE BOOL "Enable SSL/TLS support in CURL" FORCE)
# Use Windows native SChannel instead of OpenSSL
set(CURL_USE_SCHANNEL ON CACHE BOOL "Use Windows native SChannel for SSL/TLS" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "Disable OpenSSL" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "Build curl executable" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build CURL tests" FORCE)

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
    src/Tools/Settings.cpp
    src/WindowFunctions/WindowFunction.cpp
    src/WindowFunctions/WindowManager.cpp
    src/Tools/Web/HTTPClient.cpp
    src/Tools/ImPlotClient.cpp
    src/Tools/FileSystem.cpp
    src/WindowFunctions/WindowFunctions.cpp
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

