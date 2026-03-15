# Settings Management - Usage Guide

The application now includes a complete INI settings management system using the SimpleIni library.

## Overview

The `Settings` class provides an easy-to-use wrapper around SimpleIni for storing and retrieving application settings in INI format.

## Files Added

- **`include/SimpleIni.h`** - Lightweight INI file parser implementation
- **`include/Settings.h`** - Settings wrapper class header
- **`src/Settings.cpp`** - Settings wrapper class implementation

## How It Works

### Automatic Settings Persistence

The application automatically:
1. **Loads** settings from `settings.ini` on startup
2. **Saves** settings to `settings.ini` on shutdown

### Settings Structure

Settings are organized in sections with key-value pairs:

```ini
[UI]
ShowDemoWindow=true
ShowAnotherWindow=false

[Display]
ColorR=0.450000
ColorG=0.550000
ColorB=0.600000
ColorA=1.000000
```

## Using Settings in Your Code

### Basic Usage

```cpp
// Access the settings object from Application class
Settings m_settings;

// Load from file
m_settings.Load();

// Get values
std::string appName = m_settings.GetString("App", "Name", "MyApp");
int windowWidth = m_settings.GetInt("Window", "Width", 1280);
float volume = m_settings.GetFloat("Audio", "Volume", 0.5f);
bool enableVSync = m_settings.GetBool("Graphics", "VSync", true);

// Set values
m_settings.SetString("App", "Name", "MyApp");
m_settings.SetInt("Window", "Width", 1920);
m_settings.SetFloat("Audio", "Volume", 0.75f);
m_settings.SetBool("Graphics", "VSync", false);

// Save to file
m_settings.Save();
```

### Settings API Reference

#### Loading and Saving

```cpp
bool Settings::Load();           // Load settings from file
bool Settings::Save();           // Save settings to file
```

#### Getting Values

```cpp
std::string GetString(const std::string& section, const std::string& key, 
                     const std::string& defaultValue = "");
int GetInt(const std::string& section, const std::string& key, 
           int defaultValue = 0);
float GetFloat(const std::string& section, const std::string& key, 
               float defaultValue = 0.0f);
bool GetBool(const std::string& section, const std::string& key, 
             bool defaultValue = false);
```

#### Setting Values

```cpp
void SetString(const std::string& section, const std::string& key, 
               const std::string& value);
void SetInt(const std::string& section, const std::string& key, int value);
void SetFloat(const std::string& section, const std::string& key, float value);
void SetBool(const std::string& section, const std::string& key, bool value);
```

#### Key Management

```cpp
bool KeyExists(const std::string& section, const std::string& key);
void DeleteKey(const std::string& section, const std::string& key);
void DeleteSection(const std::string& section);
```

## Example: Adding a New Setting

### 1. Add to Application Header

```cpp
class Application {
private:
    std::string m_userName;
    int m_maxPlayers;
};
```

### 2. Update LoadSettings()

```cpp
void Application::LoadSettings()
{
    m_settings.Load();
    m_userName = m_settings.GetString("Player", "Name", "Player1");
    m_maxPlayers = m_settings.GetInt("Game", "MaxPlayers", 4);
}
```

### 3. Update SaveSettings()

```cpp
void Application::SaveSettings()
{
    m_settings.SetString("Player", "Name", m_userName);
    m_settings.SetInt("Game", "MaxPlayers", m_maxPlayers);
    m_settings.Save();
}
```

### 4. Use in ImGui UI

```cpp
static char userName[256] = "";
ImGui::InputText("Username", userName, 256);
if (ImGui::Button("Save"))
{
    m_userName = userName;
}
```

## Current Implementation

The application currently stores:

- **UI State**
  - `ShowDemoWindow` - Visibility of demo window
  - `ShowAnotherWindow` - Visibility of custom window

- **Display Settings**
  - `ColorR` - Red component of clear color
  - `ColorG` - Green component of clear color
  - `ColorB` - Blue component of clear color
  - `ColorA` - Alpha component of clear color

## SimpleIni Features

The bundled SimpleIni implementation supports:

✅ Load/save INI files
✅ Section-based organization
✅ Key-value pairs
✅ Comments (lines starting with `;` or `#`)
✅ Automatic whitespace trimming
✅ Case-sensitive keys and sections

## File Location

The `settings.ini` file is created in the same directory as the executable:

- Windows: `build/Debug/settings.ini`
- macOS/Linux: `build/settings.ini`

## Advanced: Custom Section Management

```cpp
// Create new sections on the fly
m_settings.SetString("MySection", "MyKey", "MyValue");

// Check if section exists before using
if (m_settings.KeyExists("MySection", "MyKey"))
{
    std::string value = m_settings.GetString("MySection", "MyKey");
}

// Delete specific settings
m_settings.DeleteKey("MySection", "MyKey");
m_settings.DeleteSection("MySection");
```

## Performance Notes

- Settings are loaded once at startup
- Settings are saved once at shutdown
- In-memory operations are instant
- INI file I/O is only performed on explicit Load()/Save() calls

## Extending Settings

To add more sophisticated settings management:

1. Extend the `Settings` class
2. Add validation methods
3. Implement versioning for backward compatibility
4. Add encryption for sensitive data

---

**Settings are now automatically managed and persisted across application runs!**
