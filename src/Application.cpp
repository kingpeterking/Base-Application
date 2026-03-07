#include "pch.h"
#include "Application.h"

Application::Application()
    : m_window(nullptr),
      m_clearColor(0.45f, 0.55f, 0.60f, 1.00f),
      m_showMainMenu(true),
      m_settings("settings.ini")
{
}

Application::~Application()
{
}

bool Application::Initialize()
{
    // Setup GLFW error callback
    glfwSetErrorCallback([](int error, const char* description) {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    });

    if (!glfwInit())
        return false;

    // GL 3.2 + GLSL 150 with compatibility profile
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create window with graphics context
    m_window = glfwCreateWindow(1280, 720, "Dear ImGui Example", NULL, NULL);
    if (m_window == NULL)
        return false;

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL function pointers
    GLFunc::InitFunctions();

    SetupImGui();
    SetupWindows();
    LoadSettings();
    return true;
}

void Application::SetupImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Application::Run()
{
    // Main loop
    while (!glfwWindowShouldClose(m_window))
    {
        glfwPollEvents();
        RenderFrame();
    }

    // Save settings before closing
    SaveSettings();
}

void Application::RenderFrame()
{
    // Get framebuffer size
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);

    // Set OpenGL viewport
    GLFunc::glViewport(0, 0, display_w, display_h);

    // Clear framebuffer
    GLFunc::glClearColor(m_clearColor.x, m_clearColor.y, m_clearColor.z, m_clearColor.w);
    GLFunc::glClear(GLFunc::GL_COLOR_BUFFER_BIT | GLFunc::GL_DEPTH_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render all window functions
    m_windowManager.RenderAllWindows();

    // Rendering
    ImGui::Render();

    // Render ImGui draw data
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    glfwSwapBuffers(m_window);
}

void Application::SetupWindows()
{
    // Main menu window (always first for easy access)
    m_windowManager.AddWindow("Application", "Main", "Main Menu", 
        [this](bool* isOpen) { RenderMainMenu(isOpen); });

    // Setup demo window
    m_windowManager.AddWindow("Panels", "Windows", "Demo Window", 
        [this](bool* isOpen) { RenderDemoWindow(isOpen); });

    // Setup hello world window
    m_windowManager.AddWindow("Panels", "Windows", "Hello World", 
        [this](bool* isOpen) { RenderHelloWorldWindow(isOpen); });

    // Setup application info window
    m_windowManager.AddWindow("Panels", "Windows", "Application Info", 
        [this](bool* isOpen) { RenderApplicationInfoWindow(isOpen); });
}

void Application::RenderMainMenu(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Main Menu", isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Available Windows");
        ImGui::Separator();

        // Iterate through all windows and display toggle controls grouped by type
        const auto& allWindows = m_windowManager.GetAllWindows();

        // Collect unique types
        std::map<std::string, std::vector<std::shared_ptr<WindowFunction>>> windowsByType;
        for (const auto& window : allWindows)
        {
            // Skip the main menu itself
            if (window->GetWindowName() == "Main Menu")
                continue;

            windowsByType[window->GetType()].push_back(window);
        }

        // Render windows grouped by type
        for (const auto& [type, windows] : windowsByType)
        {
            if (ImGui::CollapsingHeader(type.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Indent();

                for (const auto& window : windows)
                {
                    std::string windowName = window->GetWindowName();
                    bool isEnabled = window->IsEnabled();

                    // Create a unique ID for this checkbox
                    std::string checkboxLabel = windowName + "##window_toggle";

                    if (ImGui::Checkbox(checkboxLabel.c_str(), &isEnabled))
                    {
                        window->SetEnabled(isEnabled);
                    }

                    // If window is already open, add a "Focus" button next to it
                    if (isEnabled)
                    {
                        ImGui::SameLine();
                        std::string focusLabel = "Focus##" + windowName;
                        if (ImGui::SmallButton(focusLabel.c_str()))
                        {
                            // Mark this window to be rendered topmost
                            m_windowManager.SetWindowTopmost(windowName);
                        }
                    }
                }

                ImGui::Unindent();
            }
        }

        ImGui::Separator();
        ImGui::Text("Total Windows: %zu", allWindows.size() - 1); // -1 for the main menu itself

        ImGui::End();
    }
}

void Application::RenderDemoWindow(bool* isOpen)
{
    ImGui::ShowDemoWindow(isOpen);
}

