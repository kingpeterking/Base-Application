#include "pch.h"
#include "Tools/Database/Database.h"
#include "Tools/Logger.h"
#include <sstream>

namespace Database {

    Database::Database(const std::string& name, std::shared_ptr<nanodbc::connection> connection)
        : m_Name(name), m_Connection(connection) {
    }

    bool Database::IsConnected() const {
        return m_Connection && m_Connection->connected();
    }

    std::shared_ptr<Table> Database::CreateTable(const std::string& tableName) {
        try {
            auto table = std::make_shared<Table>(tableName, m_Connection);
            m_Tables[tableName] = table;
            return table;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to create table object: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return nullptr;
        }
    }

    std::shared_ptr<Table> Database::GetTable(const std::string& tableName) {
        auto it = m_Tables.find(tableName);
        if (it != m_Tables.end()) {
            return it->second;
        }

        // Try to load the table if it exists
        if (TableExists(tableName)) {
            auto table = std::make_shared<Table>(tableName, m_Connection);
            if (table->LoadSchema()) {
                m_Tables[tableName] = table;
                return table;
            }
        }

        return nullptr;
    }

    bool Database::DropTable(const std::string& tableName) {
        try {
            auto table = GetTable(tableName);
            if (!table) {
                SetError("Table not found: " + tableName);
                return false;
            }

            if (table->Drop()) {
                m_Tables.erase(tableName);
                return true;
            }
            return false;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to drop table: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    bool Database::TableExists(const std::string& tableName) {
        try {
            nanodbc::catalog catalog(*m_Connection);
            auto tables = catalog.find_tables(tableName);
            return tables.next();
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to check table existence: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    std::vector<std::string> Database::GetTableNames() {
        std::vector<std::string> tableNames;
        try {
            nanodbc::catalog catalog(*m_Connection);
            auto tables = catalog.find_tables("", "TABLE");

            while (tables.next()) {
                tableNames.push_back(tables.table_name());
            }
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to get table names: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
        }
        return tableNames;
    }

    bool Database::LoadSchema() {
        try {
            m_Tables.clear();
            auto tableNames = GetTableNames();

            for (const auto& tableName : tableNames) {
                auto table = std::make_shared<Table>(tableName, m_Connection);
                if (table->LoadSchema()) {
                    m_Tables[tableName] = table;
                }
            }

            Logger::GetInstance().Info("Schema loaded for database: " + m_Name + 
                       " (" + std::to_string(m_Tables.size()) + " tables)", "Database");
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to load database schema: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    bool Database::ExecuteSQL(const std::string& sql) {
        try {
            nanodbc::execute(*m_Connection, sql);
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to execute SQL: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    Recordset Database::ExecuteQuery(const std::string& sql) {
        try {
            nanodbc::result result = nanodbc::execute(*m_Connection, sql);
            
            Recordset recordset;
            while (result.next()) {
                Record record;
                for (short col = 0; col < result.columns(); ++col) {
                    std::string columnName = result.column_name(col);
                    int dataType = result.column_datatype(col);
                    FieldType fieldType = SQLTypeToFieldType(dataType);

                    FieldMetadata metadata(columnName, fieldType);
                    Field field(metadata);

                    if (result.is_null(col)) {
                        field.SetNull();
                    }
                    else {
                        switch (fieldType) {
                            case FieldType::Integer:
                            case FieldType::SmallInt:
                            case FieldType::TinyInt:
                                field.SetValue(result.get<int32_t>(col));
                                break;
                            case FieldType::BigInt:
                                field.SetValue(result.get<int64_t>(col));
                                break;
                            case FieldType::Float:
                                field.SetValue(result.get<float>(col));
                                break;
                            case FieldType::Double:
                            case FieldType::Decimal:
                            case FieldType::Numeric:
                                field.SetValue(result.get<double>(col));
                                break;
                            case FieldType::Boolean:
                                field.SetValue(result.get<int32_t>(col) != 0);
                                break;
                            default:
                                field.SetValue(result.get<std::string>(col));
                                break;
                        }
                    }

                    record.AddField(field);
                }
                recordset.AddRecord(record);
            }

            return recordset;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to execute query: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return Recordset();
        }
    }

    bool Database::BeginTransaction() {
        try {
            // Create a transaction object
            m_Transaction = std::make_unique<nanodbc::transaction>(*m_Connection);
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to begin transaction: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    bool Database::CommitTransaction() {
        try {
            if (m_Transaction) {
                m_Transaction->commit();
                m_Transaction.reset();
            }
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to commit transaction: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    bool Database::RollbackTransaction() {
        try {
            if (m_Transaction) {
                m_Transaction->rollback();
                m_Transaction.reset();
            }
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to rollback transaction: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
            return false;
        }
    }

    std::string Database::GetDatabaseProduct() {
        try {
            return m_Connection->dbms_name();
        }
        catch (const std::exception&) {
            return "Unknown";
        }
    }

    std::string Database::GetDatabaseVersion() {
        try {
            return m_Connection->dbms_version();
        }
        catch (const std::exception&) {
            return "Unknown";
        }
    }

    std::string Database::GetDriverName() {
        try {
            return m_Connection->driver_name();
        }
        catch (const std::exception&) {
            return "Unknown";
        }
    }

    std::string Database::GetDriverVersion() {
        try {
            std::stringstream ss;
            ss << m_Connection->driver_version();
            return ss.str();
        }
        catch (const std::exception&) {
            return "Unknown";
        }
    }

    void Database::SetError(const std::string& error) {
        m_LastError = error;
    }

} // namespace Database
