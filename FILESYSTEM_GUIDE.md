# FileSystem Guide - Cross-Platform File Handling

This guide covers the `FileSystem` utility class and the File Explorer ImGui window that enable comprehensive file and directory operations across Windows, macOS, and Linux.

## Overview

The FileSystem class provides a complete, cross-platform API for:
- Directory listing and navigation
- File operations (read, write, delete)
- File information queries (size, line count)
- Path utilities and manipulation
- Automatic exception handling

The File Explorer window provides an interactive ImGui interface for browsing directories and viewing file details with optional filtering.

## FileSystem Class API

### Directory Operations

#### ListDirectories
```cpp
static std::vector<std::string> FileSystem::ListDirectories(const std::string& path);
```
Returns a sorted vector of subdirectory names in the specified path.

```cpp
// Example
std::vector<std::string> dirs = FileSystem::ListDirectories("/home/user/documents");
for (const auto& dir : dirs) {
    std::cout << "[DIR] " << dir << std::endl;
}
```

#### ListFiles
```cpp
static std::vector<FileInfo> FileSystem::ListFiles(
    const std::string& path, 
    const std::string& extensionFilter = ""
);
```
Returns a sorted vector of `FileInfo` structures for all files matching the optional extension filter.

**Extension Filter Format:**
- Single extension: `.txt`
- Multiple extensions: `.txt;.md` or `.cpp;.h;.hpp`
- Case-insensitive: `.TXT` and `.txt` match the same files

```cpp
// Example: List all text files
std::vector<FileInfo> textFiles = FileSystem::ListFiles("/home/user/docs", ".txt");

// Example: List C++ source files
std::vector<FileInfo> cppFiles = FileSystem::ListFiles("/src", ".cpp;.h");

// Example: List all files
std::vector<FileInfo> allFiles = FileSystem::ListFiles("/src", "");
```

**FileInfo Structure:**
```cpp
struct FileInfo
{
    std::string filename;        // Just the filename (no path)
    std::string extension;       // File extension (e.g., ".txt")
    uint64_t sizeBytes;         // File size in bytes
    size_t lineCount;           // Number of lines in the file
};
```

### File Information

#### GetFileInfo
```cpp
static FileInfo FileSystem::GetFileInfo(const std::string& filepath);
```
Returns complete information for a single file.

```cpp
auto info = FileSystem::GetFileInfo("/home/user/file.txt");
std::cout << "File: " << info.filename << std::endl;
std::cout << "Size: " << info.sizeBytes << " bytes" << std::endl;
std::cout << "Lines: " << info.lineCount << std::endl;
```

#### GetFileSizeBytes
```cpp
static uint64_t FileSystem::GetFileSizeBytes(const std::string& filepath);
```
Returns file size in bytes.

```cpp
uint64_t size = FileSystem::GetFileSizeBytes("data.bin");
std::cout << "File size: " << size << " bytes" << std::endl;
```

#### GetFileLineCount
```cpp
static size_t FileSystem::GetFileLineCount(const std::string& filepath);
```
Returns the number of lines in a text file.

```cpp
size_t lines = FileSystem::GetFileLineCount("source.cpp");
std::cout << "Source file has " << lines << " lines" << std::endl;
```

#### FileExists
```cpp
static bool FileSystem::FileExists(const std::string& filepath);
```
Checks if a file exists and is accessible.

```cpp
if (FileSystem::FileExists("config.ini")) {
    std::cout << "Config file found!" << std::endl;
}
```

#### DirectoryExists
```cpp
static bool FileSystem::DirectoryExists(const std::string& dirpath);
```
Checks if a directory exists and is accessible.

```cpp
if (FileSystem::DirectoryExists("/home/user/data")) {
    std::cout << "Data directory exists" << std::endl;
}
```

### File Reading

#### ReadFileAsString
```cpp
static std::string FileSystem::ReadFileAsString(const std::string& filepath);
```
Reads entire file content into a single string.

```cpp
std::string content = FileSystem::ReadFileAsString("document.txt");
std::cout << content << std::endl;
```

#### ReadFileAsLines
```cpp
static std::vector<std::string> FileSystem::ReadFileAsLines(const std::string& filepath);
```
Reads file as vector of strings, one per line.

```cpp
std::vector<std::string> lines = FileSystem::ReadFileAsLines("config.ini");
for (size_t i = 0; i < lines.size(); ++i) {
    std::cout << (i + 1) << ": " << lines[i] << std::endl;
}
```

#### ReadFileAsCharacter
```cpp
static char FileSystem::ReadFileAsCharacter(
    const std::string& filepath, 
    size_t position
);
```
Reads a single character at the specified byte position.

```cpp
// Read first character
char first = FileSystem::ReadFileAsCharacter("file.txt", 0);

// Read character at position 100
char c = FileSystem::ReadFileAsCharacter("file.txt", 100);
```

### File Writing

