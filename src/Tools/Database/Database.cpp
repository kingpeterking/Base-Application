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

            // For MS Access, clear the default "dbo" schema
            std::string dbProduct = GetDatabaseProduct();
            if (dbProduct.find("ACCESS") != std::string::npos || dbProduct.find("Access") != std::string::npos) {
                table->SetSchema(""); // MS Access doesn't use schemas
            }

            if (table->LoadSchema()) {
                m_Tables[tableName] = table;
                return table;
            }
            else {
                // LoadSchema failed, but still return the table object so UI can show error
                LOG_ERROR("LoadSchema failed for table: " + tableName);
                return table;
            }
        }

        LOG_ERROR("Table does not exist: " + tableName);
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

    std::vector<std::string> Database::GetViewNames() {
        std::vector<std::string> viewNames;
        try {
            std::string dbProduct = GetDatabaseProduct();
            std::string sql;

            // Check if it's MS Access
            if (dbProduct.find("ACCESS") != std::string::npos || dbProduct.find("Access") != std::string::npos) {
                // MS Access: Use ODBC catalog to find views (queries)
                nanodbc::catalog catalog(*m_Connection);
                auto views = catalog.find_tables("", "VIEW");

                while (views.next()) {
                    viewNames.push_back(views.table_name());
                }
            }
            else {
                // ISO SQL Standard: Use INFORMATION_SCHEMA
                // This works for SQL Server, PostgreSQL, MySQL, etc.
                sql = "SELECT TABLE_NAME FROM INFORMATION_SCHEMA.VIEWS ORDER BY TABLE_NAME";
                nanodbc::result result = nanodbc::execute(*m_Connection, sql);

                while (result.next()) {
                    viewNames.push_back(result.get<std::string>(0));
                }
            }
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to get view names: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
        }
        return viewNames;
    }

    std::vector<std::string> Database::GetStoredProcedureNames() {
        std::vector<std::string> procedureNames;
        try {
            std::string dbProduct = GetDatabaseProduct();
            std::string sql;

            // Check if it's MS Access
            if (dbProduct.find("ACCESS") != std::string::npos || dbProduct.find("Access") != std::string::npos) {
                // MS Access doesn't support stored procedures in the traditional sense
                // Use ODBC catalog to find procedures
                nanodbc::catalog catalog(*m_Connection);
                auto procedures = catalog.find_procedures("");

                while (procedures.next()) {
                    procedureNames.push_back(procedures.procedure_name());
                }
            }
            else if (dbProduct.find("SQL Server") != std::string::npos || dbProduct.find("Microsoft") != std::string::npos) {
                // SQL Server: Use INFORMATION_SCHEMA
                sql = "SELECT ROUTINE_NAME FROM INFORMATION_SCHEMA.ROUTINES "
                      "WHERE ROUTINE_TYPE = 'PROCEDURE' "
                      "ORDER BY ROUTINE_NAME";
                nanodbc::result result = nanodbc::execute(*m_Connection, sql);

                while (result.next()) {
                    procedureNames.push_back(result.get<std::string>(0));
                }
            }
            else {
                // Try ISO SQL Standard for other databases
                sql = "SELECT ROUTINE_NAME FROM INFORMATION_SCHEMA.ROUTINES "
                      "WHERE ROUTINE_TYPE = 'PROCEDURE' "
                      "ORDER BY ROUTINE_NAME";
                nanodbc::result result = nanodbc::execute(*m_Connection, sql);

                while (result.next()) {
                    procedureNames.push_back(result.get<std::string>(0));
                }
            }
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to get stored procedure names: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
        }
        return procedureNames;
    }

    std::vector<std::string> Database::GetAllObjectNames(const std::string& objectType) {
        std::vector<std::string> objectNames;
        try {
            if (objectType.empty() || objectType == "TABLE") {
                auto tables = GetTableNames();
                objectNames.insert(objectNames.end(), tables.begin(), tables.end());
            }
            if (objectType.empty() || objectType == "VIEW") {
                auto views = GetViewNames();
                objectNames.insert(objectNames.end(), views.begin(), views.end());
            }
            if (objectType.empty() || objectType == "PROCEDURE") {
                auto procedures = GetStoredProcedureNames();
                objectNames.insert(objectNames.end(), procedures.begin(), procedures.end());
            }
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to get object names: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
        }
        return objectNames;
    }

    std::vector<Database::DatabaseObjectInfo> Database::GetDatabaseObjects(const std::string& typeFilter) {
        std::vector<DatabaseObjectInfo> objects;
        try {
            std::string dbProduct = GetDatabaseProduct();

            // Check if it's MS Access
            if (dbProduct.find("ACCESS") != std::string::npos || dbProduct.find("Access") != std::string::npos) {
                // MS Access: Use ODBC catalog methods
                nanodbc::catalog catalog(*m_Connection);

                // Get tables
                if (typeFilter.empty() || typeFilter == "TABLE") {
                    auto tables = catalog.find_tables("", "TABLE");
                    while (tables.next()) {
                        DatabaseObjectInfo info;
                        info.Name = tables.table_name();
                        info.Type = "TABLE";
                        info.Schema = tables.table_schema();
                        info.Remarks = tables.table_remarks();
                        objects.push_back(info);
                    }
                }

                // Get views
                if (typeFilter.empty() || typeFilter == "VIEW") {
                    auto views = catalog.find_tables("", "VIEW");
                    while (views.next()) {
                        DatabaseObjectInfo info;
                        info.Name = views.table_name();
                        info.Type = "VIEW";
                        info.Schema = views.table_schema();
                        info.Remarks = views.table_remarks();
                        objects.push_back(info);
                    }
                }

                // Get procedures
                if (typeFilter.empty() || typeFilter == "PROCEDURE") {
                    auto procedures = catalog.find_procedures("");
                    while (procedures.next()) {
                        DatabaseObjectInfo info;
                        info.Name = procedures.procedure_name();
                        info.Type = "PROCEDURE";
                        info.Schema = procedures.procedure_schema();
                        info.Remarks = procedures.procedure_remarks();
                        objects.push_back(info);
                    }
                }
            }
            else {
                // ISO SQL Standard / SQL Server
                std::string sql;

                if (dbProduct.find("SQL Server") != std::string::npos || dbProduct.find("Microsoft") != std::string::npos) {
                    // SQL Server: Get comprehensive object information
                    sql = "SELECT "
                          "  SCHEMA_NAME(o.schema_id) AS SchemaName, "
                          "  o.name AS ObjectName, "
                          "  o.type_desc AS ObjectType, "
                          "  CAST(ep.value AS NVARCHAR(MAX)) AS Remarks "
                          "FROM sys.objects o "
                          "LEFT JOIN sys.extended_properties ep ON ep.major_id = o.object_id "
                          "  AND ep.minor_id = 0 AND ep.name = 'MS_Description' "
                          "WHERE o.type IN ('U', 'V', 'P', 'FN', 'IF', 'TF') ";

                    if (!typeFilter.empty()) {
                        if (typeFilter == "TABLE") sql += "AND o.type = 'U' ";
                        else if (typeFilter == "VIEW") sql += "AND o.type = 'V' ";
                        else if (typeFilter == "PROCEDURE") sql += "AND o.type = 'P' ";
                    }

                    sql += "ORDER BY o.type_desc, o.name";

                    nanodbc::result result = nanodbc::execute(*m_Connection, sql);
                    while (result.next()) {
                        DatabaseObjectInfo info;
                        info.Schema = result.get<std::string>(0, "");
                        info.Name = result.get<std::string>(1, "");
                        info.Type = result.get<std::string>(2, "");
                        info.Remarks = result.get<std::string>(3, "");
                        objects.push_back(info);
                    }

                    // Get view/procedure definitions for SQL Server
                    for (auto& obj : objects) {
                        if (obj.Type == "VIEW" || obj.Type.find("PROCEDURE") != std::string::npos) {
                            try {
                                std::string defSql = "SELECT OBJECT_DEFINITION(OBJECT_ID('" + 
                                                    obj.Schema + "." + obj.Name + "'))";
                                nanodbc::result defResult = nanodbc::execute(*m_Connection, defSql);
                                if (defResult.next() && !defResult.is_null(0)) {
                                    obj.Definition = defResult.get<std::string>(0);
                                }
                            }
                            catch (...) {
                                // Skip if we can't get definition
                            }
                        }
                    }
                }
                else {
                    // Generic ISO SQL approach using INFORMATION_SCHEMA
                    // Get tables
                    if (typeFilter.empty() || typeFilter == "TABLE") {
                        sql = "SELECT TABLE_SCHEMA, TABLE_NAME, 'TABLE' AS TABLE_TYPE "
                              "FROM INFORMATION_SCHEMA.TABLES "
                              "WHERE TABLE_TYPE = 'BASE TABLE' "
                              "ORDER BY TABLE_NAME";
                        nanodbc::result result = nanodbc::execute(*m_Connection, sql);
                        while (result.next()) {
                            DatabaseObjectInfo info;
                            info.Schema = result.get<std::string>(0, "");
                            info.Name = result.get<std::string>(1, "");
                            info.Type = "TABLE";
                            objects.push_back(info);
                        }
                    }

                    // Get views
                    if (typeFilter.empty() || typeFilter == "VIEW") {
                        sql = "SELECT TABLE_SCHEMA, TABLE_NAME, VIEW_DEFINITION "
                              "FROM INFORMATION_SCHEMA.VIEWS "
                              "ORDER BY TABLE_NAME";
                        nanodbc::result result = nanodbc::execute(*m_Connection, sql);
                        while (result.next()) {
                            DatabaseObjectInfo info;
                            info.Schema = result.get<std::string>(0, "");
                            info.Name = result.get<std::string>(1, "");
                            info.Type = "VIEW";
                            info.Definition = result.get<std::string>(2, "");
                            objects.push_back(info);
                        }
                    }

                    // Get procedures
                    if (typeFilter.empty() || typeFilter == "PROCEDURE") {
                        sql = "SELECT ROUTINE_SCHEMA, ROUTINE_NAME, ROUTINE_DEFINITION "
                              "FROM INFORMATION_SCHEMA.ROUTINES "
                              "WHERE ROUTINE_TYPE = 'PROCEDURE' "
                              "ORDER BY ROUTINE_NAME";
                        nanodbc::result result = nanodbc::execute(*m_Connection, sql);
                        while (result.next()) {
                            DatabaseObjectInfo info;
                            info.Schema = result.get<std::string>(0, "");
                            info.Name = result.get<std::string>(1, "");
                            info.Type = "PROCEDURE";
                            info.Definition = result.get<std::string>(2, "");
                            objects.push_back(info);
                        }
                    }
                }
            }

            Logger::GetInstance().Info("Retrieved " + std::to_string(objects.size()) + 
                                     " database objects", "Database");
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to get database objects: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Database");
        }
        return objects;
    }

} // namespace Database
