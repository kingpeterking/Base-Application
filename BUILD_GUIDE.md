# Build Configuration Guide

This guide explains how to build the Base Application in Debug and Release configurations.

## Build Types

### Debug Configuration
- **Use Case**: Development, testing, and debugging
- **Compiler Flags**: 
  - **Windows (MSVC)**: `/W4 /ZI /Od /RTC1`
    - `/W4`: All warnings
    - `/ZI`: Debug info (Edit and Continue)
    - `/Od`: No optimization
    - `/RTC1`: Runtime checks
  - **Linux/macOS (GCC/Clang)**: `-g -O0 -Wall -Wextra`
    - `-g`: Debug symbols
    - `-O0`: No optimization
    - `-Wall -Wextra`: All warnings
- **Output**: `bin/Debug/ImGuiApp.exe`
- **File Size**: Larger (includes debug symbols)
- **Performance**: Slower execution
- **Debugging**: Full debugging support

### Release Configuration
- **Use Case**: Production deployment
- **Compiler Flags**:
  - **Windows (MSVC)**: `/W4 /O2 /Zi`
    - `/W4`: All warnings
    - `/O2`: Optimize for speed
    - `/Zi`: Program database for debugging (optional)
  - **Linux/macOS (GCC/Clang)**: `-O3 -DNDEBUG -Wall -Wextra`
    - `-O3`: Aggressive optimization
    - `-DNDEBUG`: Disable assertions
    - `-Wall -Wextra`: All warnings
- **Output**: `bin/Release/ImGuiApp.exe`
- **File Size**: Smaller (optimized)
- **Performance**: Faster execution
- **Debugging**: Limited (can be enhanced with PDB on Windows)

## Building from Command Line

### CMake Configuration

**Debug Build:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

**Release Build:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Default Behavior
If `CMAKE_BUILD_TYPE` is not specified, the build defaults to **Release** for optimal performance.

## Building with Visual Studio

### Visual Studio IDE
1. Open CMakeLists.txt as a folder in Visual Studio
2. In the configuration dropdown at the top, select:
   - **x64-Debug** for Debug configuration
   - **x64-Release** for Release configuration
3. Build using Ctrl+Shift+B

### Switching Configurations
The output binaries are automatically separated:
- Debug: `build/bin/Debug/ImGuiApp.exe`
- Release: `build/bin/Release/ImGuiApp.exe`

## Output Directories

Builds are automatically organized by configuration:
```
build/
├── bin/
│   ├── Debug/
│   │   └── ImGuiApp.exe     (Debug build)
│   └── Release/
│       └── ImGuiApp.exe     (Release build)
├── lib/
│   ├── Debug/               (Dependencies)
│   └── Release/             (Dependencies)
```

## Performance Comparison

| Aspect | Debug | Release |
|--------|-------|---------|
| Compile Time | Faster | Slower (optimizations) |
| Binary Size | Larger | Smaller |
| Runtime Performance | Slower | Faster |
| Debugging | Full support | Limited |
| Use Case | Development | Production |

## Recommended Usage

- **During Development**: Use Debug builds for full debugging capabilities
- **Performance Testing**: Use Release builds for accurate performance metrics
- **Production Deployment**: Use Release builds for optimal performance
- **Bug Investigation**: Use Debug builds with Visual Studio debugger

## Compiler Warnings

Both configurations compile with `W4`/`-Wall -Wextra` to catch potential issues. Fix all warnings during development.

## Environment Variables

You can also control the build type via environment variables:

```bash
# Windows CMD
set CMAKE_BUILD_TYPE=Release

# Windows PowerShell
$env:CMAKE_BUILD_TYPE = "Release"

# Linux/macOS
export CMAKE_BUILD_TYPE=Release
```

## Optimization Levels

### Debug (`/Od` or `-O0`)
- No optimization
- Symbols are not removed
- Code execution matches source code exactly
- Best for debugging

### Release (`/O2` or `-O3`)
- Loop optimizations
- Function inlining
- Constant folding
- Dead code elimination
- Best for performance

## Notes

- The build system automatically detects your platform (Windows/macOS/Linux)
- Compiler flags are optimized for MSVC on Windows and GCC/Clang on Unix
- All dependencies (ImGui, GLFW, OpenGL, etc.) are built with matching configurations
- Debug info is preserved in Release builds for PDB generation on Windows
