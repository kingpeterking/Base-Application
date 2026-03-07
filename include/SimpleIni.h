#pragma once

// SimpleIni - Simplified INI file parser
// Compatible with Brodie Thiesfield's SimpleIni library

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

// Return codes
#define SI_OK       0
#define SI_FAIL    -1
#define SI_UPDATED  1

typedef int SI_Error;

typedef std::map<std::string, std::map<std::string, std::string>> IniData;

class CSimpleIni
{
public:
    CSimpleIni() : m_data() {}
    ~CSimpleIni() {}

    // Load INI file
    int LoadFile(const char* filename)
    {
        m_data.clear();
        std::ifstream file(filename);
        
        if (!file.is_open())
        {
            return SI_OK;  // File doesn't exist yet, return OK to create it
        }

        std::string line;
        std::string currentSection;

        while (std::getline(file, line))
        {
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Skip empty lines and comments
            if (line.empty() || line[0] == ';' || line[0] == '#')
                continue;

            // Check for section header
            if (line[0] == '[' && line[line.length() - 1] == ']')
            {
                currentSection = line.substr(1, line.length() - 2);
                m_data[currentSection] = std::map<std::string, std::string>();
            }
            // Check for key=value pair
            else if (!currentSection.empty())
            {
                size_t pos = line.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);

                    // Trim key and value
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);

                    m_data[currentSection][key] = value;
                }
            }
        }

        file.close();
        return SI_OK;
    }

    // Save INI file
    int SaveFile(const char* filename)
    {
        std::ofstream file(filename);
        
        if (!file.is_open())
            return SI_FAIL;

        for (const auto& section : m_data)
        {
            file << "[" << section.first << "]\n";
            for (const auto& keyValue : section.second)
            {
                file << keyValue.first << "=" << keyValue.second << "\n";
            }
            file << "\n";
        }

        file.close();
        return SI_OK;
    }

    // Get value
    const char* GetValue(const char* section, const char* key, const char* defaultValue = nullptr)
    {
        auto sectionIt = m_data.find(section);
        if (sectionIt == m_data.end())
            return defaultValue;

        auto keyIt = sectionIt->second.find(key);
        if (keyIt == sectionIt->second.end())
            return defaultValue;

        m_returnValue = keyIt->second;
        return m_returnValue.c_str();
    }

    // Set value
    void SetValue(const char* section, const char* key, const char* value)
    {
        m_data[section][key] = value ? value : "";
    }

    // Delete key
    void Delete(const char* section, const char* key = nullptr)
    {
        if (key == nullptr)
        {
            // Delete entire section
            m_data.erase(section);
        }
        else
        {
            // Delete specific key
            auto sectionIt = m_data.find(section);
            if (sectionIt != m_data.end())
            {
                sectionIt->second.erase(key);
            }
        }
    }

private:
    IniData m_data;
    mutable std::string m_returnValue;
};
