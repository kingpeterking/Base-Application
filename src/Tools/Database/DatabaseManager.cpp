#include "pch.h"
#include "Tools/Database/DatabaseManager.h"
#include "Tools/Logger.h"
#include <sstream>

namespace Database {

    std::string ConnectionConfig::BuildConnectionString() const {
        std::stringstream ss;

        // Driver
        ss << "Driver={" << DriverName << "};";

        // Detect if this is an MS Access driver
        bool isAccessDriver = (DriverName.find("Access") != std::string::npos) ||
                              (DriverName.find("access") != std::string::npos);

        // Server (or DBQ for Access)
        if (!ServerAddress.empty()) {
            if (isAccessDriver) {
                // For MS Access, use DBQ= instead of Server=
                ss << "DBQ=" << ServerAddress << ";";
            }
            else {
                ss << "Server=" << ServerAddress;
                if (ServerPort > 0) {
                    ss << "," << ServerPort;
                }
                ss << ";";
            }
        }

        // Database (skip for Access since it uses DBQ)
        if (!DatabaseName.empty() && !isAccessDriver) {
            ss << "Database=" << DatabaseName << ";";
        }

        // Authentication
        if (TrustedConnection) {
            ss << "Trusted_Connection=Yes;";
        }
        else {
            if (!Username.empty()) {
                ss << "Uid=" << Username << ";";
            }
            if (!Password.empty()) {
                ss << "Pwd=" << Password << ";";
            }
        }

        // Encryption
        if (Encrypt) {
            ss << "Encrypt=Yes;";
        }

        // Timeouts
        if (ConnectionTimeout > 0) {
            ss << "Connection Timeout=" << ConnectionTimeout << ";";
        }

        // Additional options
        if (!AdditionalOptions.empty()) {
            ss << AdditionalOptions;
            if (AdditionalOptions.back() != ';') {
                ss << ";";
            }
        }

        return ss.str();
    }

    DatabaseManager::~DatabaseManager() {
        Disconnect();
    }

