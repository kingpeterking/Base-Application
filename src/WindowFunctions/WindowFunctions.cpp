#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "WindowFunctions/WindowManager.h"
#include "Tools/Web/HTTPClient.h"
#include "Tools/Logger.h"

WindowFunctions::WindowFunctions(Application* app)
    : m_app(app)
{
}

WindowFunctions::~WindowFunctions()
{
}

void WindowFunctions::RenderMainMenu(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Main Menu", isOpen, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // Build configuration indicator
        #ifdef _DEBUG
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Build: DEBUG");
        #else
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Build: RELEASE");
        #endif
        ImGui::Separator();

        ImGui::Text("Available Windows");
        ImGui::Separator();

        // Iterate through all windows and display toggle controls grouped by type
        const auto& allWindows = m_app->m_windowManager.GetAllWindows();

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
                }

                ImGui::Unindent();
            }
        }

        ImGui::Separator();
        ImGui::Text("Total Windows: %zu", allWindows.size() - 1); // -1 for the main menu itself
    }

    ImGui::End();
}

void WindowFunctions::RenderDemoWindow(bool* isOpen)
{
    ImGui::ShowDemoWindow(isOpen);
}

void WindowFunctions::RenderApplicationWindow(bool* isOpen)
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Application", isOpen))
    {
        // Application Info Section
        ImGui::Text("Dear ImGui Application");
        ImGui::Separator();

        // Build configuration
        #ifdef _DEBUG
            ImGui::Text("Configuration: DEBUG");
        #else
            ImGui::Text("Configuration: RELEASE");
        #endif

        ImGui::Text("Windows Managed: %zu", m_app->m_windowManager.GetWindowCount());
        ImGui::Text("Framework: Dear ImGui");
        ImGui::Text("Rendering: OpenGL 3.2");
        ImGui::Text("Build: C++17 with CMake");
        ImGui::Separator();

        // Interactive Controls Section
        ImGui::Text("This is some useful text.");
        ImGui::Separator();

        if (ImGui::CollapsingHeader("Window Controls"))
        {
            WindowFunction* demoWindow = m_app->m_windowManager.GetWindow("Demo Window");
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
            ImGui::ColorEdit4("Clear Color", (float*)&m_app->m_clearColor);
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
    }

    ImGui::End();
}

void WindowFunctions::RenderImPlotDemoWindow(bool* isOpen)
{
    m_app->m_implotClient.RenderDemoWindow(isOpen);
}

void WindowFunctions::RenderURLRequestWindow(bool* isOpen)
{
    static const char* httpMethods[] = { "GET", "POST", "PUT", "DELETE", "PATCH", "HEAD", "OPTIONS" };

    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("URL Request", isOpen))
    {
        ImGui::Text("HTTP Request Tool");
        ImGui::Separator();

        // HTTP Method selection
        ImGui::Text("Method:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::Combo("##http_method", &m_app->m_selectedHTTPMethod, httpMethods, IM_ARRAYSIZE(httpMethods));

        // URL input
        ImGui::Text("URL:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(400);
        ImGui::InputText("##url_input", m_app->m_urlBuffer, m_app->URL_BUFFER_SIZE);

        ImGui::Separator();

        // Collapsible section for URL Parameters
        if (ImGui::CollapsingHeader("URL Parameters", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Text("Add Parameter:");
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##param_key", m_app->m_paramKeyBuffer, m_app->PARAM_BUFFER_SIZE);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::InputText("##param_value", m_app->m_paramValueBuffer, m_app->PARAM_BUFFER_SIZE);
            ImGui::SameLine();
            if (ImGui::Button("Add Param", ImVec2(100, 0)))
            {
                std::string key = m_app->m_paramKeyBuffer;
                std::string value = m_app->m_paramValueBuffer;
                if (!key.empty() && !value.empty())
                {
                    m_app->m_requestParameters[key] = value;
                    memset(m_app->m_paramKeyBuffer, 0, m_app->PARAM_BUFFER_SIZE);
                    memset(m_app->m_paramValueBuffer, 0, m_app->PARAM_BUFFER_SIZE);
                }
            }

            // Display existing parameters
            if (!m_app->m_requestParameters.empty())
            {
                ImGui::Text("Current Parameters:");
                std::vector<std::string> keysToRemove;
                for (auto& [key, value] : m_app->m_requestParameters)
                {
                    ImGui::BulletText("%s = %s", key.c_str(), value.c_str());
                    ImGui::SameLine();
                    std::string removeButtonId = "Remove##" + key;
                    if (ImGui::SmallButton(removeButtonId.c_str()))
                    {
                        keysToRemove.push_back(key);
                    }
                }
                for (const auto& key : keysToRemove)
                {
                    m_app->m_requestParameters.erase(key);
                }
            }
        }

        // Collapsible section for Request Payload
        if (ImGui::CollapsingHeader("Request Payload", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextDisabled("(For POST, PUT, PATCH, DELETE requests)");
            ImGui::InputTextMultiline("##payload_input", m_app->m_payloadBuffer, m_app->PAYLOAD_BUFFER_SIZE,
                ImVec2(-1, 100), ImGuiInputTextFlags_AllowTabInput);
        }

        ImGui::Separator();

        // Send button
        if (ImGui::Button("Send Request", ImVec2(150, 0)) && !m_app->m_httpClient.IsLoading())
        {
            HTTPMethod method = static_cast<HTTPMethod>(m_app->m_selectedHTTPMethod);
            std::string payload = m_app->m_payloadBuffer;
            m_app->m_httpClient.PerformRequest(m_app->m_urlBuffer, method, payload, m_app->m_requestParameters);
        }

        ImGui::SameLine();
        if (ImGui::Button("Clear All", ImVec2(100, 0)))
        {
            m_app->m_httpClient.ClearAll();
            m_app->m_requestParameters.clear();
            memset(m_app->m_payloadBuffer, 0, m_app->PAYLOAD_BUFFER_SIZE);
        }

        ImGui::Separator();

        // Status and Response Code
        if (m_app->m_httpClient.IsLoading())
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "⏳ Loading...");
        }
        else
        {
            if (m_app->m_httpClient.WasSuccessful())
            {
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), 
                    "✓ Success | Status Code: %ld | Response: %zu bytes", 
                    m_app->m_httpClient.GetStatusCode(), m_app->m_httpClient.GetResponse().size());
            }
            else if (m_app->m_httpClient.GetStatusCode() > 0)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.0f, 1.0f), 
                    "⚠ HTTP %ld - %s", 
                    m_app->m_httpClient.GetStatusCode(), 
                    m_app->m_httpClient.GetError().c_str());
            }
            else if (!m_app->m_httpClient.GetError().empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), 
                    "✗ Error: %s", m_app->m_httpClient.GetError().c_str());
            }
        }

        ImGui::Separator();
        ImGui::Text("Response:");

        // Response display with scrollbar
        ImGui::BeginChild("response_scroll", ImVec2(0, -50), true, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::TextUnformatted(m_app->m_httpClient.GetResponse().c_str(), 
            m_app->m_httpClient.GetResponse().c_str() + m_app->m_httpClient.GetResponse().size());
        ImGui::EndChild();

        // Copy button
        if (!m_app->m_httpClient.GetResponse().empty())
        {
            if (ImGui::Button("Copy Response"))
            {
                ImGui::SetClipboardText(m_app->m_httpClient.GetResponse().c_str());
            }
        }
    }

    ImGui::End();
}

