# Base Application - Dear ImGui CMake Project

A modern, distributable C++ application template using **Dear ImGui** for the UI framework and **CMake** for cross-platform builds.

## Features

- ✅ **Dear ImGui** - Immediate mode GUI framework
- ✅ **GLFW** - Window creation and event handling
- ✅ **OpenGL 3.2** - Hardware-accelerated rendering with cross-platform support
- ✅ **CMake** - Cross-platform build system
- ✅ **C++17** - Modern C++ standard
- ✅ **Precompiled Headers** - Fast incremental builds
- ✅ **Settings Management** - INI file persistence system
- ✅ **Window Manager** - Modular ImGui window architecture
- ✅ **Main Menu** - Centralized window visibility control
- ✅ **ImPlot v0.16** - Professional data visualization with comprehensive demo examples
- ✅ **CURL** - HTTP/HTTPS request capabilities with Windows native SChannel SSL/TLS
- ✅ **HTTP Client Tool** - Full-featured HTTP client with multiple request types
  - Support for GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS methods
  - URL parameter builder with easy add/remove interface
  - Request payload input for POST/PUT/DELETE/PATCH
  - Response codes and error messages
  - CURL error reporting
  - **HTTPS support with no external dependencies**
- ✅ **Web Folder** - Organized web-related functionality ready for future expansion
- ✅ **Cross-Platform File System** - Complete file/directory operations API
- ✅ **File Explorer Tool** - Browse directories with file selection, favorites, and filtering
- ✅ **File Selection** - Select/deselect individual files or all at once
- ✅ **Favorite Directories** - Mark frequently-used directories for quick access
- ✅ **Auto-filtering** - File list updates instantly as you type filter criteria
- ✅ **Self-contained** - No external dependencies to install
- ✅ **Application Class Architecture** - Clean, extensible design
- ✅ **Cross-Platform OpenGL** - Smooth rendering across Windows, macOS, and Linux

## Project Structure

```
Base-Application/
├── CMakeLists.txt                      # Build configuration
├── include/
│   ├── Application.h                   # Main application class
│   ├── pch.h                           # Precompiled headers
│   ├── Tools/
│   │   ├── Settings.h                  # Settings management class
│   │   ├── SimpleIni.h                 # INI file parser
│   │   ├── ImPlotClient.h              # ImPlot visualization wrapper
│   │   ├── FileSystem.h                # Cross-platform file operations
│   │   └── Web/
│   │       └── HTTPClient.h            # HTTP/HTTPS client with REST support
│   └── WindowFunctions/
│       ├── OpenGLFunctions.h           # Cross-platform OpenGL function wrappers
│       ├── WindowFunction.h            # ImGui window wrapper class
│       ├── WindowManager.h             # Window manager class
│       └── WindowFunctions.h           # Window rendering orchestrator
├── src/
│   ├── main.cpp                        # Entry point
│   ├── Application.cpp                 # Application implementation
│   ├── pch.cpp                         # PCH source file
│   ├── Tools/
│   │   ├── Settings.cpp                # Settings implementation
│   │   ├── ImPlotClient.cpp            # ImPlot wrapper implementation
│   │   ├── FileSystem.cpp              # File system implementation
│   │   └── Web/
│   │       └── HTTPClient.cpp          # HTTP client implementation
│   └── WindowFunctions/
│       ├── WindowFunction.cpp          # Window function implementation
│       ├── WindowManager.cpp           # Window manager implementation
│       └── WindowFunctions.cpp         # Window rendering implementations
├── .gitignore                          # Git ignore rules
├── README.md                           # This file
├── SETTINGS_GUIDE.md                   # Settings usage documentation
├── WINDOW_MANAGER_GUIDE.md             # Window management documentation
├── FILESYSTEM_GUIDE.md                 # File system and explorer documentation
├── FILESYSTEM_IMPLEMENTATION.md        # Implementation details
└── IMPLEMENTATION_SUMMARY.md           # Implementation summary
```
│   ├── Tools/
│   │   ├── Settings.cpp                # Settings implementation
│   │   ├── HTTPClient.cpp              # HTTP client implementation
│   │   └── ImPlotClient.cpp            # ImPlot wrapper implementation
│   └── WindowFunctions/
│       ├── WindowFunction.cpp          # Window function implementation
│       ├── WindowManager.cpp           # Window manager implementation
│       └── WindowFunctions.cpp         # Window rendering implementations
├── .gitignore                          # Git ignore rules
├── README.md                           # This file
├── SETTINGS_GUIDE.md                   # Settings usage documentation
├── WINDOW_MANAGER_GUIDE.md             # Window management documentation
└── IMPLEMENTATION_SUMMARY.md           # Implementation details
```

## Architecture

The project is organized into three main areas:

### **Core Application** (`Application.h/cpp`)
- Handles application lifecycle (initialization, main loop, shutdown)
- Manages OpenGL context and GLFW window
- Orchestrates ImGui frame rendering
- Delegates window rendering to WindowFunctions

### **Tools** (`include/Tools/`, `src/Tools/`)
- **HTTPClient** - Network request handling via libcurl
- **ImPlotClient** - Data visualization wrapper
- **Settings** - Configuration persistence via INI files
- **SimpleIni** - Lightweight INI file parser

### **WindowFunctions** (`include/WindowFunctions/`, `src/WindowFunctions/`)
- **WindowFunction** - Encapsulates individual ImGui windows
- **WindowManager** - Manages collection of windows with type-based grouping
- **WindowFunctions** - Renders all window UI implementations

## Prerequisites

### Windows
- **Visual Studio 2019 or later** (with C++ workload)
- **CMake 3.14+** (available from [cmake.org](https://cmake.org/download/))
- **Git** (for cloning and version control)

### macOS
- **Xcode** or **Clang** compiler
- **CMake 3.14+**
- **Git**

### Linux
- **GCC** or **Clang** compiler
- **CMake 3.14+**
- **Git**
- **X11 development libraries** (for GLFW window creation)

## Building

### Clone the Repository
```sh
git clone https://github.com/kingpeterking/Base-Application.git
cd Base-Application
```

### Configure and Build

#### Windows (Visual Studio)
```sh
mkdir build
cd build
cmake ..
cmake --build . --config Debug     # For Debug build
cmake --build . --config Release   # For Release (optimized) build
```

#### macOS / Linux

**Debug Build:**
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

**Release Build:**
```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Output
The executable will be located at:
- **Windows Debug**: `build/bin/Debug/ImGuiApp.exe`
- **Windows Release**: `build/bin/Release/ImGuiApp.exe`
- **macOS/Linux Debug**: `build/bin/Debug/ImGuiApp`
- **macOS/Linux Release**: `build/bin/Release/ImGuiApp`

