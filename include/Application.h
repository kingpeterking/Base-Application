#pragma once

#include "pch.h"
#include "Tools/Settings.h"
#include "WindowFunctions/WindowManager.h"
#include "Tools/Web/HTTPClient.h"
#include "Tools/Web/WebServer.h"
#include "Tools/ImPlotClient.h"
#include "WindowFunctions/WindowFunctions.h"

struct GLFWwindow;

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void SetupImGui();
    void RenderFrame();
    void SetupWindows();

    // Window rendering functions (delegated to WindowFunctions)
    void RenderMainMenu(bool* isOpen);
    void RenderDemoWindow(bool* isOpen);
    void RenderApplicationWindow(bool* isOpen);
    void RenderImPlotDemoWindow(bool* isOpen);
    void RenderURLRequestWindow(bool* isOpen);
    void RenderFileExplorerWindow(bool* isOpen);
    void RenderLogViewerWindow(bool* isOpen);
    void RenderWebServerControlWindow(bool* isOpen);
    void RenderWebServerRequestsWindow(bool* isOpen);
    void RenderDatabaseConnectionWindow(bool* isOpen);
    void RenderDatabaseConnectionsManagerWindow(bool* isOpen);
    void RenderDatabaseObjectBrowserWindow(bool* isOpen);
    void RenderDatabaseTablesWindow(bool* isOpen);
    void RenderDatabaseViewsWindow(bool* isOpen);
    void RenderDatabaseProceduresWindow(bool* isOpen);
    void RenderDatabaseSchemaViewerWindow(bool* isOpen);

    GLFWwindow* m_window;
    ImVec4 m_clearColor;
    bool m_showMainMenu;

    // Tool clients
    HTTPClient m_httpClient;
    WebServer m_webServer;
    ImPlotClient m_implotClient;
    std::unique_ptr<WindowFunctions> m_windowFunctions;

    // Database connections - support multiple simultaneous connections
    Database::DatabaseManager m_databaseManager; // Primary/legacy single connection
    std::vector<std::shared_ptr<Database::DatabaseManager>> m_databaseConnections;
    int m_activeConnectionIndex; // Index of currently active connection (-1 = none)

    // Connection history for quick reconnect
    struct ConnectionHistoryEntry {
        Database::ConnectionConfig config;
        std::string lastUsedTimestamp;
    };
    std::vector<ConnectionHistoryEntry> m_connectionHistory;

    // Helper methods for connection management
    std::shared_ptr<Database::DatabaseManager> GetActiveConnection();
    void SetActiveConnection(int index);
    void AddConnectionToHistory(const Database::ConnectionConfig& config);
    void LoadConnectionHistory();
    void SaveConnectionHistory();
    int GenerateUniqueConnectionNumber(); // For auto-naming connections

    // URL Request state
    static constexpr size_t URL_BUFFER_SIZE = 512;
    static constexpr size_t PAYLOAD_BUFFER_SIZE = 4096;
    static constexpr size_t PARAM_BUFFER_SIZE = 256;
    char m_urlBuffer[URL_BUFFER_SIZE];
    char m_payloadBuffer[PAYLOAD_BUFFER_SIZE];
    char m_paramKeyBuffer[PARAM_BUFFER_SIZE];
    char m_paramValueBuffer[PARAM_BUFFER_SIZE];
    int m_selectedHTTPMethod;
    std::map<std::string, std::string> m_requestParameters;

    // Database connection state
    static constexpr size_t DB_BUFFER_SIZE = 256;
    char m_dbConnectionNameBuffer[DB_BUFFER_SIZE]; // New: connection name
    char m_dbDriverBuffer[DB_BUFFER_SIZE];
    char m_dbServerBuffer[DB_BUFFER_SIZE];
    char m_dbPortBuffer[16];
    char m_dbDatabaseBuffer[DB_BUFFER_SIZE];
    char m_dbUsernameBuffer[DB_BUFFER_SIZE];
    char m_dbPasswordBuffer[DB_BUFFER_SIZE];
    char m_dbConnectionStringBuffer[1024];
    char m_dbDSNBuffer[DB_BUFFER_SIZE];  // DSN name
    char m_dbDSNUsernameBuffer[DB_BUFFER_SIZE];  // Optional DSN credentials
    char m_dbDSNPasswordBuffer[DB_BUFFER_SIZE];  // Optional DSN credentials
    bool m_dbTrustedConnection;
    bool m_dbEncrypt;
    int m_dbConnectionTimeout;
    int m_dbCommandTimeout;
    int m_dbSelectedConnectionMode; // 0 = Form, 1 = Connection String, 2 = DSN, 3 = Quick Connect
    std::string m_dbConnectionStatus;
    std::vector<std::string> m_availableDrivers;

    // Window management
    WindowManager m_windowManager;

    // Settings management
    Settings m_settings;

    // Settings keys
    void LoadSettings();
    void SaveSettings();

    // Friend class for window rendering
    friend class WindowFunctions;
};

