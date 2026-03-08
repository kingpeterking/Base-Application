#include "pch.h"
#include "Tools/FileSystem.h"
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

FileSystem::FileSystem()
{
}

FileSystem::~FileSystem()
{
}

std::vector<std::string> FileSystem::ListDirectories(const std::string& path)
{
    std::vector<std::string> directories;
    
    try
    {
        if (!DirectoryExists(path))
            return directories;
        
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_directory())
            {
                directories.push_back(entry.path().filename().string());
            }
        }
        
        std::sort(directories.begin(), directories.end());
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return directories;
}

std::vector<FileInfo> FileSystem::ListFiles(const std::string& path, const std::string& extensionFilter)
{
    std::vector<FileInfo> files;
    
    try
    {
        if (!DirectoryExists(path))
            return files;
        
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                std::string filename = entry.path().filename().string();
                std::string extension = GetFileExtension(filename);
                
                // Apply extension filter if specified
                if (!extensionFilter.empty())
                {
                    // Handle multiple extensions separated by semicolons or commas
                    std::string filter = extensionFilter;
                    std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);
                    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                    
                    // Remove leading dot if present
                    if (!extension.empty() && extension[0] == '.')
                        extension = extension.substr(1);
                    if (!filter.empty() && filter[0] == '.')
                        filter = filter.substr(1);
                    
                    if (filter.find(extension) == std::string::npos)
                        continue;
                }
                
                FileInfo info;
                info.filename = filename;
                info.extension = extension;
                info.sizeBytes = fs::file_size(entry.path());
                info.lineCount = GetFileLineCount(entry.path().string());
                
                files.push_back(info);
            }
        }
        
        std::sort(files.begin(), files.end(), [](const FileInfo& a, const FileInfo& b) {
            return a.filename < b.filename;
        });
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return files;
}

FileInfo FileSystem::GetFileInfo(const std::string& filepath)
{
    FileInfo info;
    info.filename = GetFileName(filepath);
    info.extension = GetFileExtension(filepath);
    
    try
    {
        if (fs::exists(filepath))
        {
            info.sizeBytes = fs::file_size(filepath);
            info.lineCount = GetFileLineCount(filepath);
        }
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return info;
}

uint64_t FileSystem::GetFileSizeBytes(const std::string& filepath)
{
    try
    {
        if (fs::exists(filepath))
        {
            return fs::file_size(filepath);
        }
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return 0;
}

size_t FileSystem::GetFileLineCount(const std::string& filepath)
{
    size_t lineCount = 0;
    
    try
    {
        std::ifstream file(filepath);
        if (!file.is_open())
            return 0;
        
        std::string line;
        while (std::getline(file, line))
        {
            lineCount++;
        }
        
        file.close();
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return lineCount;
}

bool FileSystem::FileExists(const std::string& filepath)
{
    try
    {
        return fs::exists(filepath) && fs::is_regular_file(filepath);
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool FileSystem::DirectoryExists(const std::string& dirpath)
{
    try
    {
        return fs::exists(dirpath) && fs::is_directory(dirpath);
    }
    catch (const std::exception&)
    {
        return false;
    }
}

std::string FileSystem::ReadFileAsString(const std::string& filepath)
{
    std::string content;
    
    try
    {
        std::ifstream file(filepath);
        if (!file.is_open())
            return content;
        
        std::string line;
        while (std::getline(file, line))
        {
            content += line + "\n";
        }
        
        file.close();
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return content;
}

std::vector<std::string> FileSystem::ReadFileAsLines(const std::string& filepath)
{
    std::vector<std::string> lines;
    
    try
    {
        std::ifstream file(filepath);
        if (!file.is_open())
            return lines;
        
        std::string line;
        while (std::getline(file, line))
        {
            lines.push_back(line);
        }
        
        file.close();
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return lines;
}

char FileSystem::ReadFileAsCharacter(const std::string& filepath, size_t position)
{
    try
    {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open())
            return '\0';
        
        file.seekg(position);
        char c;
        if (file.get(c))
        {
            file.close();
            return c;
        }
        
        file.close();
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return '\0';
}

bool FileSystem::WriteFile(const std::string& filepath, const std::string& content, bool append)
{
    try
    {
        std::ios_base::openmode mode = append ? std::ios::app : std::ios::out;
        std::ofstream file(filepath, mode);
        
        if (!file.is_open())
            return false;
        
        file << content;
        file.close();
        
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

bool FileSystem::AppendToFile(const std::string& filepath, const std::string& content)
{
    return WriteFile(filepath, content, true);
}

bool FileSystem::DeleteFile(const std::string& filepath)
{
    try
    {
        if (fs::exists(filepath))
        {
            fs::remove(filepath);
            return true;
        }
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return false;
}

bool FileSystem::DeleteDirectory(const std::string& dirpath)
{
    try
    {
        if (fs::exists(dirpath))
        {
            fs::remove_all(dirpath);
            return true;
        }
    }
    catch (const std::exception&)
    {
        // Handle error silently
    }
    
    return false;
}

std::string FileSystem::GetCurrentDirectory()
{
    try
    {
        return fs::current_path().string();
    }
    catch (const std::exception&)
    {
        return "";
    }
}

bool FileSystem::SetCurrentDirectory(const std::string& path)
{
    try
    {
        fs::current_path(path);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

std::string FileSystem::JoinPath(const std::string& dir, const std::string& file)
{
    return (fs::path(dir) / file).string();
}

std::string FileSystem::GetFileExtension(const std::string& filename)
{
    size_t dotPos = filename.find_last_of('.');
    if (dotPos != std::string::npos && dotPos != 0)
    {
        return filename.substr(dotPos);
    }
    return "";
}

std::string FileSystem::GetFileName(const std::string& filepath)
{
    return fs::path(filepath).filename().string();
}

std::string FileSystem::GetParentDirectory(const std::string& path)
{
    try
    {
        fs::path p(path);
        fs::path parent = p.parent_path();

        if (parent.empty() || parent == p)
        {
            // Already at root, return the same path
            return path;
        }

        return parent.string();
    }
    catch (const std::exception&)
    {
        return path;
    }
}

std::string FileSystem::GetDirectoryPath(const std::string& filepath)
{
    return fs::path(filepath).parent_path().string();
}

std::string FileSystem::GetAbsolutePath(const std::string& relativePath)
{
    try
    {
        return fs::absolute(relativePath).string();
    }
    catch (const std::exception&)
    {
        return relativePath;
    }
}

std::string FileSystem::NormalizePath(const std::string& path)
{
    return fs::path(path).string();
}