### Build Configurations
- **Debug**: Full debugging symbols, no optimization (development)
- **Release**: Optimized code, minimal debug info (production)

For detailed build configuration options, see [BUILD_GUIDE.md](BUILD_GUIDE.md)

## Running the Application

### Windows
```sh
# Debug version
.\build\bin\Debug\ImGuiApp.exe

# Release version (optimized)
.\build\bin\Release\ImGuiApp.exe
```

### macOS / Linux
```sh
# Debug version
./build/bin/Debug/ImGuiApp

# Release version (optimized)
./build/bin/Release/ImGuiApp
```
./build/ImGuiApp
```

You should see a window with:
- A demo ImGui window showcasing various UI elements
- An interactive "Hello, world!" panel with controls
- Real-time FPS counter
- Settings that persist across application runs

## Features Included

### Demo UI Elements
- ✅ Checkbox controls
- ✅ Slider inputs
- ✅ Color picker
- ✅ Button interactions
- ✅ Window management
- ✅ FPS monitoring
- ✅ **Main Menu** - Control all windows from one place

### Window Management System
The application uses a modular WindowFunction/WindowManager architecture:
- **WindowFunction** - Encapsulates ImGui window rendering logic
- **WindowManager** - Manages a collection of windows
- **Main Menu** - Central hub for controlling all windows
  - Shows all available windows
  - Toggle checkboxes to show/hide any window
  - Always visible by default for easy access
- **Current Windows**:
  - Main Menu - Central window control panel
  - Demo Window - ImGui's comprehensive widget showcase
  - Application - Combined info panel and interactive controls (merged Hello World + Application Info)
  - Plot Demo - Data visualization with ImPlot
  - URL Request - HTTP/HTTPS client tool
  - File Explorer - Directory browser with file filtering and details

All window visibility settings are persisted across application runs.

### ImGui Demo Window
The application includes ImGui's built-in demo window showing hundreds of UI widgets and capabilities.

### Settings Management
The application automatically persists settings to an INI file:
- UI state (window visibility)
- Display settings (colors)
- Custom application settings

## Settings Management

### Overview

The application includes a complete settings management system built on SimpleIni. Settings are automatically:
- **Loaded** from `settings.ini` on startup
- **Saved** to `settings.ini` on shutdown

### Quick Usage

```cpp
// In your application code
m_settings.SetString("Section", "Key", "Value");
m_settings.SetInt("Section", "Count", 42);
m_settings.SetFloat("Section", "Volume", 0.8f);
m_settings.SetBool("Section", "Enabled", true);

