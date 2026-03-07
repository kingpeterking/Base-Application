#include "pch.h"
#include "Application.h"

Application::Application()
    : m_window(nullptr),
      m_clearColor(0.45f, 0.55f, 0.60f, 1.00f),
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
    // Demo Window
    m_windowManager.AddWindow("Windows", "Demo Window", [](bool* isOpen) {
        ImGui::ShowDemoWindow(isOpen);
    });

    // Main Control Window
    m_windowManager.AddWindow("Windows", "Hello World", [this](bool* isOpen) {
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
    });

    // Info Window
    m_windowManager.AddWindow("Windows", "Application Info", [this](bool* isOpen) {
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
    });
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

    // Load window visibility settings
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

    // Save window visibility settings
    WindowFunction* demoWindow = m_windowManager.GetWindow("Demo Window");
    if (demoWindow)
    {
        m_settings.SetBool("Windows", "ShowDemoWindow", demoWindow->IsEnabled());
    }

    WindowFunction* helloWindow = m_windowManager.GetWindow("Hello World");
    if (helloWindow)
    {
        m_settings.SetBool("Windows", "ShowHelloWorld", helloWindow->IsEnabled());
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
