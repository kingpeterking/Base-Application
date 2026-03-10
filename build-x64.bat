@echo off
REM Build script for 64-bit Windows build using Ninja
REM Usage:
REM   build-x64.bat        - Incremental build (fast, rebuilds only changed files)
REM   build-x64.bat clean  - Full clean build (slow, rebuilds everything)

echo ================================
echo Building 64-bit version...
echo ================================

REM Find Visual Studio installation
for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do (
    set "VSPATH=%%i"
)

if not defined VSPATH (
    echo ERROR: Visual Studio not found!
    exit /b 1
)

echo Found Visual Studio at: %VSPATH%

REM Initialize x64 environment
call "%VSPATH%\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Check if clean build requested or if CMakeCache doesn't exist
if /i "%1"=="clean" (
    echo Clean build requested...
    if exist build (
        echo Cleaning build directory...
        rmdir /s /q build
    )
    set NEED_CONFIGURE=1
) else (
    if not exist build\CMakeCache.txt (
        echo CMakeCache.txt not found, configuring...
        set NEED_CONFIGURE=1
    ) else (
        echo Incremental build ^(use "build-x64.bat clean" for full rebuild^)
        set NEED_CONFIGURE=0
    )
)

REM Configure with CMake if needed
if "%NEED_CONFIGURE%"=="1" (
    echo.
    echo Configuring CMake ^(64-bit^)...
    set Platform=
    cmake -G Ninja -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_GENERATOR_PLATFORM=""

    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: CMake configuration failed!
        exit /b %ERRORLEVEL%
    )
)

REM Build
echo.
echo Building...
cmake --build build

if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Build failed!
    exit /b %ERRORLEVEL%
)

echo.
echo ================================
echo Build completed successfully!
echo Executable: build\bin\Debug\ImGuiApp.exe
echo ================================