// Retrieve values
std::string value = m_settings.GetString("Section", "Key", "default");
int count = m_settings.GetInt("Section", "Count", 0);
float volume = m_settings.GetFloat("Section", "Volume", 0.5f);
bool enabled = m_settings.GetBool("Section", "Enabled", false);
```

### Current Settings

The application stores:

**UI State**
- `ShowDemoWindow` - Demo window visibility
- `ShowAnotherWindow` - Custom window visibility

**Display Settings**
- `ColorR/G/B/A` - Clear color components

### For More Details

See `SETTINGS_GUIDE.md` for:
- Complete API reference
- Advanced usage examples
- How to add new settings
- Best practices

## Extending the Application

### Modular Window System

The application uses a clean, scalable window architecture. All window rendering logic is centralized in `WindowFunctions` folder for better organization:

**How to Add a New Window:**

1. **Add rendering function to `WindowFunctions.h`:**
```cpp
void RenderMyCustomWindow(bool* isOpen);
```

2. **Implement in `WindowFunctions.cpp`:**
```cpp
void WindowFunctions::RenderMyCustomWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("My Custom Window", isOpen))
    {
        ImGui::Text("Your window content here");
        // Add your ImGui widgets here
        ImGui::End();
    }
}
```

3. **Add window delegation to `Application.h`:**
```cpp
void RenderMyCustomWindow(bool* isOpen);
```

4. **Implement delegation in `Application.cpp`:**
```cpp
void Application::RenderMyCustomWindow(bool* isOpen)
{
    m_windowFunctions->RenderMyCustomWindow(isOpen);
}
```

5. **Register in `Application::SetupWindows()`:**
```cpp
// Format: AddWindow(TYPE, MENU_NAME, WINDOW_NAME, RENDER_FUNCTION)
m_windowManager.AddWindow("Panels", "MyCategory", "My Custom Window", 
    [this](bool* isOpen) { RenderMyCustomWindow(isOpen); });
```

**Benefits:**
- ✅ All window rendering logic in one organized folder
- ✅ Each window rendering function clearly named
- ✅ Windows automatically grouped by type in Main Menu
- ✅ Easy to extend without modifying Application class
- ✅ Single Responsibility Principle

### Built-in Window Examples

The application includes six example windows:

**1. RenderMainMenu()**
- Central control panel for all windows
- Hierarchical organization by window type
- Visibility toggle for each window

**2. RenderDemoWindow()**
- ImGui's built-in demo window
- Shows all available ImGui widgets

**3. RenderApplicationWindow()**
- Combined window with:
  - Application metadata (framework, rendering, build info)
  - Window count information
  - Interactive controls (slider, button, color picker)
  - Real-time FPS monitoring
- Merged from Hello World + Application Info for better organization

**4. RenderImPlotDemoWindow()**
- ImPlot's comprehensive demo window
- Line plots, scatter plots, and bar charts

**5. RenderURLRequestWindow()**
- HTTP/HTTPS request client
- URL input and response display
- Copy response to clipboard
- Real-time request status

**6. RenderFileExplorerWindow()**
- Cross-platform file browser and explorer
- Directory navigation with path input
- File listing with extension filtering
- Detailed view: name, type, size (bytes), and line count
- Settings persistence: saves last directory and filter to ini file
- Automatically loads saved settings on next launch

## Tools Architecture

The `Tools` folder contains reusable service classes:

### HTTPClient
- Handles HTTP/HTTPS GET requests via libcurl
- Supports follow redirects and custom timeouts
- Thread-safe response buffering
- Error handling and status reporting

```cpp
HTTPClient client;
client.PerformRequest("https://api.example.com");
if (!client.GetError().empty()) {
    printf("Error: %s\n", client.GetError().c_str());
} else {
    printf("Response: %s\n", client.GetResponse().c_str());
}
```

### ImPlotClient
- Wraps ImPlot functionality
- Provides professional data visualization
- Shows comprehensive demo of all plot types

### FileSystem
- Cross-platform file and directory operations
- Complete API for file management:
  - **Directory Operations**: List directories, list files with extension filtering
  - **File Information**: Get size (bytes and line count), check existence
  - **File Reading**: Read as string, read as lines, read character by position
  - **File Writing**: Write new file, append to file
  - **File Deletion**: Delete files and directories
  - **Path Utilities**: Normalize paths, join paths, get extensions, absolute paths
- Handles cross-platform path differences automatically
- Exception-safe error handling

```cpp
// List files with extension filter
auto files = FileSystem::ListFiles("/path/to/dir", ".txt");