void WindowFunctions::RenderFileExplorerWindow(bool* isOpen)
{
    static char currentPath[512] = "";
    static char extensionFilter[256] = "";
    static char previousFilter[256] = "";
    static std::vector<FileInfo> fileList;
    static std::vector<std::string> directoryList;
    static std::vector<std::string> availableDrives;
    static std::vector<int> fileSelection;  // Use int (0/1) instead of bool to avoid std::vector<bool> issues
    static int currentDriveIndex = 0;
    static bool needsRefresh = true;

    // Favorites system
    static std::vector<std::string> favoriteDirectories;
    static int selectedFavoriteIndex = -1;
    static bool favoritesLoaded = false;
    static std::string deletedFavoriteMessage;
    static float deletedFavoriteMessageTime = 0.0f;

    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("File Explorer", isOpen))
    {
        // Load settings on first run
        if (needsRefresh && currentPath[0] == '\0')
        {
            std::string savedPath = m_app->m_settings.GetString("FileExplorer", "LastDirectory", 
                FileSystem::GetCurrentDirectory());
            std::string savedFilter = m_app->m_settings.GetString("FileExplorer", "ExtensionFilter", "");

            strcpy_s(currentPath, sizeof(currentPath), savedPath.c_str());
            strcpy_s(extensionFilter, sizeof(extensionFilter), savedFilter.c_str());

            // Load available drives
            availableDrives = FileSystem::ListAvailableDrives();
            needsRefresh = false;
        }

        // Load favorites on first run
        if (!favoritesLoaded)
        {
            std::string favoritesStr = m_app->m_settings.GetString("FileExplorer", "Favorites", "");
            favoriteDirectories.clear();

            if (!favoritesStr.empty())
            {
                // Parse semicolon-separated list
                size_t start = 0;
                size_t end = favoritesStr.find(';');
                while (end != std::string::npos)
                {
                    std::string favorite = favoritesStr.substr(start, end - start);
                    if (!favorite.empty() && FileSystem::DirectoryExists(favorite))
                    {
                        favoriteDirectories.push_back(favorite);
                    }
                    start = end + 1;
                    end = favoritesStr.find(';', start);
                }
                // Add last favorite
                std::string favorite = favoritesStr.substr(start);
                if (!favorite.empty() && FileSystem::DirectoryExists(favorite))
                {
                    favoriteDirectories.push_back(favorite);
                }
            }
            favoritesLoaded = true;
        }

        // Display deleted favorite message with fade-out
        if (deletedFavoriteMessageTime > 0.0f)
        {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, deletedFavoriteMessageTime), 
                "%s", deletedFavoriteMessage.c_str());
            deletedFavoriteMessageTime -= ImGui::GetIO().DeltaTime;
        }

        // Summary Statistics at TOP (moved from bottom)
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 1.0f, 1.0f), 
            "Files: %zu | Directories: %zu | Total Size: %.2f KB", 
            fileList.size(), directoryList.size(), 
            std::accumulate(fileList.begin(), fileList.end(), 0.0,
                [](double sum, const FileInfo& f) { return sum + f.sizeBytes; }) / 1024.0);

        ImGui::Separator();

        // Favorites section
        ImGui::Text("Favorites:");
        ImGui::SameLine();

        // Validate and clean up deleted favorites
        std::vector<std::string> validFavorites;
        for (const auto& fav : favoriteDirectories)
        {
            if (FileSystem::DirectoryExists(fav))
            {
                validFavorites.push_back(fav);
            }
            else
            {
                // Show error message for this deleted favorite
                deletedFavoriteMessage = "Favorite directory deleted: " + fav;
                deletedFavoriteMessageTime = 3.0f;
            }
        }
        favoriteDirectories = validFavorites;

        // Favorites dropdown
        if (!favoriteDirectories.empty())
        {
            if (ImGui::BeginCombo("##favorites_selector", 
                selectedFavoriteIndex >= 0 && selectedFavoriteIndex < (int)favoriteDirectories.size() 
                    ? favoriteDirectories[selectedFavoriteIndex].c_str() 
                    : "Select a favorite..."))
            {
                for (size_t i = 0; i < favoriteDirectories.size(); ++i)
                {
                    bool isSelected = (selectedFavoriteIndex == (int)i);
                    if (ImGui::Selectable(favoriteDirectories[i].c_str(), isSelected))
                    {
                        selectedFavoriteIndex = i;
                        strcpy_s(currentPath, sizeof(currentPath), favoriteDirectories[i].c_str());
                        needsRefresh = true;
                    }
                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::SameLine();
        }
        else
        {
            ImGui::Text("(No favorites yet)");
            ImGui::SameLine();
        }

        // Add to Favorites button
        if (ImGui::Button("★ Add", ImVec2(60, 0)))
        {
            // Add current directory to favorites if it's not already there
            bool alreadyFavorited = false;
            for (const auto& fav : favoriteDirectories)
            {
                if (fav == currentPath)
                {
                    alreadyFavorited = true;
                    break;
                }
            }

            if (!alreadyFavorited && FileSystem::DirectoryExists(currentPath))
            {
                favoriteDirectories.push_back(currentPath);
                selectedFavoriteIndex = favoriteDirectories.size() - 1;

                // Save favorites to INI
                std::string favoritesStr;
                for (size_t i = 0; i < favoriteDirectories.size(); ++i)
                {
                    if (i > 0) favoritesStr += ";";
                    favoritesStr += favoriteDirectories[i];
                }
                m_app->m_settings.SetString("FileExplorer", "Favorites", favoritesStr);
                m_app->m_settings.Save();
            }
        }

        // Remove from Favorites button
        ImGui::SameLine();
        if (ImGui::Button("✕ Remove", ImVec2(80, 0)))
        {
            if (selectedFavoriteIndex >= 0 && selectedFavoriteIndex < (int)favoriteDirectories.size())
            {
                favoriteDirectories.erase(favoriteDirectories.begin() + selectedFavoriteIndex);
                selectedFavoriteIndex = -1;

                // Save updated favorites to INI
                std::string favoritesStr;
                for (size_t i = 0; i < favoriteDirectories.size(); ++i)
                {
                    if (i > 0) favoritesStr += ";";
                    favoritesStr += favoriteDirectories[i];
                }
                m_app->m_settings.SetString("FileExplorer", "Favorites", favoritesStr);
                m_app->m_settings.Save();
            }
        }

        ImGui::Separator();
        if (!availableDrives.empty())
        {
            ImGui::Text("Drives/Volumes:");
            ImGui::SameLine();

            // Create a combo box for drive selection
            if (ImGui::BeginCombo("##drive_selector", availableDrives[currentDriveIndex].c_str()))
            {
                for (size_t i = 0; i < availableDrives.size(); ++i)
                {
                    bool isSelected = (currentDriveIndex == i);
                    if (ImGui::Selectable(availableDrives[i].c_str(), isSelected))
                    {
                        currentDriveIndex = i;

                        // Navigate to selected drive
                        #ifdef _WIN32
                            std::string newPath = availableDrives[i] + "\\";
                        #else
                            std::string newPath = availableDrives[i];
                            if (newPath.back() != '/') newPath += "/";
                        #endif

                        strcpy_s(currentPath, sizeof(currentPath), newPath.c_str());
                        needsRefresh = true;
                    }

                    if (isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            ImGui::SameLine();
            ImGui::Text("| ");
            ImGui::SameLine();
        }

        // Directory path input
        ImGui::Text("Current Directory:");
        ImGui::InputText("##directory_path", currentPath, sizeof(currentPath));
        ImGui::SameLine();
        if (ImGui::Button("Back##dir", ImVec2(60, 0)))
        {
            // Navigate to parent directory
            std::string parentPath = FileSystem::GetParentDirectory(currentPath);
            strcpy_s(currentPath, sizeof(currentPath), parentPath.c_str());
            needsRefresh = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Browse##dir", ImVec2(80, 0)))
        {
            // In a real app, you'd use a file browser dialog
            // For now, set to current directory
            std::string cwd = FileSystem::GetCurrentDirectory();
            strcpy_s(currentPath, sizeof(currentPath), cwd.c_str());
            needsRefresh = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Refresh", ImVec2(80, 0)))
        {
            needsRefresh = true;
        }

        ImGui::Separator();

        // Extension filter input
        ImGui::Text("Extension Filter:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(200);
        ImGui::InputText("##extension_filter", extensionFilter, sizeof(extensionFilter));
        ImGui::SameLine();
        ImGui::TextDisabled("(e.g., .txt or .cpp;.h)");

        // Check if filter has changed and trigger refresh
        if (strcmp(extensionFilter, previousFilter) != 0)
        {
            strcpy_s(previousFilter, sizeof(previousFilter), extensionFilter);
            needsRefresh = true;
        }

        // Save settings button
        ImGui::SameLine();
        if (ImGui::Button("Save Defaults", ImVec2(100, 0)))
        {
            m_app->m_settings.SetString("FileExplorer", "LastDirectory", currentPath);
            m_app->m_settings.SetString("FileExplorer", "ExtensionFilter", extensionFilter);
            m_app->m_settings.Save();
        }

        ImGui::Separator();

        // Refresh file list if needed
        if (needsRefresh && FileSystem::DirectoryExists(currentPath))
        {
            directoryList = FileSystem::ListDirectories(currentPath);
            fileList = FileSystem::ListFiles(currentPath, extensionFilter);
            fileSelection.resize(fileList.size(), false);  // Reset selection state for new file list
            needsRefresh = false;
        }

        // Ensure selection vector is the right size
        if (fileSelection.size() != fileList.size())
        {
            fileSelection.resize(fileList.size(), false);
        }

        // Select All / Deselect All buttons
        if (ImGui::Button("Select All", ImVec2(100, 0)))
        {
            for (auto& selected : fileSelection)
            {
                selected = 1;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Deselect All", ImVec2(120, 0)))
        {
            for (auto& selected : fileSelection)
            {
                selected = 0;
            }
        }

        ImGui::Separator();

        // File list with columns (added checkbox column)
        if (ImGui::BeginTable("files_table", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Select", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Size (bytes)", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Lines", ImGuiTableColumnFlags_WidthFixed, 60);
            ImGui::TableHeadersRow();

            // Add parent directory entry (..)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextDisabled("-");  // No selection for parent directory
                ImGui::TableSetColumnIndex(1);

                if (ImGui::Selectable("[DIR] ..", false, ImGuiSelectableFlags_SpanAllColumns))
                {
                    // Navigate to parent directory
                    std::string parentPath = FileSystem::GetParentDirectory(currentPath);
                    strcpy_s(currentPath, sizeof(currentPath), parentPath.c_str());
                    needsRefresh = true;
                }

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("Parent");
            }

            // List directories
            for (const auto& dir : directoryList)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextDisabled("-");  // No selection for directories
                ImGui::TableSetColumnIndex(1);

                if (ImGui::Selectable((std::string("[DIR] ") + dir).c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                {
                    // Navigate to directory
                    std::string newPath = FileSystem::JoinPath(currentPath, dir);
                    strcpy_s(currentPath, sizeof(currentPath), newPath.c_str());
                    needsRefresh = true;
                }

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("Directory");
            }

            // List files with checkboxes
            for (size_t i = 0; i < fileList.size(); ++i)
            {
                const auto& file = fileList[i];
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);

                // Checkbox for file selection
                std::string checkboxLabel = "##select_file_" + std::to_string(i);
                bool isSelected = (fileSelection[i] != 0);
                if (ImGui::Checkbox(checkboxLabel.c_str(), &isSelected))
                {
                    fileSelection[i] = isSelected ? 1 : 0;
                }

                ImGui::TableSetColumnIndex(1);
                ImGui::Selectable(file.filename.c_str(), false, ImGuiSelectableFlags_SpanAllColumns);

                ImGui::TableSetColumnIndex(2);
                ImGui::Text("%s", file.extension.c_str());

                ImGui::TableSetColumnIndex(3);
                ImGui::Text("%llu", file.sizeBytes);

                ImGui::TableSetColumnIndex(4);
                ImGui::Text("%zu", file.lineCount);
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void WindowFunctions::RenderLogViewerWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(1200, 700), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Log Viewer", isOpen))
    {
        // Static state for filters
        static bool showInfo = true;
        static bool showWarning = true;
        static bool showError = true;
        static bool showDebug = false;
        static bool autoScroll = true;
        static std::set<std::string> enabledSources;
        static std::set<std::string> knownSources; // Track sources we've seen before
        static bool firstRun = true;

        // Get all unique sources from logs
        std::set<std::string> allSources = Logger::GetInstance().GetUniqueSources();

        // Initialize enabled sources on first run (enable all)
        if (firstRun)
        {
            enabledSources = allSources;
            knownSources = allSources;
            firstRun = false;
        }

        // Add only genuinely NEW sources to enabled list automatically
        for (const auto& source : allSources)
        {
            // If this is a source we haven't seen before, enable it and mark as known
            if (knownSources.find(source) == knownSources.end())
            {
                enabledSources.insert(source);
                knownSources.insert(source);
            }
        }

        // Top bar: Level filters and controls
        ImGui::Text("Level Filters:");
        ImGui::SameLine();
        ImGui::Checkbox("Info", &showInfo);
        ImGui::SameLine();
        ImGui::Checkbox("Warning", &showWarning);
        ImGui::SameLine();
        ImGui::Checkbox("Error", &showError);
        ImGui::SameLine();
        ImGui::Checkbox("Debug", &showDebug);
        ImGui::SameLine(0, 30);
        ImGui::Checkbox("Auto-scroll", &autoScroll);

        ImGui::SameLine(0, 30);
        if (ImGui::Button("Clear Logs"))
        {
            Logger::GetInstance().Clear();
        }

        ImGui::Separator();

        // Log statistics
        int totalLogs = Logger::GetInstance().GetTotalCount();
        int infoCount = Logger::GetInstance().GetInfoCount();
        int warningCount = Logger::GetInstance().GetWarningCount();
        int errorCount = Logger::GetInstance().GetErrorCount();
        int debugCount = Logger::GetInstance().GetDebugCount();

        ImGui::Text("Total: %d | ", totalLogs);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Info: %d", infoCount);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Warning: %d", warningCount);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Error: %d", errorCount);
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Debug: %d", debugCount);

        ImGui::Separator();

        // Split window: Left panel (Source filters) and Right panel (Log table)
        if (ImGui::BeginTable("log_viewer_layout", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV))
        {
            // Setup columns: left panel (source filters) and right panel (logs)
            ImGui::TableSetupColumn("Sources", ImGuiTableColumnFlags_WidthFixed, 200);
            ImGui::TableSetupColumn("Logs", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            // LEFT PANEL: Source filters
            ImGui::TableSetColumnIndex(0);

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Source Filters");
            ImGui::Separator();

            // Select All / Deselect All buttons
            if (ImGui::Button("Enable All", ImVec2(-1, 0)))
            {
                enabledSources = allSources;
            }
            if (ImGui::Button("Disable All", ImVec2(-1, 0)))
            {
                enabledSources.clear();
            }

            ImGui::Separator();

            // Source checkboxes in a scrollable child window
            if (ImGui::BeginChild("source_list", ImVec2(0, 0), true))
            {
                if (allSources.empty())
                {
                    ImGui::TextDisabled("(No sources yet)");
                }
                else
                {
                    for (const auto& source : allSources)
                    {
                        bool enabled = enabledSources.find(source) != enabledSources.end();

                        if (ImGui::Checkbox(source.c_str(), &enabled))
                        {
                            if (enabled)
                            {
                                enabledSources.insert(source);
                            }
                            else
                            {
                                enabledSources.erase(source);
                            }
                        }
                    }
                }

                ImGui::EndChild();
            }

            // RIGHT PANEL: Log table
            ImGui::TableSetColumnIndex(1);

            // Log table
            if (ImGui::BeginTable("logs_table", 5, 
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
            {
                ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100);
                ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 150);
                ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Copy", ImGuiTableColumnFlags_WidthFixed, 50);
                ImGui::TableSetupScrollFreeze(0, 1); // Freeze header row
                ImGui::TableHeadersRow();

                // Get filtered logs (by level and source)
                auto logs = Logger::GetInstance().GetFilteredLogs(showInfo, showWarning, showError, showDebug, enabledSources);

                // Render each log entry
                for (const auto& entry : logs)
                {
                    ImGui::TableNextRow();

                    // Timestamp
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(entry.timestamp.c_str());

                    // Level with color
                    ImGui::TableSetColumnIndex(1);
                    ImVec4 levelColor;
                    const char* levelText;
                    switch (entry.level)
                    {
                        case LogLevel::Info:
                            levelColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
                            levelText = "INFO";
                            break;
                        case LogLevel::Warning:
                            levelColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
                            levelText = "WARNING";
                            break;
                        case LogLevel::Error:
                            levelColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
                            levelText = "ERROR";
                            break;
                        case LogLevel::Debug:
                            levelColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                            levelText = "DEBUG";
                            break;
                        default:
                            levelColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
                            levelText = "UNKNOWN";
                            break;
                    }
                    ImGui::TextColored(levelColor, "%s", levelText);

                    // Source
                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(entry.source.c_str());

                    // Message
                    ImGui::TableSetColumnIndex(3);
                    ImGui::TextWrapped("%s", entry.message.c_str());

                    // Copy button
                    ImGui::TableSetColumnIndex(4);
                    std::string copyButtonLabel = "📋##copy_" + std::to_string(std::hash<std::string>{}(entry.timestamp + entry.message));
                    if (ImGui::SmallButton(copyButtonLabel.c_str()))
                    {
                        std::string logEntry = "[" + entry.timestamp + "] [" + std::string(levelText) + "] [" + 
                                              entry.source + "] " + entry.message;
                        ImGui::SetClipboardText(logEntry.c_str());
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Copy this log entry");
                    }
                }

                // Auto-scroll to bottom
                if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                {
                    ImGui::SetScrollHereY(1.0f);
                }

                ImGui::EndTable();
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void WindowFunctions::RenderWebServerControlWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Web Server Control", isOpen))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "Web Server Configuration");
        ImGui::Separator();

        // Server status
        bool isRunning = m_app->m_webServer.IsRunning();
        if (isRunning)
        {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Status: RUNNING");
        }
        else
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Status: STOPPED");
        }

        ImGui::Separator();

        // Port configuration
        static int port = 8080;
        ImGui::Text("Server Port:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        if (isRunning)
        {
            ImGui::BeginDisabled();
            ImGui::InputInt("##port", &port);
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::TextDisabled("(Cannot change while running)");
        }
        else
        {
            ImGui::InputInt("##port", &port);
            if (port < 1 || port > 65535)
            {
                port = std::clamp(port, 1, 65535);
            }
        }

        ImGui::Separator();

        // Start/Stop buttons
        if (isRunning)
        {
            if (ImGui::Button("Stop Server", ImVec2(200, 40)))
            {
                m_app->m_webServer.Stop();
                LOG_INFO("Web server stopped by user");
            }
        }
        else
        {
            if (ImGui::Button("Start Server", ImVec2(200, 40)))
            {
                if (m_app->m_webServer.Start(port))
                {
                    LOG_INFO("Web server started on port " + std::to_string(port));
                }
                else
                {
                    LOG_ERROR("Failed to start web server on port " + std::to_string(port));
                }
            }
        }

        ImGui::Separator();

        // Server information
        if (isRunning)
        {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Server Information:");
            ImGui::Text("Listening on: http://0.0.0.0:%d", m_app->m_webServer.GetPort());
            ImGui::Text("Total requests: %zu", m_app->m_webServer.GetTotalRequestsReceived());
            ImGui::Text("Queued requests: %zu", m_app->m_webServer.GetRequestCount());

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Available Endpoints:");
            ImGui::BulletText("GET  /");
            ImGui::BulletText("GET  /api/status");
            ImGui::BulletText("POST /webhook");

            ImGui::Separator();

            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Test from command line:");
            ImGui::TextWrapped("curl http://localhost:%d/", m_app->m_webServer.GetPort());
            ImGui::TextWrapped("curl http://localhost:%d/api/status", m_app->m_webServer.GetPort());
            ImGui::TextWrapped("curl -X POST -d \"test\" http://localhost:%d/webhook", m_app->m_webServer.GetPort());
        }
        else
        {
            ImGui::TextDisabled("Server is not running. Click 'Start Server' to begin.");
        }
    }
    ImGui::End();  // MUST always call End() after Begin(), regardless of return value
}

void WindowFunctions::RenderWebServerRequestsWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Web Server - Request Monitor", isOpen))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "HTTP Request Monitor");
        ImGui::Separator();

        // Statistics bar
        size_t totalRequests = m_app->m_webServer.GetTotalRequestsReceived();
        size_t queuedRequests = m_app->m_webServer.GetRequestCount();

        ImGui::Text("Total Requests: %zu", totalRequests);
        ImGui::SameLine(0, 30);
        ImGui::Text("Queued: %zu", queuedRequests);
        ImGui::SameLine(0, 30);

        if (ImGui::Button("Clear History"))
        {
            m_app->m_webServer.ClearRequests();
        }

        ImGui::Separator();

        // Request table
        if (ImGui::BeginTable("requests_table", 6, 
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
            ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50);
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Method", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Path", ImGuiTableColumnFlags_WidthFixed, 200);
            ImGui::TableSetupColumn("Client IP", ImGuiTableColumnFlags_WidthFixed, 150);
            ImGui::TableSetupColumn("Body", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupScrollFreeze(0, 1); // Freeze header row
            ImGui::TableHeadersRow();

            // Get recent requests
            auto requests = m_app->m_webServer.GetRecentRequests(100);

            // Render each request (most recent first)
            for (auto it = requests.rbegin(); it != requests.rend(); ++it)
            {
                const auto& req = *it;

                ImGui::TableNextRow();

                // ID
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%d", req.id);

                // Timestamp
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(req.timestamp.c_str());

                // Method with color
                ImGui::TableSetColumnIndex(2);
                ImVec4 methodColor;
                if (req.method == "GET")
                    methodColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);  // Blue
                else if (req.method == "POST")
                    methodColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
                else if (req.method == "PUT")
                    methodColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
                else if (req.method == "DELETE")
                    methodColor = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);  // Red
                else
                    methodColor = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);  // Gray

                ImGui::TextColored(methodColor, "%s", req.method.c_str());

                // Path
                ImGui::TableSetColumnIndex(3);
                ImGui::TextUnformatted(req.path.c_str());

                // Client IP
                ImGui::TableSetColumnIndex(4);
                ImGui::TextUnformatted(req.remoteAddr.c_str());

                // Body (truncated if too long)
                ImGui::TableSetColumnIndex(5);
                if (!req.body.empty())
                {
                    std::string displayBody = req.body;
                    if (displayBody.length() > 100)
                    {
                        displayBody = displayBody.substr(0, 100) + "...";
                    }
                    ImGui::TextWrapped("%s", displayBody.c_str());
                }
                else
                {
                    ImGui::TextDisabled("(empty)");
                }
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

void WindowFunctions::RenderDatabaseConnectionWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(650, 550), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Connection", isOpen))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginTabBar("ConnectionModeTabs"))
    {
        if (ImGui::BeginTabItem("Connection Form"))
        {
            m_app->m_dbSelectedConnectionMode = 0;

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Database Connection Settings");
            ImGui::Separator();

            ImGui::Text("Driver:");
            ImGui::SetNextItemWidth(400);
            if (ImGui::BeginCombo("##driver", m_app->m_dbDriverBuffer))
            {
                if (m_app->m_availableDrivers.empty())
                {
                    m_app->m_availableDrivers = m_app->m_databaseManager.GetAvailableDrivers();
                }

                const char* commonDrivers[] = { "SQL Server", "PostgreSQL Unicode", "MySQL ODBC 8.0 Driver", 
                                                "SQLite3 ODBC Driver", "Oracle in OraDB19Home1",
                                                "Microsoft Access Driver (*.mdb, *.accdb)" };
                for (const char* driver : commonDrivers)
                {
                    bool isSelected = (strcmp(m_app->m_dbDriverBuffer, driver) == 0);
                    if (ImGui::Selectable(driver, isSelected))
                    {
                        strcpy_s(m_app->m_dbDriverBuffer, Application::DB_BUFFER_SIZE, driver);
                    }
                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }

                if (!m_app->m_availableDrivers.empty())
                {
                    ImGui::Separator();
                    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Installed Drivers:");
                    for (const auto& driver : m_app->m_availableDrivers)
                    {
                        bool isSelected = (driver == m_app->m_dbDriverBuffer);
                        if (ImGui::Selectable(driver.c_str(), isSelected))
                        {
                            strcpy_s(m_app->m_dbDriverBuffer, Application::DB_BUFFER_SIZE, driver.c_str());
                        }
                        if (isSelected)
                            ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            // Connection Name field
            ImGui::Text("Connection Name:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(250);
            ImGui::InputText("##conn_name", m_app->m_dbConnectionNameBuffer, Application::DB_BUFFER_SIZE);
            ImGui::SameLine();
            if (ImGui::SmallButton("Auto-Generate"))
            {
                int connNum = m_app->GenerateUniqueConnectionNumber();
                sprintf_s(m_app->m_dbConnectionNameBuffer, Application::DB_BUFFER_SIZE, "Connection %d", connNum);
            }
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Generate a unique connection name");
            }
            ImGui::SameLine();
            ImGui::TextDisabled("(Optional - auto-generated if empty)");

            ImGui::Text("Server:");
            ImGui::SetNextItemWidth(300);
            ImGui::InputText("##server", m_app->m_dbServerBuffer, Application::DB_BUFFER_SIZE);
            ImGui::SameLine();
            ImGui::Text("Port:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80);
            ImGui::InputText("##port", m_app->m_dbPortBuffer, sizeof(m_app->m_dbPortBuffer));

            // Show hint for MS Access
            if (strstr(m_app->m_dbDriverBuffer, "Access") != nullptr)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "💡 Tip:");
                ImGui::TextWrapped("For MS Access, enter the full file path in 'Server' field (e.g., C:\\MyDatabase.accdb)");
                ImGui::TextWrapped("Leave 'Port' and 'Database' fields empty for file-based Access databases.");
            }

            ImGui::Text("Database:");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##database", m_app->m_dbDatabaseBuffer, Application::DB_BUFFER_SIZE);

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Authentication:");

            ImGui::Checkbox("Trusted Connection (Windows Auth)", &m_app->m_dbTrustedConnection);

            if (!m_app->m_dbTrustedConnection)
            {
                ImGui::Text("Username:");
                ImGui::SetNextItemWidth(400);
                ImGui::InputText("##username", m_app->m_dbUsernameBuffer, Application::DB_BUFFER_SIZE);

                ImGui::Text("Password:");
                ImGui::SetNextItemWidth(400);
                ImGui::InputText("##password", m_app->m_dbPasswordBuffer, Application::DB_BUFFER_SIZE, 
                               ImGuiInputTextFlags_Password);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Options:");

            ImGui::Checkbox("Encrypt Connection", &m_app->m_dbEncrypt);

            ImGui::Text("Connection Timeout (seconds):");
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##conn_timeout", &m_app->m_dbConnectionTimeout, 5, 300);

            ImGui::Text("Command Timeout (seconds):");
            ImGui::SetNextItemWidth(150);
            ImGui::SliderInt("##cmd_timeout", &m_app->m_dbCommandTimeout, 5, 300);

            ImGui::Spacing();
            ImGui::Separator();

            bool isConnected = m_app->m_databaseManager.IsConnected();

            if (!isConnected)
            {
                if (ImGui::Button("Connect", ImVec2(120, 0)))
                {
                    Database::ConnectionConfig config;

                    // Generate connection name if empty
                    std::string connectionName = m_app->m_dbConnectionNameBuffer;
                    if (connectionName.empty())
                    {
                        int connNum = m_app->GenerateUniqueConnectionNumber();
                        connectionName = "Connection " + std::to_string(connNum);
                        strcpy_s(m_app->m_dbConnectionNameBuffer, Application::DB_BUFFER_SIZE, connectionName.c_str());
                    }
                    config.ConnectionName = connectionName;

                    config.DriverName = m_app->m_dbDriverBuffer;
                    config.ServerAddress = m_app->m_dbServerBuffer;

                    try {
                        int port = std::stoi(m_app->m_dbPortBuffer);
                        config.ServerPort = port;
                    }
                    catch (...) {
                        config.ServerPort = 0;
                    }

                    config.DatabaseName = m_app->m_dbDatabaseBuffer;
                    config.Username = m_app->m_dbUsernameBuffer;
                    config.Password = m_app->m_dbPasswordBuffer;
                    config.TrustedConnection = m_app->m_dbTrustedConnection;
                    config.Encrypt = m_app->m_dbEncrypt;
                    config.ConnectionTimeout = m_app->m_dbConnectionTimeout;
                    config.CommandTimeout = m_app->m_dbCommandTimeout;

                    // Create new connection manager
                    auto newConnection = std::make_shared<Database::DatabaseManager>();
                    if (newConnection->Connect(config))
                    {
                        // Add to connections list
                        m_app->m_databaseConnections.push_back(newConnection);
                        m_app->SetActiveConnection(static_cast<int>(m_app->m_databaseConnections.size()) - 1);

                        // Add to history
                        m_app->AddConnectionToHistory(config);

                        m_app->m_dbConnectionStatus = "Connected: " + connectionName + " - " + 
                            newConnection->GetDatabaseProduct() + " " +
                            newConnection->GetDatabaseVersion();
                        LOG_INFO_SRC(m_app->m_dbConnectionStatus, "Database");

                        // Also connect the legacy single connection for backwards compatibility
                        m_app->m_databaseManager.Connect(config);
                    }
                    else
                    {
                        m_app->m_dbConnectionStatus = "Connection failed: " + 
                            newConnection->GetLastError();
                        LOG_ERROR_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Test Connection", ImVec2(120, 0)))
                {
                    Database::ConnectionConfig config;
                    config.DriverName = m_app->m_dbDriverBuffer;
                    config.ServerAddress = m_app->m_dbServerBuffer;

                    try {
                        int port = std::stoi(m_app->m_dbPortBuffer);
                        config.ServerPort = port;
                    }
                    catch (...) {
                        config.ServerPort = 0;
                    }

                    config.DatabaseName = m_app->m_dbDatabaseBuffer;
                    config.Username = m_app->m_dbUsernameBuffer;
                    config.Password = m_app->m_dbPasswordBuffer;
                    config.TrustedConnection = m_app->m_dbTrustedConnection;
                    config.Encrypt = m_app->m_dbEncrypt;
                    config.ConnectionTimeout = m_app->m_dbConnectionTimeout;
                    config.CommandTimeout = m_app->m_dbCommandTimeout;

                    std::string errorMessage;
                    if (m_app->m_databaseManager.TestConnection(config, errorMessage))
                    {
                        m_app->m_dbConnectionStatus = "Test successful - connection OK";
                        LOG_INFO_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                    else
                    {
                        m_app->m_dbConnectionStatus = "Test failed: " + errorMessage;
                        LOG_ERROR_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                }
            }
            else
            {
                if (ImGui::Button("Disconnect", ImVec2(120, 0)))
                {
                    m_app->m_databaseManager.Disconnect();
                    m_app->m_dbConnectionStatus = "Disconnected";
                    LOG_INFO_SRC("Database disconnected", "Database");
                }

                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Connection Information:");
                ImGui::BulletText("Server: %s", m_app->m_databaseManager.GetServerName().c_str());
                ImGui::BulletText("Product: %s", m_app->m_databaseManager.GetDatabaseProduct().c_str());
                ImGui::BulletText("Version: %s", m_app->m_databaseManager.GetDatabaseVersion().c_str());
                ImGui::BulletText("Driver: %s %s", 
                                m_app->m_databaseManager.GetDriverName().c_str(),
                                m_app->m_databaseManager.GetDriverVersion().c_str());

                auto db = m_app->m_databaseManager.GetDatabase();
                if (db)
                {
                    ImGui::BulletText("Current Database: %s", db->GetName().c_str());
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextWrapped("Status: %s", m_app->m_dbConnectionStatus.c_str());

            // Show generated connection string for debugging
            ImGui::Spacing();
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Generated Connection String (Debug)", ImGuiTreeNodeFlags_DefaultOpen))
            {
                Database::ConnectionConfig previewConfig;
                previewConfig.DriverName = m_app->m_dbDriverBuffer;
                previewConfig.ServerAddress = m_app->m_dbServerBuffer;

                try {
                    int port = std::stoi(m_app->m_dbPortBuffer);
                    previewConfig.ServerPort = port;
                }
                catch (...) {
                    previewConfig.ServerPort = 0;
                }

                previewConfig.DatabaseName = m_app->m_dbDatabaseBuffer;
                previewConfig.Username = m_app->m_dbUsernameBuffer;
                previewConfig.Password = "********";  // Don't show actual password
                previewConfig.TrustedConnection = m_app->m_dbTrustedConnection;
                previewConfig.Encrypt = m_app->m_dbEncrypt;
                previewConfig.ConnectionTimeout = m_app->m_dbConnectionTimeout;
                previewConfig.CommandTimeout = m_app->m_dbCommandTimeout;

                std::string previewConnectionString = previewConfig.BuildConnectionString();

                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "This is the connection string that will be used:");
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                ImGui::InputTextMultiline("##preview_connstring", 
                    const_cast<char*>(previewConnectionString.c_str()), 
                    previewConnectionString.length() + 1,
                    ImVec2(-1, 80),
                    ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();

                if (ImGui::Button("Copy Connection String"))
                {
                    ImGui::SetClipboardText(previewConnectionString.c_str());
                }
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Connection String"))
        {
            m_app->m_dbSelectedConnectionMode = 1;

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "ODBC Connection String");
            ImGui::Separator();

            ImGui::Text("Enter ODBC connection string:");
            ImGui::SetNextItemWidth(-1);
            ImGui::InputTextMultiline("##connstring", m_app->m_dbConnectionStringBuffer, 
                                     sizeof(m_app->m_dbConnectionStringBuffer),
                                     ImVec2(-1, 150));

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Connection String Examples"))
            {
                ImGui::TextWrapped("SQL Server (Windows Auth):");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={SQL Server};Server=localhost;Database=master;Trusted_Connection=Yes;");

                ImGui::Spacing();
                ImGui::TextWrapped("SQL Server (SQL Auth):");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={SQL Server};Server=localhost;Database=mydb;Uid=sa;Pwd=password;");

                ImGui::Spacing();
                ImGui::TextWrapped("Azure SQL Database:");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={ODBC Driver 17 for SQL Server};Server=myserver.database.windows.net;Database=mydb;Uid=username;Pwd=password;Encrypt=Yes;");

                ImGui::Spacing();
                ImGui::TextWrapped("PostgreSQL:");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={PostgreSQL Unicode};Server=localhost;Port=5432;Database=mydb;Uid=postgres;Pwd=password;");

                ImGui::Spacing();
                ImGui::TextWrapped("MySQL:");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={MySQL ODBC 8.0 Driver};Server=localhost;Database=mydb;User=root;Password=password;");

                ImGui::Spacing();
                ImGui::TextWrapped("MS Access (Local File):");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\\MyDatabase.accdb;");

                ImGui::Spacing();
                ImGui::TextWrapped("MS Access (With Password):");
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                    "Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\\MyDatabase.accdb;PWD=mypassword;");
            }

            ImGui::Spacing();
            ImGui::Separator();

            bool isConnected = m_app->m_databaseManager.IsConnected();

            if (!isConnected)
            {
                if (ImGui::Button("Connect", ImVec2(120, 0)))
                {
                    // Generate connection name
                    int connNum = m_app->GenerateUniqueConnectionNumber();
                    std::string connectionName = "Connection " + std::to_string(connNum);

                    // Create new connection manager
                    auto newConnection = std::make_shared<Database::DatabaseManager>();
                    if (newConnection->ConnectWithConnectionString(m_app->m_dbConnectionStringBuffer))
                    {
                        // Add to connections list
                        m_app->m_databaseConnections.push_back(newConnection);
                        m_app->SetActiveConnection(static_cast<int>(m_app->m_databaseConnections.size()) - 1);

                        // Try to extract config from connection for history
                        Database::ConnectionConfig config;
                        config.ConnectionName = connectionName;
                        // Note: Connection string parsing for history would be complex, skip for now

                        m_app->m_dbConnectionStatus = "Connected: " + connectionName + " - " + 
                            newConnection->GetDatabaseProduct() + " " +
                            newConnection->GetDatabaseVersion();
                        LOG_INFO_SRC(m_app->m_dbConnectionStatus, "Database");

                        // Also connect the legacy single connection for backwards compatibility
                        m_app->m_databaseManager.ConnectWithConnectionString(m_app->m_dbConnectionStringBuffer);
                    }
                    else
                    {
                        m_app->m_dbConnectionStatus = "Connection failed: " + 
                            newConnection->GetLastError();
                        LOG_ERROR_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Test Connection", ImVec2(120, 0)))
                {
                    std::string errorMessage;
                    if (m_app->m_databaseManager.TestConnectionString(m_app->m_dbConnectionStringBuffer, errorMessage))
                    {
                        m_app->m_dbConnectionStatus = "Test successful - connection OK";
                        LOG_INFO_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                    else
                    {
                        m_app->m_dbConnectionStatus = "Test failed: " + errorMessage;
                        LOG_ERROR_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                }
            }
            else
            {
                if (ImGui::Button("Disconnect", ImVec2(120, 0)))
                {
                    m_app->m_databaseManager.Disconnect();
                    m_app->m_dbConnectionStatus = "Disconnected";
                    LOG_INFO_SRC("Database disconnected", "Database");
                }

                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextWrapped("Status: %s", m_app->m_dbConnectionStatus.c_str());

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Quick Connect"))
        {
            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Quick Connection Presets");
            ImGui::Separator();

            ImGui::TextWrapped("Select a preset to quickly connect to common database configurations:");
            ImGui::Spacing();

            if (ImGui::Button("Local SQL Server (Windows Auth)", ImVec2(300, 0)))
            {
                strcpy_s(m_app->m_dbDriverBuffer, Application::DB_BUFFER_SIZE, "SQL Server");
                strcpy_s(m_app->m_dbServerBuffer, Application::DB_BUFFER_SIZE, "localhost");
                strcpy_s(m_app->m_dbPortBuffer, sizeof(m_app->m_dbPortBuffer), "1433");
                strcpy_s(m_app->m_dbDatabaseBuffer, Application::DB_BUFFER_SIZE, "master");
                m_app->m_dbTrustedConnection = true;
                m_app->m_dbEncrypt = false;
                m_app->m_dbSelectedConnectionMode = 0;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("SQL Server with Windows authentication");

            if (ImGui::Button("Local PostgreSQL", ImVec2(300, 0)))
            {
                strcpy_s(m_app->m_dbDriverBuffer, Application::DB_BUFFER_SIZE, "PostgreSQL Unicode");
                strcpy_s(m_app->m_dbServerBuffer, Application::DB_BUFFER_SIZE, "localhost");
                strcpy_s(m_app->m_dbPortBuffer, sizeof(m_app->m_dbPortBuffer), "5432");
                strcpy_s(m_app->m_dbDatabaseBuffer, Application::DB_BUFFER_SIZE, "postgres");
                strcpy_s(m_app->m_dbUsernameBuffer, Application::DB_BUFFER_SIZE, "postgres");
                m_app->m_dbTrustedConnection = false;
                m_app->m_dbEncrypt = false;
                m_app->m_dbSelectedConnectionMode = 0;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("PostgreSQL default configuration");

            if (ImGui::Button("Local MySQL", ImVec2(300, 0)))
            {
                strcpy_s(m_app->m_dbDriverBuffer, Application::DB_BUFFER_SIZE, "MySQL ODBC 8.0 Driver");
                strcpy_s(m_app->m_dbServerBuffer, Application::DB_BUFFER_SIZE, "localhost");
                strcpy_s(m_app->m_dbPortBuffer, sizeof(m_app->m_dbPortBuffer), "3306");
                strcpy_s(m_app->m_dbDatabaseBuffer, Application::DB_BUFFER_SIZE, "mysql");
                strcpy_s(m_app->m_dbUsernameBuffer, Application::DB_BUFFER_SIZE, "root");
                m_app->m_dbTrustedConnection = false;
                m_app->m_dbEncrypt = false;
                m_app->m_dbSelectedConnectionMode = 0;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("MySQL default configuration");

            if (ImGui::Button("Azure SQL Database", ImVec2(300, 0)))
            {
                strcpy_s(m_app->m_dbDriverBuffer, Application::DB_BUFFER_SIZE, "ODBC Driver 17 for SQL Server");
                strcpy_s(m_app->m_dbServerBuffer, Application::DB_BUFFER_SIZE, "myserver.database.windows.net");
                strcpy_s(m_app->m_dbPortBuffer, sizeof(m_app->m_dbPortBuffer), "1433");
                strcpy_s(m_app->m_dbDatabaseBuffer, Application::DB_BUFFER_SIZE, "mydb");
                m_app->m_dbTrustedConnection = false;
                m_app->m_dbEncrypt = true;
                m_app->m_dbSelectedConnectionMode = 0;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("Azure SQL with encryption");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                "After selecting a preset, switch to 'Connection Form' tab to customize and connect.");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("DSN"))
        {
            m_app->m_dbSelectedConnectionMode = 2;

            ImGui::Spacing();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Data Source Name (DSN) Connection");
            ImGui::Separator();

            // Show application architecture
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Application Architecture:");
            #if defined(_WIN64)
                ImGui::SameLine();
                ImGui::Text("64-bit");
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "💡 Use 64-bit ODBC Administrator: odbcad32.exe");
            #elif defined(_WIN32)
                ImGui::SameLine();
                ImGui::Text("32-bit");
                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "💡 Use 32-bit ODBC Administrator: C:\\Windows\\SysWOW64\\odbcad32.exe");
            #else
                ImGui::SameLine();
                ImGui::Text("Unknown");
            #endif
            ImGui::Separator();

            ImGui::TextWrapped("Connect using a DSN configured in Windows ODBC Data Source Administrator.");
            ImGui::Spacing();

            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "How to create a DSN:");
            ImGui::BulletText("Open 'ODBC Data Sources' from Windows Start menu");
            ImGui::BulletText("Choose 'User DSN' or 'System DSN' tab");
            ImGui::BulletText("Click 'Add' and select your driver");
            ImGui::BulletText("Configure your connection and give it a name");
            ImGui::BulletText("Click 'OK' to save");

            ImGui::Spacing();
            ImGui::Separator();

            ImGui::Text("DSN Name:");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##dsn_name", m_app->m_dbDSNBuffer, Application::DB_BUFFER_SIZE);
            ImGui::SameLine();
            ImGui::TextDisabled("(Name you gave to the DSN)");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Optional Credentials:");
            ImGui::TextDisabled("(Only needed if DSN doesn't store credentials)");

            ImGui::Text("Username:");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##dsn_username", m_app->m_dbDSNUsernameBuffer, Application::DB_BUFFER_SIZE);

            ImGui::Text("Password:");
            ImGui::SetNextItemWidth(400);
            ImGui::InputText("##dsn_password", m_app->m_dbDSNPasswordBuffer, Application::DB_BUFFER_SIZE,
                           ImGuiInputTextFlags_Password);

            ImGui::Spacing();
            ImGui::Separator();

            bool isConnected = m_app->m_databaseManager.IsConnected();

            if (!isConnected)
            {
                if (ImGui::Button("Connect", ImVec2(120, 0)))
                {
                    // Build DSN connection string
                    std::string dsnConnectionString = "DSN=" + std::string(m_app->m_dbDSNBuffer) + ";";

                    if (strlen(m_app->m_dbDSNUsernameBuffer) > 0)
                    {
                        dsnConnectionString += "Uid=" + std::string(m_app->m_dbDSNUsernameBuffer) + ";";
                    }
                    if (strlen(m_app->m_dbDSNPasswordBuffer) > 0)
                    {
                        dsnConnectionString += "Pwd=" + std::string(m_app->m_dbDSNPasswordBuffer) + ";";
                    }

                    LOG_INFO_SRC("Attempting DSN connection: " + dsnConnectionString, "Database");

                    // Generate connection name
                    int connNum = m_app->GenerateUniqueConnectionNumber();
                    std::string connectionName = "DSN: " + std::string(m_app->m_dbDSNBuffer) + " (" + std::to_string(connNum) + ")";

                    // Create new connection manager
                    auto newConnection = std::make_shared<Database::DatabaseManager>();
                    if (newConnection->ConnectWithConnectionString(dsnConnectionString))
                    {
                        // Add to connections list
                        m_app->m_databaseConnections.push_back(newConnection);
                        m_app->SetActiveConnection(static_cast<int>(m_app->m_databaseConnections.size()) - 1);

                        m_app->m_dbConnectionStatus = "Connected: " + connectionName + " - " + 
                            newConnection->GetDatabaseProduct() + " " +
                            newConnection->GetDatabaseVersion();
                        LOG_INFO_SRC(m_app->m_dbConnectionStatus, "Database");

                        // Also connect the legacy single connection for backwards compatibility
                        m_app->m_databaseManager.ConnectWithConnectionString(dsnConnectionString);
                    }
                    else
                    {
                        m_app->m_dbConnectionStatus = "DSN connection failed: " + 
                            newConnection->GetLastError();
                        LOG_ERROR_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Test Connection", ImVec2(120, 0)))
                {
                    // Build DSN connection string
                    std::string dsnConnectionString = "DSN=" + std::string(m_app->m_dbDSNBuffer) + ";";

                    if (strlen(m_app->m_dbDSNUsernameBuffer) > 0)
                    {
                        dsnConnectionString += "Uid=" + std::string(m_app->m_dbDSNUsernameBuffer) + ";";
                    }
                    if (strlen(m_app->m_dbDSNPasswordBuffer) > 0)
                    {
                        dsnConnectionString += "Pwd=" + std::string(m_app->m_dbDSNPasswordBuffer) + ";";
                    }

                    std::string errorMessage;
                    if (m_app->m_databaseManager.TestConnectionString(dsnConnectionString, errorMessage))
                    {
                        m_app->m_dbConnectionStatus = "DSN test successful - connection OK";
                        LOG_INFO_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                    else
                    {
                        m_app->m_dbConnectionStatus = "DSN test failed: " + errorMessage;
                        LOG_ERROR_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                }
            }
            else
            {
                if (ImGui::Button("Disconnect", ImVec2(120, 0)))
                {
                    m_app->m_databaseManager.Disconnect();
                    m_app->m_dbConnectionStatus = "Disconnected";
                    LOG_INFO_SRC("Database disconnected", "Database");
                }

                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connected");
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::TextWrapped("Status: %s", m_app->m_dbConnectionStatus.c_str());

            // Show generated DSN connection string
            ImGui::Spacing();
            ImGui::Separator();
            if (ImGui::CollapsingHeader("Generated DSN Connection String (Debug)", ImGuiTreeNodeFlags_DefaultOpen))
            {
                std::string dsnPreviewString = "DSN=" + std::string(m_app->m_dbDSNBuffer) + ";";
                if (strlen(m_app->m_dbDSNUsernameBuffer) > 0)
                {
                    dsnPreviewString += "Uid=" + std::string(m_app->m_dbDSNUsernameBuffer) + ";";
                }
                if (strlen(m_app->m_dbDSNPasswordBuffer) > 0)
                {
                    dsnPreviewString += "Pwd=********;";
                }

                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "This is the DSN connection string that will be used:");
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                ImGui::InputTextMultiline("##preview_dsn_connstring", 
                    const_cast<char*>(dsnPreviewString.c_str()), 
                    dsnPreviewString.length() + 1,
                    ImVec2(-1, 60),
                    ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();

                if (ImGui::Button("Copy DSN Connection String"))
                {
                    ImGui::SetClipboardText(dsnPreviewString.c_str());
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    bool isConnected = m_app->m_databaseManager.IsConnected();
    if (isConnected)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Connection Information:");

        if (ImGui::BeginTable("connection_info", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
        {
            ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed, 150);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Server");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(m_app->m_databaseManager.GetServerName().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Database Product");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(m_app->m_databaseManager.GetDatabaseProduct().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Product Version");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(m_app->m_databaseManager.GetDatabaseVersion().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Driver Name");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(m_app->m_databaseManager.GetDriverName().c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("Driver Version");
            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(m_app->m_databaseManager.GetDriverVersion().c_str());

            auto db = m_app->m_databaseManager.GetDatabase();
            if (db)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("Current Database");
                ImGui::TableSetColumnIndex(1);
                ImGui::TextUnformatted(db->GetName().c_str());
            }

            ImGui::EndTable();
        }
    }

    ImGui::End();
}

// Render Database Connections Manager Window
void WindowFunctions::RenderDatabaseConnectionsManagerWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Connections Manager", isOpen))
    {
        ImGui::End();
        return;
    }

    // Create tabs
    if (ImGui::BeginTabBar("ConnectionsTabBar"))
    {
        // Tab 1: Active Connections
        if (ImGui::BeginTabItem("Active Connections"))
        {
            ImGui::TextWrapped("Manage all currently active database connections. You can have multiple connections open simultaneously.");
            ImGui::Separator();

            if (m_app->m_databaseConnections.empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No active connections.");
                ImGui::Text("Use 'Database Connection' window to create new connections.");
            }
            else
            {
                ImGui::Text("Total Connections: %d", static_cast<int>(m_app->m_databaseConnections.size()));
                ImGui::SameLine();
                if (m_app->m_activeConnectionIndex >= 0)
                {
                    ImGui::Text("| Active: %s", m_app->m_databaseConnections[m_app->m_activeConnectionIndex]->GetConnectionName().c_str());
                }
                else
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "| No active connection selected");
                }
                ImGui::Separator();

                // Table of connections
                if (ImGui::BeginTable("ActiveConnectionsTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
                {
                    ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Driver", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    ImGui::TableSetupColumn("Server", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    ImGui::TableSetupColumn("Database", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    int indexToRemove = -1;
                    int indexToSetActive = -1;

                    for (int i = 0; i < static_cast<int>(m_app->m_databaseConnections.size()); ++i)
                    {
                        auto& conn = m_app->m_databaseConnections[i];
                        if (!conn) continue;

                        ImGui::TableNextRow();

                        // Active indicator
                        ImGui::TableSetColumnIndex(0);
                        if (i == m_app->m_activeConnectionIndex)
                        {
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "✓ Active");
                        }
                        else
                        {
                            ImGui::Text("   ");
                        }

                        // Name
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(conn->GetConnectionName().c_str());

                        // Driver
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextUnformatted(conn->GetDriverName().c_str());

                        // Server
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(conn->GetServerName().c_str());

                        // Database
                        ImGui::TableSetColumnIndex(4);
                        ImGui::TextUnformatted(conn->GetCurrentDatabaseName().c_str());

                        // Status
                        ImGui::TableSetColumnIndex(5);
                        if (conn->IsConnected())
                        {
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Connected");
                        }
                        else
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Disconnected");
                        }

                        // Actions
                        ImGui::TableSetColumnIndex(6);

                        ImGui::PushID(i);

                        if (i != m_app->m_activeConnectionIndex)
                        {
                            if (ImGui::SmallButton("Set Active"))
                            {
                                indexToSetActive = i;
                            }
                            ImGui::SameLine();
                        }

                        if (ImGui::SmallButton("Disconnect"))
                        {
                            indexToRemove = i;
                        }

                        ImGui::PopID();
                    }

                    ImGui::EndTable();

                    // Process actions after table rendering
                    if (indexToSetActive >= 0)
                    {
                        m_app->SetActiveConnection(indexToSetActive);
                        LOG_INFO("Set active connection: " + m_app->m_databaseConnections[indexToSetActive]->GetConnectionName());
                    }

                    if (indexToRemove >= 0)
                    {
                        auto& conn = m_app->m_databaseConnections[indexToRemove];
                        std::string name = conn->GetConnectionName();
                        conn->Disconnect();
                        m_app->m_databaseConnections.erase(m_app->m_databaseConnections.begin() + indexToRemove);

                        // Adjust active connection index
                        if (m_app->m_activeConnectionIndex == indexToRemove)
                        {
                            m_app->m_activeConnectionIndex = -1;
                        }
                        else if (m_app->m_activeConnectionIndex > indexToRemove)
                        {
                            m_app->m_activeConnectionIndex--;
                        }

                        LOG_INFO("Disconnected and removed connection: " + name);
                    }
                }
            }

            ImGui::EndTabItem();
        }

        // Tab 2: Connection History
        if (ImGui::BeginTabItem("Connection History"))
        {
            ImGui::TextWrapped("Previously successful connections. Click 'Reconnect' to quickly establish a saved connection.");
            ImGui::Text("Note: Passwords are not saved for security. You'll need to re-enter passwords for reconnection.");
            ImGui::Separator();

            if (m_app->m_connectionHistory.empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No connection history yet.");
                ImGui::Text("Successfully connect to a database to add it to history.");
            }
            else
            {
                ImGui::Text("Total History Entries: %d", static_cast<int>(m_app->m_connectionHistory.size()));
                ImGui::Separator();

                // Table of history
                if (ImGui::BeginTable("ConnectionHistoryTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable))
                {
                    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Driver", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    ImGui::TableSetupColumn("Server", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    ImGui::TableSetupColumn("Database", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Username", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                    ImGui::TableSetupColumn("Last Used", ImGuiTableColumnFlags_WidthFixed, 140.0f);
                    ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    int indexToReconnect = -1;
                    int indexToDelete = -1;
                    static char reconnectPasswordBuffer[256] = "";
                    static int reconnectingIndex = -1;

                    for (int i = 0; i < static_cast<int>(m_app->m_connectionHistory.size()); ++i)
                    {
                        const auto& entry = m_app->m_connectionHistory[i];

                        ImGui::TableNextRow();

                        // Name
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextUnformatted(entry.config.ConnectionName.c_str());

                        // Driver
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextUnformatted(entry.config.DriverName.c_str());

                        // Server
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextUnformatted(entry.config.ServerAddress.c_str());

                        // Database
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextUnformatted(entry.config.DatabaseName.c_str());

                        // Username
                        ImGui::TableSetColumnIndex(4);
                        if (entry.config.TrustedConnection)
                        {
                            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(Trusted)");
                        }
                        else
                        {
                            ImGui::TextUnformatted(entry.config.Username.c_str());
                        }

                        // Last Used
                        ImGui::TableSetColumnIndex(5);
                        ImGui::TextUnformatted(entry.lastUsedTimestamp.c_str());

                        // Actions
                        ImGui::TableSetColumnIndex(6);
                        ImGui::PushID(i);

                        // Show password input if this is the reconnecting entry
                        if (reconnectingIndex == i)
                        {
                            ImGui::Text("Password:");
                            ImGui::SameLine();
                            ImGui::SetNextItemWidth(100);
                            if (ImGui::InputText("##pwd", reconnectPasswordBuffer, sizeof(reconnectPasswordBuffer), ImGuiInputTextFlags_Password | ImGuiInputTextFlags_EnterReturnsTrue))
                            {
                                indexToReconnect = i;
                            }
                            ImGui::SameLine();
                            if (ImGui::SmallButton("Connect"))
                            {
                                indexToReconnect = i;
                            }
                            ImGui::SameLine();
                            if (ImGui::SmallButton("Cancel"))
                            {
                                reconnectingIndex = -1;
                                memset(reconnectPasswordBuffer, 0, sizeof(reconnectPasswordBuffer));
                            }
                        }
                        else
                        {
                            if (ImGui::SmallButton("Reconnect"))
                            {
                                if (entry.config.TrustedConnection || !entry.config.Username.empty())
                                {
                                    // If trusted connection or has username, prompt for password
                                    reconnectingIndex = i;
                                    memset(reconnectPasswordBuffer, 0, sizeof(reconnectPasswordBuffer));
                                }
                                else
                                {
                                    // No credentials needed
                                    indexToReconnect = i;
                                }
                            }
                            ImGui::SameLine();
                            if (ImGui::SmallButton("Delete"))
                            {
                                indexToDelete = i;
                            }
                        }

                        ImGui::PopID();
                    }

                    ImGui::EndTable();

                    // Process reconnect action
                    if (indexToReconnect >= 0)
                    {
                        Database::ConnectionConfig config = m_app->m_connectionHistory[indexToReconnect].config;

                        // If password was entered, use it
                        if (reconnectingIndex == indexToReconnect && strlen(reconnectPasswordBuffer) > 0)
                        {
                            config.Password = reconnectPasswordBuffer;
                        }

                        // Ensure unique connection name
                        int connNum = m_app->GenerateUniqueConnectionNumber();
                        if (config.ConnectionName.empty() || config.ConnectionName == "Connection 1")
                        {
                            config.ConnectionName = "Connection " + std::to_string(connNum);
                        }
                        else
                        {
                            config.ConnectionName = config.ConnectionName + " (" + std::to_string(connNum) + ")";
                        }

                        // Create new connection
                        auto newConnection = std::make_shared<Database::DatabaseManager>();
                        if (newConnection->Connect(config))
                        {
                            m_app->m_databaseConnections.push_back(newConnection);
                            m_app->SetActiveConnection(static_cast<int>(m_app->m_databaseConnections.size()) - 1);
                            m_app->AddConnectionToHistory(config);
                            LOG_INFO("Reconnected to: " + config.ConnectionName);
                            m_app->m_dbConnectionStatus = "Connected: " + config.ConnectionName;

                            // Clear password buffer
                            memset(reconnectPasswordBuffer, 0, sizeof(reconnectPasswordBuffer));
                            reconnectingIndex = -1;
                        }
                        else
                        {
                            LOG_ERROR("Failed to reconnect: " + newConnection->GetLastError());
                            m_app->m_dbConnectionStatus = "Connection failed: " + newConnection->GetLastError();
                        }
                    }

                    // Process delete action
                    if (indexToDelete >= 0)
                    {
                        std::string name = m_app->m_connectionHistory[indexToDelete].config.ConnectionName;
                        m_app->m_connectionHistory.erase(m_app->m_connectionHistory.begin() + indexToDelete);
                        m_app->SaveConnectionHistory();
                        LOG_INFO("Deleted connection history: " + name);

                        if (reconnectingIndex == indexToDelete)
                        {
                            reconnectingIndex = -1;
                            memset(reconnectPasswordBuffer, 0, sizeof(reconnectPasswordBuffer));
                        }
                        else if (reconnectingIndex > indexToDelete)
                        {
                            reconnectingIndex--;
                        }
                    }
                }
            }

            ImGui::Spacing();
            if (ImGui::Button("Clear All History"))
            {
                m_app->m_connectionHistory.clear();
                m_app->SaveConnectionHistory();
                LOG_INFO("Cleared all connection history");
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

