cmake_minimum_required(VERSION 3.14)
project(ImGuiApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# MSVC Runtime Library Configuration
if(MSVC)
    # Use dynamic runtime library for both Debug and Release
    # This prevents linker issues with runtime symbols
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")

    # Set warning level and optimization flags
    add_compile_options(/W4 /EHsc)

    if(CMAKE_BUILD_TYPE MATCHES Debug)
        # Debug: No optimization, full debugging, runtime checks
        add_compile_options(/Od /RTC1 /ZI)
        message(STATUS "Building DEBUG configuration with full debugging support")
    else()
        # Release: Optimization, minimal debug info
        add_compile_options(/O2 /Zi)
        message(STATUS "Building RELEASE configuration with optimizations")
    endif()
else()
    # GCC/Clang flags
    if(CMAKE_BUILD_TYPE MATCHES Debug)
        add_compile_options(-g -O0 -Wall -Wextra)
        message(STATUS "Building DEBUG configuration with full debugging support")
    else()
        add_compile_options(-O3 -DNDEBUG -Wall -Wextra)
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
set(BUILD_CURL_EXE OFF CACHE BOOL "Build curl executable" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Build CURL tests" FORCE)
set(ENABLE_CURLDEBUG OFF CACHE BOOL "Disable CURL debug" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library" FORCE)

# Platform-specific SSL/TLS backend configuration
if(WIN32)
    # Windows: Use native SChannel (no external dependencies)
    set(CURL_USE_SCHANNEL ON CACHE BOOL "Use Windows native SChannel for SSL/TLS" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "Disable OpenSSL on Windows" FORCE)
    set(CURL_USE_SECTRANSP OFF CACHE BOOL "Disable Secure Transport on Windows" FORCE)
    message(STATUS "CURL SSL/TLS: Using Windows native SChannel")
elseif(APPLE)
    # macOS: Use native Secure Transport (no external dependencies)
    set(CURL_USE_SECTRANSP ON CACHE BOOL "Use macOS native Secure Transport for SSL/TLS" FORCE)
    set(CURL_USE_OPENSSL OFF CACHE BOOL "Disable OpenSSL on macOS" FORCE)
    set(CURL_USE_SCHANNEL OFF CACHE BOOL "Disable SChannel on macOS" FORCE)
    message(STATUS "CURL SSL/TLS: Using macOS native Secure Transport")
else()
    # Linux/Unix (including Debian): Use OpenSSL
    set(CURL_USE_OPENSSL ON CACHE BOOL "Use OpenSSL for SSL/TLS on Linux" FORCE)
    set(CURL_USE_SCHANNEL OFF CACHE BOOL "Disable SChannel on Linux" FORCE)
    set(CURL_USE_SECTRANSP OFF CACHE BOOL "Disable Secure Transport on Linux" FORCE)
    message(STATUS "CURL SSL/TLS: Using OpenSSL (ensure libssl-dev is installed)")
endif()

# SimpleIni - header-only library from Brodie Thiesfield
# We'll create local SimpleIni headers

# Dear ImGui (using docking branch for window docking features)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)

# ImPlot (using master for compatibility with ImGui docking branch)
FetchContent_Declare(
    implot
    GIT_REPOSITORY https://github.com/epezent/implot.git
    GIT_TAG master
)

# CURL
FetchContent_Declare( 
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_5_0
)

# cpp-httplib (HTTP server library)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)

# Make dependencies available
FetchContent_MakeAvailable(imgui glfw implot curl httplib)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Create the executable
add_executable(ImGuiApp
    src/pch.cpp
    src/main.cpp
    src/Application.cpp
    src/Tools/Settings.cpp
    src/Tools/Logger.cpp
    src/Tools/Web/WebServer.cpp
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
    ${httplib_SOURCE_DIR}
)

# Enable precompiled headers
target_precompile_headers(ImGuiApp PRIVATE include/pch.h)

# Set as default startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ImGuiApp)

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

