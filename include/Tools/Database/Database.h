#pragma once

#include "Tools/Database/Table.h"
#include <nanodbc/nanodbc.h>
#include <memory>
#include <map>
#include <string>
#include <vector>

namespace Database {

    // Database class - represents a database instance
    class Database {
    public:
        Database(const std::string& name, std::shared_ptr<nanodbc::connection> connection);
        ~Database() = default;

        // Basic properties
        const std::string& GetName() const { return m_Name; }
        bool IsConnected() const;

        // Table management
        std::shared_ptr<Table> CreateTable(const std::string& tableName);
        std::shared_ptr<Table> GetTable(const std::string& tableName);
        bool DropTable(const std::string& tableName);
        bool TableExists(const std::string& tableName);
        std::vector<std::string> GetTableNames();

        // Load all tables from the database
        bool LoadSchema();

        // Get all tables
        const std::map<std::string, std::shared_ptr<Table>>& GetTables() const { return m_Tables; }

        // Execute SQL
        bool ExecuteSQL(const std::string& sql);
        Recordset ExecuteQuery(const std::string& sql);

        // Transaction management
        bool BeginTransaction();
        bool CommitTransaction();
        bool RollbackTransaction();

        // Database information
        std::string GetDatabaseProduct();
        std::string GetDatabaseVersion();
        std::string GetDriverName();
        std::string GetDriverVersion();

        // Get last error
        const std::string& GetLastError() const { return m_LastError; }

    private:
        std::string m_Name;
        std::shared_ptr<nanodbc::connection> m_Connection;
        std::unique_ptr<nanodbc::transaction> m_Transaction;
        std::map<std::string, std::shared_ptr<Table>> m_Tables;
        std::string m_LastError;

        void SetError(const std::string& error);
    };

} // namespace Database
