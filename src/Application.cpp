#include "pch.h"
#include "Application.h"

Application::Application()
    : m_window(nullptr),
      m_clearColor(0.45f, 0.55f, 0.60f, 1.00f),
      m_showDemoWindow(true),
      m_showAnotherWindow(false),
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

    SetupImGui();
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
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window
    if (m_showDemoWindow)
        ImGui::ShowDemoWindow(&m_showDemoWindow);

    // 2. Show a simple window that we create ourselves
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::Checkbox("Demo Window", &m_showDemoWindow);
        ImGui::Checkbox("Another Window", &m_showAnotherWindow);
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
        ImGui::ColorEdit4("clear color", (float*)&m_clearColor);
        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window
    if (m_showAnotherWindow)
    {
        ImGui::Begin("Another Window", &m_showAnotherWindow);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            m_showAnotherWindow = false;
        ImGui::End();
    }

    // Rendering
    ImGui::Render();

    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);

    // Use Windows API to clear the screen (only for Windows)
    #ifdef _WIN32
        // For Windows, we can use a simple approach - just swap the buffer
        // ImGui will render to it via OpenGL commands inside the backend
    #endif

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(m_window);
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

    // Load UI state from settings
    m_showDemoWindow = m_settings.GetBool("UI", "ShowDemoWindow", true);
    m_showAnotherWindow = m_settings.GetBool("UI", "ShowAnotherWindow", false);

    // Load color settings
    float r = m_settings.GetFloat("Display", "ColorR", 0.45f);
    float g = m_settings.GetFloat("Display", "ColorG", 0.55f);
    float b = m_settings.GetFloat("Display", "ColorB", 0.60f);
    float a = m_settings.GetFloat("Display", "ColorA", 1.00f);
    m_clearColor = ImVec4(r, g, b, a);
}

void Application::SaveSettings()
{
    // Save UI state to settings
    m_settings.SetBool("UI", "ShowDemoWindow", m_showDemoWindow);
    m_settings.SetBool("UI", "ShowAnotherWindow", m_showAnotherWindow);

    // Save color settings
    m_settings.SetFloat("Display", "ColorR", m_clearColor.x);
    m_settings.SetFloat("Display", "ColorG", m_clearColor.y);
    m_settings.SetFloat("Display", "ColorB", m_clearColor.z);
    m_settings.SetFloat("Display", "ColorA", m_clearColor.w);

    // Save to file
    if (!m_settings.Save())
    {
        fprintf(stderr, "Failed to save settings\n");
    }
}
