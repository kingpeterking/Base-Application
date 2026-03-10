#include "pch.h"
#include "Tools/Database/Table.h"
#include "Tools/Logger.h"
#include <sstream>
#include <random>

namespace Database {

    Table::Table(const std::string& name, std::shared_ptr<nanodbc::connection> connection)
        : m_Name(name), m_Connection(connection), m_Schema("dbo") {
    }

    std::string Table::GetFullTableName() const {
        if (!m_Schema.empty()) {
            return m_Schema + "." + m_Name;
        }
        return m_Name;
    }

    void Table::AddFieldMetadata(const FieldMetadata& field) {
        m_Fields.push_back(field);
    }

    FieldMetadata* Table::GetField(const std::string& name) {
        for (auto& field : m_Fields) {
            if (field.GetName() == name) {
                return &field;
            }
        }
        return nullptr;
    }

    const FieldMetadata* Table::GetField(const std::string& name) const {
        for (auto& field : m_Fields) {
            if (field.GetName() == name) {
                return &field;
            }
        }
        return nullptr;
    }

    bool Table::HasField(const std::string& name) const {
        return GetField(name) != nullptr;
    }

    void Table::AddIndex(const IndexMetadata& index) {
        m_Indexes.push_back(index);
    }

    void Table::AddForeignKey(const ForeignKeyMetadata& fk) {
        m_ForeignKeys.push_back(fk);
    }

