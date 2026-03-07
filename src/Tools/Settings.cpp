#include "pch.h"
#include "Tools/Settings.h"
#include "Tools/SimpleIni.h"

Settings::Settings(const std::string& filePath)
    : m_filePath(filePath)
{
    m_pIni = new CSimpleIni();
}

Settings::~Settings()
{
    if (m_pIni)
    {
        delete m_pIni;
        m_pIni = nullptr;
    }
}

bool Settings::Load()
{
    if (!m_pIni)
        return false;

    SI_Error err = m_pIni->LoadFile(m_filePath.c_str());
    return (err >= 0);
}

bool Settings::Save()
{
    if (!m_pIni)
        return false;

    SI_Error err = m_pIni->SaveFile(m_filePath.c_str());
    return (err >= 0);
}

std::string Settings::GetString(const std::string& section, const std::string& key, const std::string& defaultValue)
{
    if (!m_pIni)
        return defaultValue;

    const char* value = m_pIni->GetValue(section.c_str(), key.c_str(), defaultValue.c_str());
    return value ? std::string(value) : defaultValue;
}

void Settings::SetString(const std::string& section, const std::string& key, const std::string& value)
{
    if (!m_pIni)
        return;

    m_pIni->SetValue(section.c_str(), key.c_str(), value.c_str());
}

int Settings::GetInt(const std::string& section, const std::string& key, int defaultValue)
{
    std::string value = GetString(section, key, std::to_string(defaultValue));
    try
    {
        return std::stoi(value);
    }
    catch (...)
    {
        return defaultValue;
    }
}

void Settings::SetInt(const std::string& section, const std::string& key, int value)
{
    SetString(section, key, std::to_string(value));
}

float Settings::GetFloat(const std::string& section, const std::string& key, float defaultValue)
{
    std::string value = GetString(section, key, std::to_string(defaultValue));
    try
    {
        return std::stof(value);
    }
    catch (...)
    {
        return defaultValue;
    }
}

void Settings::SetFloat(const std::string& section, const std::string& key, float value)
{
    SetString(section, key, std::to_string(value));
}

bool Settings::GetBool(const std::string& section, const std::string& key, bool defaultValue)
{
    std::string value = GetString(section, key, defaultValue ? "true" : "false");
    return (value == "true" || value == "1" || value == "yes" || value == "on");
}

void Settings::SetBool(const std::string& section, const std::string& key, bool value)
{
    SetString(section, key, value ? "true" : "false");
}

bool Settings::KeyExists(const std::string& section, const std::string& key)
{
    if (!m_pIni)
        return false;

    const char* value = m_pIni->GetValue(section.c_str(), key.c_str(), nullptr);
    return value != nullptr;
}

void Settings::DeleteKey(const std::string& section, const std::string& key)
{
    if (!m_pIni)
        return;

    m_pIni->Delete(section.c_str(), key.c_str());
}

void Settings::DeleteSection(const std::string& section)
{
    if (!m_pIni)
        return;

    m_pIni->Delete(section.c_str());
}