// Get file information
auto info = FileSystem::GetFileInfo("file.txt");
printf("File: %s, Size: %llu bytes, Lines: %zu\n", 
    info.filename.c_str(), info.sizeBytes, info.lineCount);

// Read file content
std::vector<std::string> lines = FileSystem::ReadFileAsLines("file.txt");

// Write to file
FileSystem::WriteFile("output.txt", "Hello, World!");
```

### Settings Manager
- Persists application state to INI file
- Type-safe access (string, int, float, bool)
- Automatic load/save on startup/shutdown
- Simple key-value storage with section organization

```cpp
Settings settings("app.ini");
settings.SetFloat("Display", "Volume", 0.8f);
float volume = settings.GetFloat("Display", "Volume", 0.5f);
settings.Save();
```

## Adding Custom Tools

To add a new tool service:

1. **Create header** in `include/Tools/MyTool.h`:
```cpp
class MyTool
{
public:
    MyTool();
    ~MyTool();

    void DoSomething();
private:
    // Your data
};
```

2. **Implement** in `src/Tools/MyTool.cpp`:
```cpp
// Implementation
```

3. **Add to Application**:
```cpp
// In include/Application.h
std::unique_ptr<MyTool> m_myTool;

// In src/Application.cpp constructor
m_myTool = std::make_unique<MyTool>();
```

## Using the Main Menu

The **Main Menu** window provides centralized control over all application windows organized by type:

### Features
- **Organized by Type** - Windows grouped into categories (Application, Panels, etc.)
- **Collapsible Sections** - Each type has a collapsible header showing all windows of that type
- **Window List** - Shows all available windows within each category
- **Toggle Controls** - Checkboxes to enable/disable each window
- **Always Available** - Main Menu is always visible by default

### Window Organization

**Example structure:**
```
Main Menu
├─ Application (collapsible)
│  └─ ☑ Main Menu
├─ Panels (collapsible)
│  ├─ ☑ Demo Window
│  ├─ ☑ Hello World
│  └─ ☑ Application Info
```

### Window Recovery
If you accidentally close a window and can't find it:
1. The **Main Menu** is always visible by default
2. Find the window's category section
3. Expand the section if collapsed
4. Check the checkbox next to the window name to show it again

### User Experience Flow
```
1. Application starts → Main Menu visible
2. User opens/closes windows via Main Menu toggles
3. Each window has a unique checkbox + Focus button
4. Already-open windows show a "Focus" button to bring to front
5. Window state automatically saved on application exit
6. On next launch, windows appear in the same state
```

### Extending with New Windows
When you add new windows (following the pattern in "Extending the Application" section), they automatically:
- Appear in the Main Menu under their type category
- Get toggle checkboxes for visibility control
- Get a Focus button if already open
- Have their visibility persisted in settings

### Window Types

The type parameter groups windows in the Main Menu. Common types:
- `"Application"` - Core application windows (Main Menu, Settings, etc.)
- `"Panels"` - Content/display panels (Demo, Info, etc.)
- `"Tools"` - Utility windows (Console, Debug, etc.)
- Custom types - Create your own categories as needed

**Example with different types:**
```cpp
// These will appear under "Tools" section in the menu
m_windowManager.AddWindow("Tools", "Development", "Debug Console", 
    [this](bool* isOpen) { RenderDebugConsole(isOpen); });

// These will appear under "Panels" section
m_windowManager.AddWindow("Panels", "Content", "Viewport", 
    [this](bool* isOpen) { RenderViewport(isOpen); });