    bool Table::Create() {
        try {
            if (m_Fields.empty()) {
                SetError("Cannot create table: No fields defined");
                return false;
            }

            std::stringstream ss;
            ss << "CREATE TABLE " << GetFullTableName() << " (";

            // Add columns
            for (size_t i = 0; i < m_Fields.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << m_Fields[i].GenerateColumnDefinition();
            }

            ss << ")";

            nanodbc::execute(*m_Connection, ss.str());
            Logger::GetInstance().Info("Table created: " + GetFullTableName());
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to create table: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::Drop() {
        try {
            std::string sql = "DROP TABLE " + GetFullTableName();
            nanodbc::execute(*m_Connection, sql);
            Logger::GetInstance().Info("Table dropped: " + GetFullTableName());
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to drop table: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::Exists() {
        try {
            // Try to query the table
            std::string sql = "SELECT TOP 1 * FROM " + GetFullTableName();
            nanodbc::result result = nanodbc::execute(*m_Connection, sql);
            return true;
        }
        catch (const std::exception&) {
            return false;
        }
    }

    bool Table::Truncate() {
        try {
            std::string sql = "TRUNCATE TABLE " + GetFullTableName();
            nanodbc::execute(*m_Connection, sql);
            Logger::GetInstance().Info("Table truncated: " + GetFullTableName());
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to truncate table: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::LoadSchema() {
        try {
            m_Fields.clear();
            m_Indexes.clear();
            m_ForeignKeys.clear();

            // Get column information
            nanodbc::catalog catalog(*m_Connection);
            auto columns = catalog.find_columns(m_Name, "");

            while (columns.next()) {
                std::string columnName = columns.column_name();
                int dataType = columns.data_type();
                int columnSize = columns.column_size();
                bool nullable = (columns.nullable() == SQL_NULLABLE);

                FieldType fieldType = SQLTypeToFieldType(dataType);
                FieldMetadata field(columnName, fieldType, columnSize, nullable);

                // Set precision and scale for numeric types
                if (fieldType == FieldType::Decimal || fieldType == FieldType::Numeric) {
                    field.SetPrecision(columnSize);
                    field.SetScale(columns.decimal_digits());
                }

                m_Fields.push_back(field);
            }

            // Get primary key information
            auto primaryKeys = catalog.find_primary_keys(m_Name);
            while (primaryKeys.next()) {
                std::string columnName = primaryKeys.column_name();
                auto field = GetField(columnName);
                if (field) {
                    field->SetPrimaryKey(true);
                }
            }

            Logger::GetInstance().Info("Schema loaded for table: " + GetFullTableName() + 
                       " (" + std::to_string(m_Fields.size()) + " columns)");
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to load schema: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::CreateIndex(const std::string& indexName, const std::vector<std::string>& columns,
                           bool unique, bool clustered) {
        try {
            std::stringstream ss;
            ss << "CREATE ";
            if (unique) ss << "UNIQUE ";
            if (clustered) ss << "CLUSTERED ";
            ss << "INDEX " << indexName << " ON " << GetFullTableName() << " (";

            for (size_t i = 0; i < columns.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << columns[i];
            }
            ss << ")";

            nanodbc::execute(*m_Connection, ss.str());

            IndexMetadata index;
            index.Name = indexName;
            index.Columns = columns;
            index.IsUnique = unique;
            index.IsClustered = clustered;
            m_Indexes.push_back(index);

            Logger::GetInstance().Info("Index created: " + indexName + " on " + GetFullTableName());
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to create index: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::CreateForeignKey(const std::string& fkName, const std::string& column,
                                 const std::string& refTable, const std::string& refColumn,
                                 const std::string& onDelete, const std::string& onUpdate) {
        try {
            std::stringstream ss;
            ss << "ALTER TABLE " << GetFullTableName()
               << " ADD CONSTRAINT " << fkName
               << " FOREIGN KEY (" << column << ")"
               << " REFERENCES " << refTable << "(" << refColumn << ")";

            if (!onDelete.empty() && onDelete != "NO ACTION") {
                ss << " ON DELETE " << onDelete;
            }
            if (!onUpdate.empty() && onUpdate != "NO ACTION") {
                ss << " ON UPDATE " << onUpdate;
            }

            nanodbc::execute(*m_Connection, ss.str());

            ForeignKeyMetadata fk;
            fk.Name = fkName;
            fk.Column = column;
            fk.ReferencedTable = refTable;
            fk.ReferencedColumn = refColumn;
            fk.OnDelete = onDelete;
            fk.OnUpdate = onUpdate;
            m_ForeignKeys.push_back(fk);

            Logger::GetInstance().Info("Foreign key created: " + fkName + " on " + GetFullTableName());
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to create foreign key: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    std::string Table::BuildInsertSQL(const Record& record) {
        std::stringstream ss;
        ss << "INSERT INTO " << GetFullTableName() << " (";

        // Column names
        const auto& fields = record.GetFields();
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << fields[i].GetMetadata().GetName();
        }

        ss << ") VALUES (";

        // Values
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "?";
        }

        ss << ")";
        return ss.str();
    }

    bool Table::Insert(const Record& record) {
        try {
            std::string sql = BuildInsertSQL(record);
            nanodbc::statement stmt(*m_Connection);
            stmt.prepare(sql);

            // Bind parameters - store values to keep them alive
            const auto& fields = record.GetFields();
            std::vector<int32_t> intVals;
            std::vector<int64_t> bigIntVals;
            std::vector<float> floatVals;
            std::vector<double> doubleVals;
            std::vector<bool> boolVals;
            std::vector<std::string> stringVals;

            for (size_t i = 0; i < fields.size(); ++i) {
                const auto& field = fields[i];
                short paramIndex = static_cast<short>(i);

                if (field.IsNull()) {
                    stmt.bind_null(paramIndex);
                }
                else if (auto val = field.GetInt()) {
                    intVals.push_back(val.value());
                    stmt.bind(paramIndex, &intVals.back());
                }
                else if (auto val = field.GetBigInt()) {
                    bigIntVals.push_back(val.value());
                    stmt.bind(paramIndex, &bigIntVals.back());
                }
                else if (auto val = field.GetFloat()) {
                    floatVals.push_back(val.value());
                    stmt.bind(paramIndex, &floatVals.back());
                }
                else if (auto val = field.GetDouble()) {
                    doubleVals.push_back(val.value());
                    stmt.bind(paramIndex, &doubleVals.back());
                }
                else if (auto val = field.GetString()) {
                    stringVals.push_back(val.value());
                    stmt.bind(paramIndex, stringVals.back().c_str());
                }
                else if (auto val = field.GetBool()) {
                    // Convert bool to int for nanodbc`n                    intVals.push_back(val.value() ? 1 : 0);`n                    stmt.bind(paramIndex, &intVals.back());
                }
            }

            stmt.execute();
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to insert record: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::InsertBatch(const std::vector<Record>& records) {
        if (records.empty()) return true;

        try {
            std::string sql = BuildInsertSQL(records[0]);
            nanodbc::statement stmt(*m_Connection);
            stmt.prepare(sql);

            std::vector<int32_t> intVals;
            std::vector<int64_t> bigIntVals;
            std::vector<float> floatVals;
            std::vector<double> doubleVals;
            std::vector<bool> boolVals;
            std::vector<std::string> stringVals;

            for (const auto& record : records) {
                intVals.clear();
                bigIntVals.clear();
                floatVals.clear();
                doubleVals.clear();
                boolVals.clear();
                stringVals.clear();

                const auto& fields = record.GetFields();
                for (size_t i = 0; i < fields.size(); ++i) {
                    const auto& field = fields[i];
                    short paramIndex = static_cast<short>(i);

                    if (field.IsNull()) {
                        stmt.bind_null(paramIndex);
                    }
                    else if (auto val = field.GetInt()) {
                        intVals.push_back(val.value());
                        stmt.bind(paramIndex, &intVals.back());
                    }
                    else if (auto val = field.GetBigInt()) {
                        bigIntVals.push_back(val.value());
                        stmt.bind(paramIndex, &bigIntVals.back());
                    }
                    else if (auto val = field.GetFloat()) {
                        floatVals.push_back(val.value());
                        stmt.bind(paramIndex, &floatVals.back());
                    }
                    else if (auto val = field.GetDouble()) {
                        doubleVals.push_back(val.value());
                        stmt.bind(paramIndex, &doubleVals.back());
                    }
                    else if (auto val = field.GetString()) {
                        stringVals.push_back(val.value());
                        stmt.bind(paramIndex, stringVals.back().c_str());
                    }
                    else if (auto val = field.GetBool()) {
                        // Convert bool to int for nanodbc`n                    intVals.push_back(val.value() ? 1 : 0);`n                    stmt.bind(paramIndex, &intVals.back());
                    }
                }
                stmt.execute();
            }

            Logger::GetInstance().Info("Batch insert: " + std::to_string(records.size()) + 
                       " records into " + GetFullTableName(), "Table");
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to insert batch: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    std::string Table::BuildUpdateSQL(const Record& record, const std::string& whereClause) {
        std::stringstream ss;
        ss << "UPDATE " << GetFullTableName() << " SET ";

        const auto& fields = record.GetFields();
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << fields[i].GetMetadata().GetName() << " = ?";
        }

        if (!whereClause.empty()) {
            ss << " WHERE " << whereClause;
        }

        return ss.str();
    }

    bool Table::Update(const Record& record, const std::string& whereClause) {
        try {
            std::string sql = BuildUpdateSQL(record, whereClause);
            nanodbc::statement stmt(*m_Connection);
            stmt.prepare(sql);

            // Bind parameters - store values to keep them alive
            const auto& fields = record.GetFields();
            std::vector<int32_t> intVals;
            std::vector<int64_t> bigIntVals;
            std::vector<float> floatVals;
            std::vector<double> doubleVals;
            std::vector<bool> boolVals;
            std::vector<std::string> stringVals;

            for (size_t i = 0; i < fields.size(); ++i) {
                const auto& field = fields[i];
                short paramIndex = static_cast<short>(i);

                if (field.IsNull()) {
                    stmt.bind_null(paramIndex);
                }
                else if (auto val = field.GetInt()) {
                    intVals.push_back(val.value());
                    stmt.bind(paramIndex, &intVals.back());
                }
                else if (auto val = field.GetBigInt()) {
                    bigIntVals.push_back(val.value());
                    stmt.bind(paramIndex, &bigIntVals.back());
                }
                else if (auto val = field.GetFloat()) {
                    floatVals.push_back(val.value());
                    stmt.bind(paramIndex, &floatVals.back());
                }
                else if (auto val = field.GetDouble()) {
                    doubleVals.push_back(val.value());
                    stmt.bind(paramIndex, &doubleVals.back());
                }
                else if (auto val = field.GetString()) {
                    stringVals.push_back(val.value());
                    stmt.bind(paramIndex, stringVals.back().c_str());
                }
                else if (auto val = field.GetBool()) {
                    // Convert bool to int for nanodbc`n                    intVals.push_back(val.value() ? 1 : 0);`n                    stmt.bind(paramIndex, &intVals.back());
                }
            }

            stmt.execute();
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to update record: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::Delete(const std::string& whereClause) {
        try {
            std::stringstream ss;
            ss << "DELETE FROM " << GetFullTableName();
            if (!whereClause.empty()) {
                ss << " WHERE " << whereClause;
            }

            nanodbc::execute(*m_Connection, ss.str());
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to delete records: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    std::string Table::BuildSelectSQL(const std::vector<std::string>& columns,
                                     const std::string& whereClause,
                                     const std::string& orderBy,
                                     int limit, int offset) {
        std::stringstream ss;
        ss << "SELECT ";

        if (columns.empty()) {
            ss << "*";
        }
        else {
            for (size_t i = 0; i < columns.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << columns[i];
            }
        }

        ss << " FROM " << GetFullTableName();

        if (!whereClause.empty()) {
            ss << " WHERE " << whereClause;
        }

        if (!orderBy.empty()) {
            ss << " ORDER BY " << orderBy;
        }

        if (limit > 0) {
            if (offset > 0) {
                ss << " OFFSET " << offset << " ROWS FETCH NEXT " << limit << " ROWS ONLY";
            }
            else {
                ss << " OFFSET 0 ROWS FETCH NEXT " << limit << " ROWS ONLY";
            }
        }

        return ss.str();
    }

    Recordset Table::ConvertResultToRecordset(nanodbc::result& result) {
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

    Recordset Table::Select(const std::string& whereClause, const std::string& orderBy,
                           int limit, int offset) {
        return SelectColumns({}, whereClause, orderBy, limit, offset);
    }

    Recordset Table::SelectColumns(const std::vector<std::string>& columns,
                                   const std::string& whereClause,
                                   const std::string& orderBy,
                                   int limit, int offset) {
        try {
            std::string sql = BuildSelectSQL(columns, whereClause, orderBy, limit, offset);
            nanodbc::result result = nanodbc::execute(*m_Connection, sql);
            return ConvertResultToRecordset(result);
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to select records: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return Recordset();
        }
    }

    bool Table::ExecuteSQL(const std::string& sql) {
        try {
            nanodbc::execute(*m_Connection, sql);
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to execute SQL: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    Recordset Table::ExecuteQuery(const std::string& sql) {
        try {
            nanodbc::result result = nanodbc::execute(*m_Connection, sql);
            return ConvertResultToRecordset(result);
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to execute query: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return Recordset();
        }
    }

    int64_t Table::Count(const std::string& whereClause) {
        try {
            std::stringstream ss;
            ss << "SELECT COUNT(*) FROM " << GetFullTableName();
            if (!whereClause.empty()) {
                ss << " WHERE " << whereClause;
            }

            nanodbc::result result = nanodbc::execute(*m_Connection, ss.str());
            if (result.next()) {
                return result.get<int64_t>(0);
            }
            return 0;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to count records: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return -1;
        }
    }

    bool Table::BeginTransaction() {
        try {
            // Create a transaction object
            m_Transaction = std::make_unique<nanodbc::transaction>(*m_Connection);
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to begin transaction: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::CommitTransaction() {
        try {
            if (m_Transaction) {
                m_Transaction->commit();
                m_Transaction.reset();
            }
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to commit transaction: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    bool Table::RollbackTransaction() {
        try {
            if (m_Transaction) {
                m_Transaction->rollback();
                m_Transaction.reset();
            }
            return true;
        }
        catch (const std::exception& e) {
            SetError(std::string("Failed to rollback transaction: ") + e.what());
            Logger::GetInstance().Error(m_LastError, "Table");
            return false;
        }
    }

    std::string Table::GenerateUUID() {
        // Generate a UUID (Version 4 - Random)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::uniform_int_distribution<> dis2(8, 11);

        std::stringstream ss;
        ss << std::hex;

        for (int i = 0; i < 8; ++i) ss << dis(gen);
        ss << "-";
        for (int i = 0; i < 4; ++i) ss << dis(gen);
        ss << "-4";
        for (int i = 0; i < 3; ++i) ss << dis(gen);
        ss << "-";
        ss << dis2(gen);
        for (int i = 0; i < 3; ++i) ss << dis(gen);
        ss << "-";
        for (int i = 0; i < 12; ++i) ss << dis(gen);

        return ss.str();
    }

    void Table::SetError(const std::string& error) {
        m_LastError = error;
    }

} // namespace Database