    bool DatabaseManager::Connect(const ConnectionConfig& config) {
        try {
            m_Config = config;
            std::string connectionString = config.BuildConnectionString();

            // Store connection string without password for duplicate detection
            ConnectionConfig tempConfig = config;
            tempConfig.Password = "";  // Remove password
            m_Config.OriginalConnectionString = tempConfig.BuildConnectionString();

            Logger::GetInstance().Info("Connecting to database: " + config.DatabaseName, "DatabaseManager");
            Logger::GetInstance().Info("Connection string: " + connectionString, "DatabaseManager");

            m_Connection = std::make_shared<nanodbc::connection>(connectionString);

            if (m_Connection->connected()) {
                m_CurrentDatabaseName = config.DatabaseName;
                m_CurrentDatabase = std::make_shared<Database>(config.DatabaseName, m_Connection);

                Logger::GetInstance().Info("Connected to " + GetDatabaseProduct() + " " + GetDatabaseVersion(), "DatabaseManager");
                Logger::GetInstance().Info("Driver: " + GetDriverName() + " " + GetDriverVersion(), "DatabaseManager");
                return true;
            }

            SetError("Failed to connect to database");
            return false;
        }
        catch (const std::exception& e) {
            SetError(std::string("Connection failed: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    bool DatabaseManager::ConnectWithConnectionString(const std::string& connectionString, const ConnectionConfig& config) {
        try {
            // Store the config (especially the connection name)
            m_Config = config;

            Logger::GetInstance().Info("Connecting with connection string", "DatabaseManager");
            m_Connection = std::make_shared<nanodbc::connection>(connectionString);

            if (m_Connection->connected()) {
                m_CurrentDatabaseName = m_Connection->database_name();
                m_CurrentDatabase = std::make_shared<Database>(m_CurrentDatabaseName, m_Connection);

                // If connection name wasn't provided, generate a default one
                if (m_Config.ConnectionName.empty()) {
                    m_Config.ConnectionName = "Connection (unnamed)";
                }

                Logger::GetInstance().Info("Connected to " + GetDatabaseProduct() + " " + GetDatabaseVersion(), "DatabaseManager");
                Logger::GetInstance().Info("Driver: " + GetDriverName() + " " + GetDriverVersion(), "DatabaseManager");
                return true;
            }

            SetError("Failed to connect to database");
            return false;
        }
        catch (const std::exception& e) {
            SetError(std::string("Connection failed: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    void DatabaseManager::Disconnect() {
        try {
            if (m_Connection && m_Connection->connected()) {
                m_CurrentDatabase.reset();
                m_Connection->disconnect();
                Logger::GetInstance().Info("Disconnected from database", "DatabaseManager");
            }
            m_Connection.reset();
            m_CurrentDatabaseName.clear();
        }
        catch (const std::exception& e) {
            SetError(std::string("Disconnect failed: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
        }
    }

    bool DatabaseManager::IsConnected() const {
        return m_Connection && m_Connection->connected();
    }

    std::shared_ptr<Database> DatabaseManager::GetDatabase() {
        return m_CurrentDatabase;
    }

    bool DatabaseManager::CreateDatabase(const std::string& databaseName) {
        try {
            if (!IsConnected()) {
                SetError("Not connected to server");
                return false;
            }

            // Connect to master database to create new database
            std::string sql = "CREATE DATABASE " + databaseName;
            nanodbc::execute(*m_Connection, sql);

            Logger::GetInstance().Info("Database created: " + databaseName, "DatabaseManager");
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to create database: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    bool DatabaseManager::DropDatabase(const std::string& databaseName) {
        try {
            if (!IsConnected()) {
                SetError("Not connected to server");
                return false;
            }

            // Cannot drop current database, switch to master first
            if (databaseName == m_CurrentDatabaseName) {
                if (!SwitchDatabase("master")) {
                    return false;
                }
            }

            std::string sql = "DROP DATABASE " + databaseName;
            nanodbc::execute(*m_Connection, sql);

            Logger::GetInstance().Info("Database dropped: " + databaseName, "DatabaseManager");
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to drop database: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    bool DatabaseManager::DatabaseExists(const std::string& databaseName) {
        try {
            if (!IsConnected()) {
                return false;
            }

            std::string sql = "SELECT name FROM sys.databases WHERE name = '" + databaseName + "'";
            nanodbc::result result = nanodbc::execute(*m_Connection, sql);
            return result.next();
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to check database existence: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    bool DatabaseManager::SwitchDatabase(const std::string& databaseName) {
        try {
            if (!IsConnected()) {
                SetError("Not connected to server");
                return false;
            }

            // Use the database
            std::string sql = "USE " + databaseName;
            nanodbc::execute(*m_Connection, sql);

            m_CurrentDatabaseName = databaseName;
            m_CurrentDatabase = std::make_shared<Database>(databaseName, m_Connection);

            Logger::GetInstance().Info("Switched to database: " + databaseName, "DatabaseManager");
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to switch database: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    std::vector<std::string> DatabaseManager::GetDatabaseNames() {
        std::vector<std::string> databaseNames;
        try {
            if (!IsConnected()) {
                return databaseNames;
            }

            // SQL Server query
            std::string sql = "SELECT name FROM sys.databases ORDER BY name";
            nanodbc::result result = nanodbc::execute(*m_Connection, sql);

            while (result.next()) {
                databaseNames.push_back(result.get<std::string>(0));
            }
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to get database names: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
        }
        return databaseNames;
    }

    std::string DatabaseManager::GetServerName() const {
        try {
            if (m_Connection && m_Connection->connected()) {
                // nanodbc doesn't have server_name(), use config or database catalog info
                if (!m_Config.ServerAddress.empty()) {
                    return m_Config.ServerAddress;
                }
                return m_Connection->database_name();
            }
        }
        catch (const std::exception&) {
        }
        return "Unknown";
    }

    std::string DatabaseManager::GetDatabaseProduct() const {
        try {
            if (m_Connection && m_Connection->connected()) {
                return m_Connection->dbms_name();
            }
        }
        catch (const std::exception&) {
        }
        return "Unknown";
    }

    std::string DatabaseManager::GetDatabaseVersion() const {
        try {
            if (m_Connection && m_Connection->connected()) {
                return m_Connection->dbms_version();
            }
        }
        catch (const std::exception&) {
        }
        return "Unknown";
    }

    std::string DatabaseManager::GetDriverName() const {
        try {
            if (m_Connection && m_Connection->connected()) {
                return m_Connection->driver_name();
            }
        }
        catch (const std::exception&) {
        }
        return "Unknown";
    }

    std::string DatabaseManager::GetDriverVersion() const {
        try {
            if (m_Connection && m_Connection->connected()) {
                std::stringstream ss;
                ss << m_Connection->driver_version();
                return ss.str();
            }
        }
        catch (const std::exception&) {
        }
        return "Unknown";
    }

    std::vector<std::string> DatabaseManager::GetAvailableDrivers() {
        std::vector<std::string> drivers;
        try {
            auto driverList = nanodbc::list_drivers();
            for (const auto& driver : driverList) {
                drivers.push_back(driver.name);
            }
        }
        catch (const std::exception& e) {
            Logger::GetInstance().Error(std::string("Failed to list drivers: ") + e.what(), "DatabaseManager");
        }
        return drivers;
    }

    bool DatabaseManager::TestConnection(const ConnectionConfig& config, std::string& errorMessage) {
        try {
            std::string connectionString = config.BuildConnectionString();
            nanodbc::connection conn(connectionString);
            
            if (conn.connected()) {
                conn.disconnect();
                return true;
            }
            errorMessage = "Connection failed";
            return false;
        }
        catch (const std::exception& e) {
            errorMessage = e.what();
            return false;
        }
    }

    bool DatabaseManager::TestConnectionString(const std::string& connectionString, std::string& errorMessage) {
        try {
            nanodbc::connection conn(connectionString);
            
            if (conn.connected()) {
                conn.disconnect();
                return true;
            }
            errorMessage = "Connection failed";
            return false;
        }
        catch (const std::exception& e) {
            errorMessage = e.what();
            return false;
        }
    }

    bool DatabaseManager::ReconnectToDatabase(const std::string& databaseName) {
        try {
            Disconnect();
            m_Config.DatabaseName = databaseName;
            return Connect(m_Config);
        }
        catch (const std::exception& e) {
            SetError(std::string("Reconnection failed: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "DatabaseManager");
            return false;
        }
    }

    void DatabaseManager::SetError(const std::string& error) {
        m_LastError = error;
    }

    std::vector<std::string> DatabaseManager::GetTableNames() {
        if (!m_CurrentDatabase) {
            SetError("No database selected");
            return std::vector<std::string>();
        }
        return m_CurrentDatabase->GetTableNames();
    }

    std::vector<std::string> DatabaseManager::GetViewNames() {
        if (!m_CurrentDatabase) {
            SetError("No database selected");
            return std::vector<std::string>();
        }
        return m_CurrentDatabase->GetViewNames();
    }

    std::vector<std::string> DatabaseManager::GetStoredProcedureNames() {
        if (!m_CurrentDatabase) {
            SetError("No database selected");
            return std::vector<std::string>();
        }
        return m_CurrentDatabase->GetStoredProcedureNames();
    }

    std::vector<Database::DatabaseObjectInfo> DatabaseManager::GetDatabaseObjects(const std::string& typeFilter) {
        if (!m_CurrentDatabase) {
            SetError("No database selected");
            return std::vector<Database::DatabaseObjectInfo>();
        }
        return m_CurrentDatabase->GetDatabaseObjects(typeFilter);
    }

} // namespace Database