#### WriteFile
```cpp
static bool FileSystem::WriteFile(
    const std::string& filepath, 
    const std::string& content, 
    bool append = false
);
```
Writes content to a file. If `append` is false, overwrites existing file. If true, appends to end.

Returns true on success, false on failure.

```cpp
// Create new file
if (FileSystem::WriteFile("output.txt", "Hello, World!")) {
    std::cout << "File written successfully" << std::endl;
}

// Append to existing file
FileSystem::WriteFile("log.txt", "New log entry\n", true);
```

#### AppendToFile
```cpp
static bool FileSystem::AppendToFile(const std::string& filepath, const std::string& content);
```
Convenience function that appends content to a file. Equivalent to `WriteFile(filepath, content, true)`.

```cpp
FileSystem::AppendToFile("debug.log", "[INFO] Application started\n");
```

### File and Directory Deletion

#### DeleteFile
```cpp
static bool FileSystem::DeleteFile(const std::string& filepath);
```
Permanently deletes a file. Returns true on success.

```cpp
if (FileSystem::DeleteFile("temporary.tmp")) {
    std::cout << "Temporary file deleted" << std::endl;
}
```

#### DeleteDirectory
```cpp
static bool FileSystem::DeleteDirectory(const std::string& dirpath);
```
Deletes an empty directory. Returns false if directory contains files.

```cpp
if (FileSystem::DeleteDirectory("/tmp/empty_folder")) {
    std::cout << "Directory deleted" << std::endl;
} else {
    std::cout << "Directory not empty or doesn't exist" << std::endl;
}
```

### Path Utilities

#### GetCurrentDirectory
```cpp
static std::string FileSystem::GetCurrentDirectory();
```
Returns the current working directory as an absolute path.

```cpp
std::string cwd = FileSystem::GetCurrentDirectory();
std::cout << "Current directory: " << cwd << std::endl;
```

#### SetCurrentDirectory
```cpp
static bool FileSystem::SetCurrentDirectory(const std::string& path);
```
Changes the current working directory. Returns true on success.

```cpp
if (FileSystem::SetCurrentDirectory("/home/user/projects")) {
    std::cout << "Changed to projects directory" << std::endl;
}
```

#### JoinPath
```cpp
static std::string FileSystem::JoinPath(
    const std::string& dir, 
    const std::string& file
);
```
Joins directory and file/subdirectory into a proper path with correct separators.

**Cross-platform safe**: Handles path separators automatically.

```cpp
// Produces correct path for current platform
std::string fullPath = FileSystem::JoinPath("/home/user", "documents");
// Windows: C:\Users\user\documents (if in that location)
// Unix: /home/user/documents
```

#### GetFileExtension
```cpp
static std::string FileSystem::GetFileExtension(const std::string& filename);
```
Extracts the file extension (including the dot).

```cpp
std::string ext = FileSystem::GetFileExtension("document.txt");
// Returns: ".txt"

std::string noExt = FileSystem::GetFileExtension("README");
// Returns: "" (empty string)
```

#### GetParentDirectory
```cpp
static std::string FileSystem::GetParentDirectory(const std::string& path);
```
Returns the parent directory path. Safe at root - returns the same path if already at root.

```cpp
std::string parent = FileSystem::GetParentDirectory("/home/user/documents");
// Returns: "/home/user"

std::string root = FileSystem::GetParentDirectory("/");
// Returns: "/" (same path - already at root)
```

## File Explorer ImGui Window

The File Explorer window provides an interactive interface for browsing files and directories.

### Features

1. **Directory Navigation**
   - Manual path input field
   - Browse button to set to current directory
   - **Back button to navigate to parent directory**
   - Refresh button to reload file list
   - **Click on ".." to navigate to parent directory**
   - Click on directories to navigate into them

2. **File Information Display**
   - Organized in a table with columns:
     - Name: Filename or [DIR] for directories
     - Type: File extension
     - Size: File size in bytes
     - Lines: Number of lines in file

3. **Extension Filtering**
   - Optional extension filter (e.g., `.txt` or `.cpp;.h`)
   - Real-time filtering of file list
   - Case-insensitive matching

4. **Settings Persistence**
   - Automatically saves current directory to `settings.ini`
   - Automatically saves extension filter to `settings.ini`
   - Loads saved settings on startup
   - "Save Defaults" button to manually save current state

5. **Statistics Summary**
   - Shows total file count
   - Shows total directory count
   - Calculates and displays total size in KB

### Window Usage

#### Opening the File Explorer
1. Click "Main Menu" window
2. Expand the "Tools" → "Files" section
3. Check "File Explorer" checkbox
4. Or use `Application::RenderFileExplorerWindow()`

#### Navigating Directories
1. Enter a path in the "Current Directory" field
2. Click "Refresh" to load files
3. **Click the "Back" button to go up one directory level**
4. **Or click on ".." in the file list to navigate to parent directory**
5. Click on a directory name in the table to enter it
6. Use the "Browse" button to jump to current working directory

