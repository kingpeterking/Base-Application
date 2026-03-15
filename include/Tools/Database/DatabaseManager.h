#pragma once

#include "Tools/Database/Database.h"
#include <nanodbc/nanodbc.h>
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace Database {

    // Connection configuration
    struct ConnectionConfig {
        std::string ConnectionName;     // User-friendly connection name
        std::string DriverName;         // ODBC driver name (e.g., "SQL Server", "PostgreSQL", "MySQL")
        std::string ServerAddress;      // Server address or hostname
        int ServerPort = 0;             // Server port (0 = use default)
        std::string DatabaseName;       // Database name
        std::string Username;           // Username for authentication
        std::string Password;           // Password for authentication
        bool TrustedConnection = false; // Use Windows authentication (SQL Server)
        bool Encrypt = false;           // Use encrypted connection
        std::string AdditionalOptions;  // Additional connection string options
        int ConnectionTimeout = 30;     // Connection timeout in seconds
        int CommandTimeout = 30;        // Command timeout in seconds
        std::string OriginalConnectionString; // Original connection string for reconnection (passwords removed)

        // Build ODBC connection string
        std::string BuildConnectionString() const;
    };

    // DatabaseManager class - manages database connections and database lifecycle
    class DatabaseManager {
    public:
        DatabaseManager() = default;
        ~DatabaseManager();

        // Connection management
        bool Connect(const ConnectionConfig& config);
        bool ConnectWithConnectionString(const std::string& connectionString, const ConnectionConfig& config = ConnectionConfig());
        void Disconnect();
        bool IsConnected() const;

        // Get current database
        std::shared_ptr<Database> GetDatabase();

        // Database operations
        bool CreateDatabase(const std::string& databaseName);
        bool DropDatabase(const std::string& databaseName);
        bool DatabaseExists(const std::string& databaseName);
        bool SwitchDatabase(const std::string& databaseName);
        std::vector<std::string> GetDatabaseNames();

        // Database object inspection (delegates to current database)
        std::vector<std::string> GetTableNames();
        std::vector<std::string> GetViewNames();
        std::vector<std::string> GetStoredProcedureNames();
        std::vector<Database::DatabaseObjectInfo> GetDatabaseObjects(const std::string& typeFilter = "");

        // Connection information
        std::string GetConnectionName() const { return m_Config.ConnectionName; }
        std::string GetCurrentDatabaseName() const { return m_CurrentDatabaseName; }
        std::string GetServerName() const;
        std::string GetDatabaseProduct() const;
        std::string GetDatabaseVersion() const;
        std::string GetDriverName() const;
        std::string GetDriverVersion() const;
        std::string GetUsername() const { return m_Config.Username; }
        std::string GetOriginalConnectionString() const { return m_Config.OriginalConnectionString; }
        ConnectionConfig GetConfig() const { return m_Config; }

        // ODBC drivers
        static std::vector<std::string> GetAvailableDrivers();

        // Test connection
        static bool TestConnection(const ConnectionConfig& config, std::string& errorMessage);
        static bool TestConnectionString(const std::string& connectionString, std::string& errorMessage);

        // Get last error
        const std::string& GetLastError() const { return m_LastError; }

        // Get connection
        std::shared_ptr<nanodbc::connection> GetConnection() { return m_Connection; }

    private:
        std::shared_ptr<nanodbc::connection> m_Connection;
        std::shared_ptr<Database> m_CurrentDatabase;
        std::string m_CurrentDatabaseName;
        ConnectionConfig m_Config;
        std::string m_LastError;

        void SetError(const std::string& error);
        bool ReconnectToDatabase(const std::string& databaseName);
    };

} // namespace Database
