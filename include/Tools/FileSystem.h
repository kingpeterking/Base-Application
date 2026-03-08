#pragma once

#include <string>
#include <vector>
#include <cstdint>

struct FileInfo
{
    std::string filename;
    std::string extension;
    uint64_t sizeBytes;
    size_t lineCount;
};

class FileSystem
{
public:
    FileSystem();
    ~FileSystem();

    // Directory operations
    static std::vector<std::string> ListDirectories(const std::string& path);
    static std::vector<FileInfo> ListFiles(const std::string& path, const std::string& extensionFilter = "");
    
    // File information
    static FileInfo GetFileInfo(const std::string& filepath);
    static uint64_t GetFileSizeBytes(const std::string& filepath);
    static size_t GetFileLineCount(const std::string& filepath);
    static bool FileExists(const std::string& filepath);
    static bool DirectoryExists(const std::string& dirpath);
    
    // File reading
    static std::string ReadFileAsString(const std::string& filepath);
    static std::vector<std::string> ReadFileAsLines(const std::string& filepath);
    static char ReadFileAsCharacter(const std::string& filepath, size_t position);
    
    // File writing
    static bool WriteFile(const std::string& filepath, const std::string& content, bool append = false);
    static bool AppendToFile(const std::string& filepath, const std::string& content);
    
    // File deletion
    static bool DeleteFile(const std::string& filepath);
    static bool DeleteDirectory(const std::string& dirpath);
    
    // Path utilities
    static std::string GetCurrentDirectory();
    static bool SetCurrentDirectory(const std::string& path);
    static std::string JoinPath(const std::string& dir, const std::string& file);
    static std::string GetFileExtension(const std::string& filename);
    static std::string GetFileName(const std::string& filepath);
    static std::string GetParentDirectory(const std::string& path);
    static std::string GetDirectoryPath(const std::string& filepath);
    static std::string GetAbsolutePath(const std::string& relativePath);

private:
    static std::string NormalizePath(const std::string& path);
};