#### Filtering Files
1. Enter an extension filter: `.txt`, `.cpp;.h`, etc.
2. Click "Refresh" to apply filter
3. Leave empty to show all files

#### Saving Settings
- Manually click "Save Defaults" to persist current directory and filter
- Or they're saved automatically when application exits

### ImGui Implementation

```cpp
void WindowFunctions::RenderFileExplorerWindow(bool* isOpen)
{
    static char currentPath[512] = "";
    static char extensionFilter[256] = "";
    static std::vector<FileInfo> fileList;
    static std::vector<std::string> directoryList;
    static bool needsRefresh = true;

    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("File Explorer", isOpen))
    {
        // Load saved settings on first run
        if (needsRefresh && currentPath[0] == '\0')
        {
            std::string savedPath = m_app->m_settings.GetString("FileExplorer", "LastDirectory", 
                FileSystem::GetCurrentDirectory());
            std::string savedFilter = m_app->m_settings.GetString("FileExplorer", "ExtensionFilter", "");

            strcpy_s(currentPath, sizeof(currentPath), savedPath.c_str());
            strcpy_s(extensionFilter, sizeof(extensionFilter), savedFilter.c_str());
            needsRefresh = false;
        }

        // UI controls...
        // Directory path input and buttons
        // Extension filter input
        // Save defaults button
        // File list table

        ImGui::End();
    }
}
```

## Common Use Cases

### List all text files in a directory
```cpp
auto textFiles = FileSystem::ListFiles("/path/to/dir", ".txt");
for (const auto& file : textFiles) {
    std::cout << file.filename << " (" << file.sizeBytes << " bytes)" << std::endl;
}
```

### Copy a file
```cpp
std::string content = FileSystem::ReadFileAsString("source.txt");
FileSystem::WriteFile("destination.txt", content);
```

### Count lines in a source file
```cpp
size_t lines = FileSystem::GetFileLineCount("main.cpp");
std::cout << "main.cpp has " << lines << " lines of code" << std::endl;
```

### Create a file tree browser
```cpp
void BrowseDirectory(const std::string& path) {
    auto dirs = FileSystem::ListDirectories(path);
    auto files = FileSystem::ListFiles(path);
    
    for (const auto& dir : dirs) {
        std::cout << "[DIR] " << dir << std::endl;
        BrowseDirectory(FileSystem::JoinPath(path, dir));
    }
    
    for (const auto& file : files) {
        std::cout << "[FILE] " << file.filename << " (" 
                  << file.sizeBytes << " bytes)" << std::endl;
    }
}
```

### Log file operations
```cpp
std::string GetTimestamp() {
    auto now = std::time(nullptr);
    auto tm = std::localtime(&now);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buffer);
}

void LogMessage(const std::string& message) {
    std::string logEntry = "[" + GetTimestamp() + "] " + message + "\n";
    FileSystem::AppendToFile("app.log", logEntry);
}
```

## Cross-Platform Behavior

The FileSystem class uses C++17 `<filesystem>` (std::filesystem) to handle platform differences automatically:

| Aspect | Windows | macOS/Linux |
|--------|---------|-------------|
| Path separator | `\` | `/` |
| Home directory | `C:\Users\username` | `/home/username` |
| Temp directory | `%TEMP%` | `/tmp` |
| Line endings | `\r\n` (CRLF) | `\n` (LF) |
| Path handling | Via `std::filesystem::path` | Via `std::filesystem::path` |

The API abstracts these differences - path separators are handled automatically, and line ending differences are preserved in file content (not modified).

## Error Handling

All FileSystem operations are exception-safe:
- Return `false` or empty results on failure
- Don't throw exceptions
- Log errors silently
- Return sensible defaults

```cpp
// Safe - won't throw, just returns empty string
std::string content = FileSystem::ReadFileAsString("nonexistent.txt");
if (content.empty()) {
    std::cout << "File not found or empty" << std::endl;
}
```

## Performance Considerations

1. **Directory Listing**: O(n) where n = number of files/directories
2. **File Reading**: O(file_size) - entire file must be read into memory
3. **Line Counting**: O(file_size) - reads file to count newlines
4. **Filtering**: O(n*m) where n = files, m = filter patterns

For large directories or files:
- Consider caching results
- Use filters to reduce list size
- Stream large files instead of reading entirely

## INI File Storage

The File Explorer automatically persists two settings:

```ini
[FileExplorer]
LastDirectory=/home/user/documents
ExtensionFilter=.txt;.md
```

These are loaded automatically on window startup and saved when:
- "Save Defaults" button is clicked
- Application exits (via Application::SaveSettings)

## See Also

- [SETTINGS_GUIDE.md](SETTINGS_GUIDE.md) - Settings persistence system
- [WINDOW_MANAGER_GUIDE.md](WINDOW_MANAGER_GUIDE.md) - Window management
- [README.md](README.md) - Main project documentation
- C++ Reference: https://en.cppreference.com/w/cpp/filesystem

## License

FileSystem class is part of the Base Application project. See LICENSE file for details.
