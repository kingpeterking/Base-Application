# Window Management System - Developer Guide

The application now includes a modular window management system that separates ImGui window logic into reusable components.

## Architecture Overview

### Components

1. **WindowFunction** - Represents a single ImGui window
2. **WindowManager** - Manages a collection of WindowFunction instances
3. **Application** - Creates and coordinates window functions via the manager

## WindowFunction Class

### Purpose
Encapsulates all logic for a single ImGui window including:
- Rendering function (the ImGui code)
- Window name and menu name
- Visibility state
- Enable/disable functionality

### API Reference

```cpp
class WindowFunction {
public:
    using WindowRenderFunction = std::function<void(bool*)>;

    // Constructors
    WindowFunction();
    WindowFunction(const std::string& menuName, 
                   const std::string& windowName, 
                   WindowRenderFunction renderFunc);

    // Rendering
    void Render();  // Calls the render function if enabled

    // Getters
    const std::string& GetMenuName() const;
    const std::string& GetWindowName() const;
    bool IsEnabled() const;

    // Setters
    void SetEnabled(bool enabled);
    void ToggleEnabled();

    // Debug
    void PrintInfo() const;
};
```

### Usage Example

```cpp
// Create a window function with a lambda
auto renderFunc = [](bool* isOpen) {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("My Window", isOpen)) {
        ImGui::Text("Window content goes here");
        ImGui::End();
    }
};

WindowFunction myWindow("Tools", "My Window", renderFunc);
myWindow.Render();  // Renders if enabled
```

## WindowManager Class

### Purpose
Manages a collection of WindowFunction instances with the following features:
- Add/remove windows dynamically
- Render all windows with one call
- Query windows by name
- Enable/disable windows globally or individually

### API Reference

```cpp
class WindowManager {
public:
    WindowManager();
    ~WindowManager();

    // Add a window
    void AddWindow(const std::string& menuName, 
                   const std::string& windowName, 
                   WindowFunction::WindowRenderFunction renderFunc);

    // Remove a window by name
    void RemoveWindow(const std::string& windowName);

    // Render all enabled windows
    void RenderAllWindows();

    // Query windows
    WindowFunction* GetWindow(const std::string& windowName);
    const std::vector<std::shared_ptr<WindowFunction>>& GetAllWindows() const;

    // Control all windows
    void EnableAllWindows();
    void DisableAllWindows();

    // Info
    size_t GetWindowCount() const;
    void PrintAllWindows() const;
};
```

### Usage Example

```cpp
WindowManager manager;

// Add windows
manager.AddWindow("Tools", "Settings", [](bool* isOpen) {
    if (ImGui::Begin("Settings", isOpen)) {
        ImGui::Text("Settings panel");
        ImGui::End();
    }
});

manager.AddWindow("Tools", "Stats", [](bool* isOpen) {
    if (ImGui::Begin("Stats", isOpen)) {
        ImGui::Text("Statistics panel");
        ImGui::End();
    }
});

// In render loop
manager.RenderAllWindows();

// Manage windows
WindowFunction* settings = manager.GetWindow("Settings");
if (settings) {
    settings->SetEnabled(false);
}
```

## Application Integration

### Current Windows

The Application class now manages three windows:

#### 1. Demo Window
- **Name**: "Demo Window"
- **Menu**: "Windows"
- **Purpose**: ImGui's built-in demo showing all available widgets
- **Visibility**: Controlled by checkbox in Hello World window

#### 2. Hello World Window
- **Name**: "Hello World"
- **Menu**: "Windows"
- **Purpose**: Main control panel with:
  - Window visibility toggles
  - Display settings (color picker)
  - Interactive controls (slider, button, counter)
  - Performance metrics

#### 3. Application Info
- **Name**: "Application Info"
- **Menu**: "Windows"
- **Purpose**: Read-only information panel displaying:
  - Framework information
  - Window count
  - Rendering details

### Window Lifecycle

```
Application::Initialize()
    ↓
SetupImGui()
    ↓
SetupWindows()  ← Creates all WindowFunction instances
    ↓
LoadSettings()  ← Restores window visibility state
    ↓
Application::Run()
    ↓
RenderFrame()
    ↓
m_windowManager.RenderAllWindows()  ← Renders all enabled windows
    ↓
SaveSettings()  ← Persists window visibility state
```

## Creating Custom Windows

### Step 1: Add Window Creation in SetupWindows()

