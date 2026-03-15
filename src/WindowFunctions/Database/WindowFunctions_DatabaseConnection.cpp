#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "Tools/Logger.h"

// ============================================================================
// DATABASE CONNECTION WINDOW FUNCTIONS
// ============================================================================
// Database Connection, Database Connections Manager
// ============================================================================

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

                    // Check for duplicate connection
                    std::string driverName = config.DriverName;
                    std::string serverAddr = config.ServerAddress;
                    std::string dbName = config.DatabaseName;
                    std::string username = config.Username;
                    int port = config.ServerPort;

                    bool isDuplicate = false;
                    std::string existingConnName;
                    for (const auto& existingConn : m_app->m_databaseConnections)
                    {
                        if (existingConn && existingConn->IsConnected())
                        {
                            // Compare connection parameters (case-insensitive for server names)
                            bool sameDriver = (existingConn->GetDriverName() == driverName);
                            bool sameServer = (_stricmp(existingConn->GetServerName().c_str(), serverAddr.c_str()) == 0);
                            bool sameDatabase = (existingConn->GetCurrentDatabaseName() == dbName);

                            // For MS Access, only compare server (file path) since database is empty
                            bool isAccessDriver = (driverName.find("Access") != std::string::npos);

                            if (isAccessDriver)
                            {
                                if (sameDriver && sameServer)
                                {
                                    isDuplicate = true;
                                    existingConnName = existingConn->GetConnectionName();
                                    break;
                                }
                            }
                            else
                            {
                                // For regular databases, compare driver, server, database, and username
                                // Note: Not comparing username for trusted connections
                                bool sameCredentials = config.TrustedConnection || 
                                    (existingConn->GetUsername() == username);

                                if (sameDriver && sameServer && sameDatabase && sameCredentials)
                                {
                                    isDuplicate = true;
                                    existingConnName = existingConn->GetConnectionName();
                                    break;
                                }
                            }
                        }
                    }

                    if (isDuplicate)
                    {
                        m_app->m_dbConnectionStatus = "Connection already exists: " + existingConnName + 
                            ". Cannot create duplicate connection.";
                        LOG_WARNING_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                    else
                    {
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


                if (ImGui::Button("Connect", ImVec2(120, 0)))
                {
                    // Generate connection name
                    int connNum = m_app->GenerateUniqueConnectionNumber();
                    std::string connectionName = "Connection " + std::to_string(connNum);

                    // Normalize connection string (remove password) for storage and duplicate detection
                    std::string currentConnStr = m_app->m_dbConnectionStringBuffer;
                    std::string normalizedConnStr = currentConnStr;
                    size_t pwdPos = normalizedConnStr.find("Pwd=");
                    if (pwdPos == std::string::npos) pwdPos = normalizedConnStr.find("PWD=");
                    if (pwdPos == std::string::npos) pwdPos = normalizedConnStr.find("Password=");
                    if (pwdPos != std::string::npos)
                    {
                        size_t endPos = normalizedConnStr.find(';', pwdPos);
                        if (endPos != std::string::npos)
                        {
                            normalizedConnStr.erase(pwdPos, endPos - pwdPos + 1);
                        }
                        else
                        {
                            normalizedConnStr.erase(pwdPos);
                        }
                    }

                    // Create connection config with name and original connection string
                    Database::ConnectionConfig tempConfig;
                    tempConfig.ConnectionName = connectionName;
                    tempConfig.OriginalConnectionString = normalizedConnStr;

                    // Check for duplicate connection by comparing connection strings (without password)
                    bool isDuplicate = false;
                    std::string existingConnName;
                    for (const auto& existingConn : m_app->m_databaseConnections)
                    {
                        if (existingConn && existingConn->IsConnected())
                        {
                            std::string existingOriginalStr = existingConn->GetOriginalConnectionString();
                            if (!existingOriginalStr.empty())
                            {
                                // Case-insensitive comparison
                                if (_stricmp(normalizedConnStr.c_str(), existingOriginalStr.c_str()) == 0)
                                {
                                    isDuplicate = true;
                                    existingConnName = existingConn->GetConnectionName();
                                    break;
                                }
                            }
                        }
                    }

                    if (isDuplicate)
                    {
                        m_app->m_dbConnectionStatus = "Connection already exists: " + existingConnName + 
                            ". Cannot create duplicate connection.";
                        LOG_WARNING_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                    else
                    {
                        // Create new connection manager
                        auto newConnection = std::make_shared<Database::DatabaseManager>();
                    if (newConnection->ConnectWithConnectionString(m_app->m_dbConnectionStringBuffer, tempConfig))
                    {
                        // Add to connections list
                        m_app->m_databaseConnections.push_back(newConnection);
                        m_app->SetActiveConnection(static_cast<int>(m_app->m_databaseConnections.size()) - 1);

                        // Create config for history (reuse tempConfig but add additional info)
                        Database::ConnectionConfig config = tempConfig;
                        config.DriverName = newConnection->GetDriverName();
                        config.ServerAddress = newConnection->GetServerName();
                        config.DatabaseName = newConnection->GetCurrentDatabaseName();

                        // Add to history
                        m_app->AddConnectionToHistory(config);

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

                    // Check for duplicate DSN connection
                    std::string currentDSN = m_app->m_dbDSNBuffer;
                    bool isDuplicate = false;
                    std::string existingConnName;

                    for (const auto& existingConn : m_app->m_databaseConnections)
                    {
                        if (existingConn && existingConn->IsConnected())
                        {
                            std::string existingOriginalStr = existingConn->GetOriginalConnectionString();
                            // Check if this is a DSN connection by looking for "DSN=" in the connection string
                            if (existingOriginalStr.find("DSN=") != std::string::npos ||
                                existingOriginalStr.find("dsn=") != std::string::npos)
                            {
                                // Extract DSN name from existing connection
                                size_t dsnPos = existingOriginalStr.find("DSN=");
                                if (dsnPos == std::string::npos)
                                    dsnPos = existingOriginalStr.find("dsn=");

                                if (dsnPos != std::string::npos)
                                {
                                    dsnPos += 4; // Skip "DSN="
                                    size_t endPos = existingOriginalStr.find(';', dsnPos);
                                    std::string existingDSN;
                                    if (endPos != std::string::npos)
                                    {
                                        existingDSN = existingOriginalStr.substr(dsnPos, endPos - dsnPos);
                                    }
                                    else
                                    {
                                        existingDSN = existingOriginalStr.substr(dsnPos);
                                    }

                                    // Compare DSN names (case-insensitive)
                                    if (_stricmp(currentDSN.c_str(), existingDSN.c_str()) == 0)
                                    {
                                        isDuplicate = true;
                                        existingConnName = existingConn->GetConnectionName();
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    if (isDuplicate)
                    {
                        m_app->m_dbConnectionStatus = "DSN connection already exists: " + existingConnName + 
                            ". Cannot create duplicate connection to the same DSN.";
                        LOG_WARNING_SRC(m_app->m_dbConnectionStatus, "Database");
                    }
                    else
                    {
                        // Generate unique connection name
                        int connNum = m_app->GenerateUniqueConnectionNumber();
                        std::string connectionName = "DSN: " + std::string(m_app->m_dbDSNBuffer) + " (" + std::to_string(connNum) + ")";

                        // Build original DSN connection string (without password) for storage and duplicate detection
                        std::string originalDsnStr = "DSN=" + std::string(m_app->m_dbDSNBuffer) + ";";
                        if (strlen(m_app->m_dbDSNUsernameBuffer) > 0)
                        {
                            originalDsnStr += "Uid=" + std::string(m_app->m_dbDSNUsernameBuffer) + ";";
                        }

                        // Create connection config with name and original connection string
                        Database::ConnectionConfig tempConfig;
                        tempConfig.ConnectionName = connectionName;
                        tempConfig.OriginalConnectionString = originalDsnStr;
                        tempConfig.Username = m_app->m_dbDSNUsernameBuffer;
                        tempConfig.TrustedConnection = (strlen(m_app->m_dbDSNUsernameBuffer) == 0);

                        // Create new connection manager
                        auto newConnection = std::make_shared<Database::DatabaseManager>();
                    if (newConnection->ConnectWithConnectionString(dsnConnectionString, tempConfig))
                    {
                        // Add to connections list
                        m_app->m_databaseConnections.push_back(newConnection);
                        m_app->SetActiveConnection(static_cast<int>(m_app->m_databaseConnections.size()) - 1);

                        // Create config for history (reuse tempConfig but add additional info)
                        Database::ConnectionConfig config = tempConfig;
                        config.DriverName = newConnection->GetDriverName();
                        config.ServerAddress = newConnection->GetServerName();
                        config.DatabaseName = newConnection->GetCurrentDatabaseName();

                        // Add to history
                        m_app->AddConnectionToHistory(config);

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

    ImGui::End();
}

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
                            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), ">> Active");
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
                                // Only prompt for password if NOT trusted connection AND has username (SQL Auth)
                                if (!entry.config.TrustedConnection && !entry.config.Username.empty())
                                {
                                    // SQL Auth - prompt for password
                                    reconnectingIndex = i;
                                    memset(reconnectPasswordBuffer, 0, sizeof(reconnectPasswordBuffer));
                                }
                                else
                                {
                                    // Trusted connection or no credentials - just reconnect
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
                        bool connectSuccess = false;

                        // If we have an original connection string, use it (for Connection String/DSN connections)
                        if (!config.OriginalConnectionString.empty())
                        {
                            std::string reconnectString = config.OriginalConnectionString;

                            // Add password if provided
                            if (!config.Password.empty())
                            {
                                if (reconnectString.back() != ';') reconnectString += ";";
                                reconnectString += "Pwd=" + config.Password + ";";
                            }

                            LOG_INFO("Reconnecting with original connection string");
                            connectSuccess = newConnection->ConnectWithConnectionString(reconnectString);
                        }
                        // Special handling for MS Access (file-based database)
                        else if (config.DriverName.find("Access") != std::string::npos)
                        {
                            // Build MS Access connection string with DBQ parameter
                            std::string accessConnStr = "Driver={" + config.DriverName + "};";
                            accessConnStr += "DBQ=" + config.ServerAddress + ";";

                            if (!config.Password.empty())
                            {
                                accessConnStr += "PWD=" + config.Password + ";";
                            }

                            LOG_INFO("Reconnecting MS Access database with DBQ connection string");
                            connectSuccess = newConnection->ConnectWithConnectionString(accessConnStr);
                        }
                        else
                        {
                            // Use standard Connect method for Form connections
                            LOG_INFO("Reconnecting with connection config");
                            connectSuccess = newConnection->Connect(config);
                        }

                        if (connectSuccess)
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
