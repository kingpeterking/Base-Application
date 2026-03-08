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

        ImGui::End();
    }
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

        ImGui::End();
    }
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

        ImGui::End();
    }
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

        ImGui::End();
    }
}

void WindowFunctions::RenderLogViewerWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(1000, 600), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Log Viewer", isOpen))
    {
        // Filter checkboxes
        static bool showInfo = true;
        static bool showWarning = true;
        static bool showError = true;
        static bool showDebug = false;
        static bool autoScroll = true;

        ImGui::Text("Filters:");
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

        // Log table
        if (ImGui::BeginTable("logs_table", 4, 
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
            ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("Time", ImGuiTableColumnFlags_WidthFixed, 100);
            ImGui::TableSetupColumn("Level", ImGuiTableColumnFlags_WidthFixed, 80);
            ImGui::TableSetupColumn("Source", ImGuiTableColumnFlags_WidthFixed, 150);
            ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupScrollFreeze(0, 1); // Freeze header row
            ImGui::TableHeadersRow();

            // Get filtered logs
            auto logs = Logger::GetInstance().GetFilteredLogs(showInfo, showWarning, showError, showDebug);

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
            }

            // Auto-scroll to bottom
            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            {
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::EndTable();
        }

        ImGui::End();
    }
}