```cpp
void Application::SetupWindows()
{
    // ... existing windows ...

    m_windowManager.AddWindow("Custom", "My Custom Window", [this](bool* isOpen) {
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("My Custom Window", isOpen))
        {
            // Your ImGui code here
            ImGui::Text("Hello from custom window!");

            static float value = 0.5f;
            ImGui::SliderFloat("Value", &value, 0.0f, 1.0f);

            ImGui::End();
        }
    });
}
```

### Step 2: Add Settings Persistence

```cpp
void Application::LoadSettings()
{
    // ... existing code ...

    WindowFunction* customWindow = m_windowManager.GetWindow("My Custom Window");
    if (customWindow)
    {
        customWindow->SetEnabled(m_settings.GetBool("Windows", "ShowCustom", true));
    }
}

void Application::SaveSettings()
{
    // ... existing code ...

    WindowFunction* customWindow = m_windowManager.GetWindow("My Custom Window");
    if (customWindow)
    {
        m_settings.SetBool("Windows", "ShowCustom", customWindow->IsEnabled());
    }
}
```

### Step 3: Add Window Toggle in Main Panel

```cpp
// In Hello World window render function
if (ImGui::CollapsingHeader("Window Controls"))
{
    WindowFunction* customWindow = m_windowManager.GetWindow("My Custom Window");
    if (customWindow)
    {
        bool enabled = customWindow->IsEnabled();
        if (ImGui::Checkbox("Show Custom Window", &enabled))
        {
            customWindow->SetEnabled(enabled);
        }
    }
}
```

## Advanced Patterns

### Dynamic Window Creation

```cpp
void Application::AddDynamicWindow(const std::string& title)
{
    m_windowManager.AddWindow("Dynamic", title, [title](bool* isOpen) {
        if (ImGui::Begin(title.c_str(), isOpen))
        {
            ImGui::Text("Dynamic window: %s", title.c_str());
            ImGui::End();
        }
    });
}
```

### Window with State

```cpp
struct WindowState {
    float value = 0.5f;
    int count = 0;
};

// In SetupWindows
auto windowState = std::make_shared<WindowState>();
m_windowManager.AddWindow("Tools", "Stateful", [windowState](bool* isOpen) {
    if (ImGui::Begin("Stateful Window", isOpen))
    {
        ImGui::SliderFloat("Value", &windowState->value, 0.0f, 1.0f);
        if (ImGui::Button("Increment"))
            windowState->count++;
        ImGui::Text("Count: %d", windowState->count);
        ImGui::End();
    }
});
```

### Modal Windows

```cpp
m_windowManager.AddWindow("Modals", "Confirmation", [](bool* isOpen) {
    if (*isOpen) {
        if (ImGui::BeginPopupModal("Confirm Action", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Are you sure?");
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                *isOpen = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::End();
        }
    }
});
```

## Performance Considerations

1. **Enabled Windows Only** - Only enabled windows execute their render function
2. **Shared Pointers** - WindowFunction instances use shared_ptr for safe memory management
3. **Lambda Captures** - Avoid capturing large objects by value in window render functions
4. **Settings Caching** - Settings are loaded once at startup, saved once at shutdown

## Debugging

### Print All Windows

```cpp
m_windowManager.PrintAllWindows();
// Output: WindowManager: 3 windows registered
//         WindowFunction: Menu=Windows, Window=Demo Window, Enabled=true
//         WindowFunction: Menu=Windows, Window=Hello World, Enabled=true
//         WindowFunction: Menu=Windows, Window=Application Info, Enabled=true
```

### Inspect Specific Window

```cpp
WindowFunction* window = m_windowManager.GetWindow("Hello World");
if (window) {
    printf("Window enabled: %s\n", window->IsEnabled() ? "yes" : "no");
    window->PrintInfo();
}
```

## Best Practices

1. **Keep Windows Focused** - Each window should have a single responsibility
2. **Use Consistent Naming** - Use descriptive window and menu names
3. **Handle nullptr** - Always check if GetWindow() returns nullptr
4. **Persist Visibility** - Save and load window visibility state
5. **Avoid Heavy Processing** - Keep render functions lightweight
6. **Use Separators** - Organize complex windows with ImGui::Separator()
7. **Add Documentation** - Comment non-obvious window functionality

## File Organization

```
include/
├── WindowFunction.h     # WindowFunction class declaration
├── WindowManager.h      # WindowManager class declaration
└── Application.h        # Application uses WindowManager

src/
├── WindowFunction.cpp   # WindowFunction implementation
├── WindowManager.cpp    # WindowManager implementation
└── Application.cpp      # Contains SetupWindows() and window render code
```

---

**The modular window system makes it easy to add, remove, and manage ImGui windows!**