```

## Build Configuration

### Change OpenGL Version
To target a different OpenGL version, edit `src/Application.cpp`:

```cpp
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // Major version
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);  // Minor version
```

And update the GLSL version in `SetupImGui()`:
```cpp
const char* glsl_version = "#version 450";  // Match OpenGL version
```

### Window Size
Edit the window dimensions in `src/Application.cpp`:
```cpp
m_window = glfwCreateWindow(1920, 1080, "My App", NULL, NULL);
```

## Dependencies

All dependencies are automatically fetched and built by CMake:

- **Dear ImGui** v1.91.0 - UI framework
- **GLFW** 3.3.8 - Window & input handling
- **OpenGL** 3.2 - Rendering API
- **ImPlot** v0.16 - Professional data visualization
- **libcurl** v8.5.0 - HTTP/HTTPS networking
- **SimpleIni** - INI file parser (bundled)

No pre-installation required!

## Performance

The project includes:
- ✅ **Precompiled headers** - Reduce compilation time significantly
- ✅ **Efficient rendering** - 60 FPS on most systems
- ✅ **Optimized builds** - Release builds available via CMake
- ✅ **Settings caching** - Load/save only at startup/shutdown

### Build Times
- **First build**: ~20-30 seconds (dependencies compiled)
- **Incremental builds**: ~2-5 seconds (PCH reduces recompilation)

## Rendering System

### Cross-Platform OpenGL Support

The application uses a custom OpenGL function wrapper (`include/WindowFunctions/OpenGLFunctions.h`) to ensure cross-platform compatibility:

- **Dynamic function loading** via `glfwGetProcAddress()`
- **No system header conflicts** - avoids OpenGL header conflicts with ImGui
- **Clean rendering pipeline** - proper framebuffer clearing and viewport setup
- **Artifact-free rendering** - smooth window redrawing when moving windows
- **Co-located with window infrastructure** - organized in WindowFunctions folder

### OpenGL Functions Provided

- `GLFunc::glViewport()` - Set rendering viewport
- `GLFunc::glClearColor()` - Set framebuffer clear color
- `GLFunc::glClear()` - Clear framebuffer buffers
- `GLFunc::GL_COLOR_BUFFER_BIT` - Color buffer constant
- `GLFunc::GL_DEPTH_BUFFER_BIT` - Depth buffer constant

These functions are automatically initialized in `Application::Initialize()` after creating the OpenGL context.

## Documentation

Comprehensive documentation is available in the following guides:

- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Debug and Release build configurations, compiler options, performance tuning
- **[SETTINGS_GUIDE.md](SETTINGS_GUIDE.md)** - Settings management and INI file usage
- **[WINDOW_MANAGER_GUIDE.md](WINDOW_MANAGER_GUIDE.md)** - Window system architecture and usage
- **[FILESYSTEM_GUIDE.md](FILESYSTEM_GUIDE.md)** - File system API and File Explorer usage
- **[FILESYSTEM_IMPLEMENTATION.md](FILESYSTEM_IMPLEMENTATION.md)** - Technical implementation details

## Troubleshooting

### Build Errors
If you encounter build errors:
1. Delete the `build/` directory
2. Run CMake again: `cmake ..`
3. Rebuild: `cmake --build .`
4. See [BUILD_GUIDE.md](BUILD_GUIDE.md) for configuration details

### OpenGL Errors
If you get OpenGL context errors:
- Ensure your graphics drivers are up to date
- Try lowering the OpenGL version in `Application.cpp`
- Check that your GPU supports OpenGL 3.2+

### Missing Dependencies
- Ensure CMake is installed and in your PATH
- On Linux, install development libraries: `sudo apt install libxorg-dev`

### Settings Not Persisting
- Ensure `settings.ini` file can be written to the executable directory
- Check file permissions on Linux/macOS
- The settings file is created on first run

## Cross-Platform Support

This project builds and runs on:
- ✅ Windows (7, 8, 10, 11)
- ✅ macOS (Intel and Apple Silicon)
- ✅ Linux (Ubuntu, Fedora, etc.)

## Project Organization

### Best Practices Implemented

1. **Precompiled Headers** - All common includes centralized in `pch.h`
2. **Single Responsibility** - Application handles UI, Settings handles persistence
3. **Resource Management** - RAII principles throughout
4. **Type Safety** - Settings API supports multiple data types
5. **Configuration** - Everything configurable via CMake

## License

This project uses open-source libraries:
- **Dear ImGui** - MIT License
- **GLFW** - Zlib License
- **CMake** - BSD License

See individual library documentation for details.

## Contributing

To extend this project:

1. Create a feature branch
2. Make your changes
3. Test on Windows, macOS, and Linux if possible
4. Commit and push to GitHub
5. Create a pull request

## Resources

### Dear ImGui
- [Official Documentation](https://github.com/ocornut/imgui)
- [API Reference](https://github.com/ocornut/imgui/blob/master/imgui.h)
- [Examples](https://github.com/ocornut/imgui/tree/master/examples)

### GLFW
- [Official Documentation](https://www.glfw.org/documentation.html)

### CMake
- [Official Documentation](https://cmake.org/cmake/help/latest/)

### Documentation Files
- [SETTINGS_GUIDE.md](SETTINGS_GUIDE.md) - Complete settings API documentation
- [FILESYSTEM_GUIDE.md](FILESYSTEM_GUIDE.md) - File handling and File Explorer window guide
- [WINDOW_MANAGER_GUIDE.md](WINDOW_MANAGER_GUIDE.md) - Window management system documentation

## Getting Help

For issues or questions:
1. Check the troubleshooting section above
2. Review [SETTINGS_GUIDE.md](SETTINGS_GUIDE.md) for settings-related questions
3. Review [FILESYSTEM_GUIDE.md](FILESYSTEM_GUIDE.md) for file handling questions
4. Review [WINDOW_MANAGER_GUIDE.md](WINDOW_MANAGER_GUIDE.md) for window management questions
5. Review the ImGui/GLFW documentation
6. Open an issue on GitHub

## Recent Updates

### Version 1.11.2
- ✅ Added Debug and Release build configurations
  - **Debug**: Full debugging symbols, runtime checks, no optimization (development)
  - **Release**: Optimized code, minimal debug info (production deployment)
  - Separate output directories for each configuration: `bin/Debug/` and `bin/Release/`
  - Automatic binary size/performance optimization in Release mode
- ✅ Compiler-specific optimization settings
  - **Windows (MSVC)**: `/Od` for Debug, `/O2` for Release
  - **Linux/macOS (GCC/Clang)**: `-O0` for Debug, `-O3` for Release
- ✅ New BUILD_GUIDE.md documentation
  - Complete guide to build configurations
  - Performance comparison (Debug vs Release)
  - Command-line building examples
- ✅ Updated README with build instructions
  - Separate commands for Debug and Release builds
  - Updated output directory structure
  - Cross-platform build examples

### Version 1.11.1
- ✅ Fixed HTTPS/SSL support for CURL
  - Changed from requiring external OpenSSL to Windows native **SChannel**
  - HTTPS requests now work without additional dependencies
  - Tested with `https://www.bbc.com` and other HTTPS APIs
  - Updated CMakeLists.txt with proper CURL SSL configuration
  - Cross-platform support: Windows (SChannel), macOS/Linux (native TLS)

