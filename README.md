# Base Application - Dear ImGui CMake Project

A modern, distributable C++ application template using **Dear ImGui** for the UI framework and **CMake** for cross-platform builds.

## Features

- ✅ **Dear ImGui** - Immediate mode GUI framework
- ✅ **GLFW** - Window creation and event handling
- ✅ **OpenGL 3.2** - Hardware-accelerated rendering
- ✅ **CMake** - Cross-platform build system
- ✅ **C++17** - Modern C++ standard
- ✅ **Precompiled Headers** - Fast incremental builds
- ✅ **Self-contained** - No external dependencies to install
- ✅ **Application Class Architecture** - Clean, extensible design

## Project Structure

```
Base-Application/
├── CMakeLists.txt              # Build configuration
├── include/
│   ├── Application.h           # Main application class
│   └── pch.h                   # Precompiled headers
├── src/
│   ├── main.cpp                # Entry point
│   ├── Application.cpp         # Application implementation
│   └── pch.cpp                 # PCH source file
├── .gitignore                  # Git ignore rules
└── README.md                   # This file
```

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
cmake --build . --config Debug
```

#### macOS / Linux
```sh
mkdir build
cd build
cmake ..
cmake --build .
```

### Output
The executable will be located at:
- **Windows**: `build/Debug/ImGuiApp.exe`
- **macOS/Linux**: `build/ImGuiApp`

## Running the Application

### Windows
```sh
.\build\Debug\ImGuiApp.exe
```

### macOS / Linux
```sh
./build/ImGuiApp
```

You should see a window with:
- A demo ImGui window showcasing various UI elements
- An interactive "Hello, world!" panel with controls
- Real-time FPS counter

## Features Included

### Demo UI Elements
- ✅ Checkbox controls
- ✅ Slider inputs
- ✅ Color picker
- ✅ Button interactions
- ✅ Window management
- ✅ FPS monitoring

### ImGui Demo Window
The application includes ImGui's built-in demo window showing hundreds of UI widgets and capabilities.

## Extending the Application

### Adding Custom UI
Edit `src/Application.cpp` and modify the `RenderFrame()` method to add your own ImGui widgets:

```cpp
ImGui::Begin("My Custom Window");
ImGui::Text("Hello, Custom UI!");
ImGui::SliderFloat("My Slider", &myValue, 0.0f, 1.0f);
if (ImGui::Button("My Button")) {
    // Handle button click
}
ImGui::End();
```

### Adding Custom Logic
Extend the `Application` class to add application-specific functionality:

1. Add member variables to `include/Application.h`
2. Implement logic in `src/Application.cpp`
3. Call your functions from `RenderFrame()` or `Run()`

### Example: Adding a Data Container
```cpp
// In Application.h
class Application {
private:
    std::vector<std::string> myData;
    void UpdateData();
};

// In Application.cpp
void Application::UpdateData() {
    // Your custom logic here
}
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

- **Dear ImGui** v1.89.9 - UI framework
- **GLFW** 3.3.8 - Window & input handling
- **OpenGL** 3.2 - Rendering API

No pre-installation required!

## Performance

The project includes:
- ✅ **Precompiled headers** - Reduce compilation time significantly
- ✅ **Efficient rendering** - 60 FPS on most systems
- ✅ **Optimized builds** - Release builds available via CMake

### Build Times
- **First build**: ~20-30 seconds (dependencies compiled)
- **Incremental builds**: ~2-5 seconds (PCH reduces recompilation)

## Troubleshooting

### Build Errors
If you encounter build errors:
1. Delete the `build/` directory
2. Run CMake again: `cmake ..`
3. Rebuild: `cmake --build .`

### OpenGL Errors
If you get OpenGL context errors:
- Ensure your graphics drivers are up to date
- Try lowering the OpenGL version in `Application.cpp`
- Check that your GPU supports OpenGL 3.2+

### Missing Dependencies
- Ensure CMake is installed and in your PATH
- On Linux, install development libraries: `sudo apt install libxorg-dev`

## Cross-Platform Support

This project builds and runs on:
- ✅ Windows (7, 8, 10, 11)
- ✅ macOS (Intel and Apple Silicon)
- ✅ Linux (Ubuntu, Fedora, etc.)

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

## Getting Help

For issues or questions:
1. Check the troubleshooting section above
2. Review the ImGui/GLFW documentation
3. Open an issue on GitHub

## Author

Created as a modern template for ImGui-based applications.

---

**Ready to build?** Run `cmake .. && cmake --build .` in the build directory!
