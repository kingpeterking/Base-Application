#include "pch.h"
#include "Application.h"

Application::Application()
    : m_window(nullptr),
      m_clearColor(0.45f, 0.55f, 0.60f, 1.00f),
      m_showMainMenu(true),
      m_settings("settings.ini")
{
    memset(m_urlBuffer, 0, URL_BUFFER_SIZE);
    strcpy_s(m_urlBuffer, URL_BUFFER_SIZE, "https://api.github.com");
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

    // Setup ImPlot context
    ImPlot::CreateContext();

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
    // Create WindowFunctions instance
    m_windowFunctions = std::make_unique<WindowFunctions>(this);

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

    // Setup ImPlot demo window
    m_windowManager.AddWindow("Panels", "Plotting", "Plot Demo", 
        [this](bool* isOpen) { RenderImPlotDemoWindow(isOpen); });

    // Setup URL request window
    m_windowManager.AddWindow("Tools", "Network", "URL Request", 
        [this](bool* isOpen) { RenderURLRequestWindow(isOpen); });
}

// Window rendering delegations to WindowFunctions
void Application::RenderMainMenu(bool* isOpen)
{
    m_windowFunctions->RenderMainMenu(isOpen);
}

void Application::RenderDemoWindow(bool* isOpen)
{
    m_windowFunctions->RenderDemoWindow(isOpen);
}

void Application::RenderHelloWorldWindow(bool* isOpen)
{
    m_windowFunctions->RenderHelloWorldWindow(isOpen);
}

void Application::RenderApplicationInfoWindow(bool* isOpen)
{
    m_windowFunctions->RenderApplicationInfoWindow(isOpen);
}

void Application::RenderImPlotDemoWindow(bool* isOpen)
{
    m_windowFunctions->RenderImPlotDemoWindow(isOpen);
}

void Application::RenderURLRequestWindow(bool* isOpen)
{
    m_windowFunctions->RenderURLRequestWindow(isOpen);
}

void Application::Shutdown()
{
    // Cleanup ImPlot
    ImPlot::DestroyContext();

    // Cleanup ImGui
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

    WindowFunction* implotWindow = m_windowManager.GetWindow("Plot Demo");
    if (implotWindow)
    {
        implotWindow->SetEnabled(m_settings.GetBool("Windows", "ShowPlotDemo", false));
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

    WindowFunction* implotWindow = m_windowManager.GetWindow("Plot Demo");
    if (implotWindow)
    {
        m_settings.SetBool("Windows", "ShowPlotDemo", implotWindow->IsEnabled());
    }

    // Save to file
    if (!m_settings.Save())
    {
        fprintf(stderr, "Failed to save settings\n");
    }
}