void Application::RenderHelloWorldWindow(bool* isOpen)
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Hello, world!", isOpen))
    {
        ImGui::Text("This is some useful text.");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Window Controls"))
        {
            WindowFunction* demoWindow = m_windowManager.GetWindow("Demo Window");
            if (demoWindow)
            {
                bool demoEnabled = demoWindow->IsEnabled();
                if (ImGui::Checkbox("Show Demo Window", &demoEnabled))
                {
                    demoWindow->SetEnabled(demoEnabled);
                }
            }
        }

        if (ImGui::CollapsingHeader("Display Settings"))
        {
            ImGui::ColorEdit4("Clear Color", (float*)&m_clearColor);
        }

        if (ImGui::CollapsingHeader("Interaction"))
        {
            ImGui::SliderFloat("Float Slider", &f, 0.0f, 1.0f);
            if (ImGui::Button("Button"))
            {
                counter++;
            }
            ImGui::SameLine();
            ImGui::Text("Counter = %d", counter);
        }

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Separator();
        ImGui::Text("Performance");
        ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

        ImGui::End();
    }
}

void Application::RenderApplicationInfoWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Application Info", isOpen))
    {
        ImGui::Text("Dear ImGui Application");
        ImGui::Separator();
        ImGui::Text("Windows Managed: %zu", m_windowManager.GetWindowCount());
        ImGui::Text("Framework: Dear ImGui");
        ImGui::Text("Rendering: OpenGL 3.2");
        ImGui::Text("Build: C++17 with CMake");
        ImGui::End();
    }
}

void Application::Shutdown()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}

void Application::LoadSettings()
{
    // Load settings from file
    if (!m_settings.Load())
    {
        fprintf(stdout, "Creating new settings file\n");
    }

    // Load color settings
    float r = m_settings.GetFloat("Display", "ColorR", 0.45f);
    float g = m_settings.GetFloat("Display", "ColorG", 0.55f);
    float b = m_settings.GetFloat("Display", "ColorB", 0.60f);
    float a = m_settings.GetFloat("Display", "ColorA", 1.00f);
    m_clearColor = ImVec4(r, g, b, a);

    // Load main menu visibility
    m_showMainMenu = m_settings.GetBool("Windows", "ShowMainMenu", true);

    // Load window visibility settings
    WindowFunction* mainMenu = m_windowManager.GetWindow("Main Menu");
    if (mainMenu)
    {
        mainMenu->SetEnabled(m_showMainMenu);
    }

    WindowFunction* demoWindow = m_windowManager.GetWindow("Demo Window");
    if (demoWindow)
    {
        demoWindow->SetEnabled(m_settings.GetBool("Windows", "ShowDemoWindow", true));
    }

    WindowFunction* helloWindow = m_windowManager.GetWindow("Hello World");
    if (helloWindow)
    {
        helloWindow->SetEnabled(m_settings.GetBool("Windows", "ShowHelloWorld", true));
    }

    WindowFunction* infoWindow = m_windowManager.GetWindow("Application Info");
    if (infoWindow)
    {
        infoWindow->SetEnabled(m_settings.GetBool("Windows", "ShowAppInfo", true));
    }
}

void Application::SaveSettings()
{
    // Save color settings
    m_settings.SetFloat("Display", "ColorR", m_clearColor.x);
    m_settings.SetFloat("Display", "ColorG", m_clearColor.y);
    m_settings.SetFloat("Display", "ColorB", m_clearColor.z);
    m_settings.SetFloat("Display", "ColorA", m_clearColor.w);

    // Save main menu visibility
    WindowFunction* mainMenu = m_windowManager.GetWindow("Main Menu");
    if (mainMenu)
    {
        m_settings.SetBool("Windows", "ShowMainMenu", mainMenu->IsEnabled());
    }

    // Save window visibility settings
    WindowFunction* demoWindow = m_windowManager.GetWindow("Demo Window");
    if (demoWindow)
    {
        m_settings.SetBool("Windows", "ShowDemoWindow", demoWindow->IsEnabled());
    }

    WindowFunction* helloWindow = m_windowManager.GetWindow("Hello World");
    if (helloWindow)
    {
        m_settings.SetBool("Windows", "ShowHelloWindow", helloWindow->IsEnabled());
    }

    WindowFunction* infoWindow = m_windowManager.GetWindow("Application Info");
    if (infoWindow)
    {
        m_settings.SetBool("Windows", "ShowAppInfo", infoWindow->IsEnabled());
    }

    // Save to file
    if (!m_settings.Save())
    {
        fprintf(stderr, "Failed to save settings\n");
    }
}
