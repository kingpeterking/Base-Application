#pragma once

#include <string>

class Settings
{
public:
    Settings(const std::string& filePath = "settings.ini");
    ~Settings();

    // Load settings from file
    bool Load();

    // Save settings to file
    bool Save();

    // String settings
    std::string GetString(const std::string& section, const std::string& key, const std::string& defaultValue = "");
    void SetString(const std::string& section, const std::string& key, const std::string& value);

    // Integer settings
    int GetInt(const std::string& section, const std::string& key, int defaultValue = 0);
    void SetInt(const std::string& section, const std::string& key, int value);

    // Float settings
    float GetFloat(const std::string& section, const std::string& key, float defaultValue = 0.0f);
    void SetFloat(const std::string& section, const std::string& key, float value);

    // Boolean settings
    bool GetBool(const std::string& section, const std::string& key, bool defaultValue = false);
    void SetBool(const std::string& section, const std::string& key, bool value);

    // Check if key exists
    bool KeyExists(const std::string& section, const std::string& key);

    // Delete key
    void DeleteKey(const std::string& section, const std::string& key);

    // Delete entire section
    void DeleteSection(const std::string& section);

private:
    std::string m_filePath;
    class CSimpleIni* m_pIni;
};