### Version 1.11
- ✅ Enhanced HTTP Client with full REST API support
  - Support for all HTTP methods: GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS
  - URL parameter builder with add/remove interface
  - Request payload input for request bodies
  - Display HTTP status codes (200, 404, 500, etc.)
  - CURL error reporting on screen
  - Better response feedback with status indicators
- ✅ Reorganized web functionality into dedicated Web folder
  - Created `include/Tools/Web/` and `src/Tools/Web/` folders
  - Moved HTTPClient to Web folder structure
  - Foundation for future web-related features
- ✅ Improved HTTP Request window UI
  - Larger window for better visibility (1000x700)
  - Collapsible sections for parameters and payload
  - Clear visual feedback for request status
  - Emoji indicators (✓, ✗, ⚠, ⏳) for quick status recognition

### Version 1.10
- ✅ Added file selection checkboxes for each file in File Explorer
  - Select/deselect individual files with checkboxes
  - "Select All" button to mark all files at once
  - "Deselect All" button to clear all selections
  - Selection state managed per file
- ✅ Moved summary statistics to top of File Explorer window
  - File count, directory count, and total size now visible immediately
  - Displayed in blue text for visual distinction
- ✅ Added favorites system for frequently-used directories
  - Mark current directory as favorite with ★ Add button
  - Remove favorites with ✕ Remove button
  - Favorites dropdown selector for quick navigation
  - Automatic directory validation (error shown for deleted favorites)
  - Favorites persisted to INI file across sessions
- ✅ Implemented auto-refresh for extension filter
  - File list updates instantly as you type filter criteria
  - No need to click "Refresh" button manually
  - Filter persists across sessions

### Version 1.9
- ✅ Added cross-platform FileSystem utility class with comprehensive API
  - Directory and file listing with extension filtering
  - File reading (as string, lines, or by character position)
  - File information (size in bytes and line count)
  - File writing and deletion operations
  - Cross-platform path handling utilities
- ✅ Implemented File Explorer ImGui window
  - Browse directories with manual path input
  - Extension filter for targeted file listings
  - Detailed file view (name, type, size, line count)
  - Directory statistics and summary
  - Persists last directory and filter to settings.ini
  - Automatically loads saved settings on startup
- ✅ Added parent directory navigation (..)
  - Back button for traversal
  - GetParentDirectory() utility function
- ✅ Added drive/volume selection dropdown
  - Windows: Detects C:, D:, E: drives and other mounted drives
  - Linux: Shows /mnt, /media, and / root
  - macOS: Shows /Volumes and mount points

### Version 1.8
- ✅ Merged Hello World and Application Info windows into single Application window
- ✅ Consolidated all app info and interactive controls in one panel
- ✅ Moved OpenGLFunctions.h to WindowFunctions folder for better organization
- ✅ All rendering infrastructure now co-located in WindowFunctions folder
- ✅ Reduced from 6 to 5 windows for cleaner application interface

### Version 1.7
- ✅ Moved Settings and SimpleIni to Tools folder
- ✅ Moved WindowFunction and WindowManager to WindowFunctions folder
- ✅ Consolidated all rendering infrastructure in dedicated folders
- ✅ Professional folder organization (Core/Tools/WindowFunctions)
- ✅ Updated README with new architecture documentation

### Version 1.6
- ✅ Added Plot Demo window for data visualization
- ✅ Uses ImPlot v0.16 for professional charting
- ✅ Comprehensive demo showing all plot types
- ✅ Integrated HTTP client tool for network requests
- ✅ Settings persistence for all windows

### Version 1.5.1
- ✅ Removed Focus button (z-order control not fully working with ImGui)
- ✅ Simplified Main Menu to focus on window visibility control
- ✅ Clean checkbox interface for showing/hiding windows

### Version 1.5
- ✅ Added window type/category system for better organization
- ✅ Main Menu now groups windows by type in collapsible sections
- ✅ Types: "Application", "Panels", etc. (easily extensible)
- ✅ Cleaner menu UI with logical window grouping
- ✅ Updated AddWindow() to require type parameter

### Version 1.4
- ✅ Added Main Menu window - Central hub for window visibility control
- ✅ Toggle all windows on/off from the Main Menu
- ✅ Focus button to bring already-open windows to the front
- ✅ Main menu visibility persisted in settings
- ✅ Always visible by default for easy window recovery

### Version 1.3.1
- ✅ Refactored ImGui window rendering into separate member functions
- ✅ Cleaner SetupWindows() method - now shows all windows at a glance
- ✅ Each window has its own dedicated RenderXxxWindow() function
- ✅ Improved code organization and maintainability
- ✅ Easier to add new windows following the established pattern

### Version 1.3
- ✅ Added OpenGLFunctions.h - Cross-platform OpenGL function wrapper
- ✅ Fixed window redrawing artifacts - Proper framebuffer clearing with glClear()
- ✅ Fixed glViewport implementation - Viewport properly set to framebuffer size
- ✅ Improved rendering pipeline - glClearColor now applied from settings
- ✅ Dynamic function pointer loading via glfwGetProcAddress for maximum compatibility

### Version 1.2
- ✅ Added WindowFunction and WindowManager classes
- ✅ Modular window architecture with proper encapsulation
- ✅ Window visibility settings now persisted
- ✅ New WINDOW_MANAGER_GUIDE.md documentation
- ✅ Easy window creation and management system

### Version 1.1
- ✅ Added Settings management system with SimpleIni
- ✅ Application now persists UI state and display settings
- ✅ New SETTINGS_GUIDE.md documentation
- ✅ Centralized includes via precompiled headers

### Version 1.0
- ✅ Initial release with Dear ImGui + GLFW + CMake
- ✅ Cross-platform support
- ✅ Application class architecture

## Author

Created as a modern template for ImGui-based applications with professional settings management.

---

**Ready to build?** Run `cmake .. && cmake --build .` in the build directory!

**Need settings documentation?** Check out [SETTINGS_GUIDE.md](SETTINGS_GUIDE.md)

