#include "pch.h"
#include "Tools/Database/Field.h"
#include <sstream>

namespace Database {

    // Convert FieldType to string
    std::string FieldTypeToString(FieldType type) {
        switch (type) {
            case FieldType::Integer: return "Integer";
            case FieldType::BigInt: return "BigInt";
            case FieldType::SmallInt: return "SmallInt";
            case FieldType::TinyInt: return "TinyInt";
            case FieldType::Float: return "Float";
            case FieldType::Double: return "Double";
            case FieldType::Decimal: return "Decimal";
            case FieldType::Numeric: return "Numeric";
            case FieldType::Char: return "Char";
            case FieldType::VarChar: return "VarChar";
            case FieldType::Text: return "Text";
            case FieldType::NChar: return "NChar";
            case FieldType::NVarChar: return "NVarChar";
            case FieldType::NText: return "NText";
            case FieldType::Date: return "Date";
            case FieldType::Time: return "Time";
            case FieldType::DateTime: return "DateTime";
            case FieldType::DateTime2: return "DateTime2";
            case FieldType::Timestamp: return "Timestamp";
            case FieldType::Boolean: return "Boolean";
            case FieldType::Binary: return "Binary";
            case FieldType::VarBinary: return "VarBinary";
            case FieldType::Blob: return "Blob";
            case FieldType::UUID: return "UUID";
            default: return "Unknown";
        }
    }

    // Convert SQL type code to FieldType
    FieldType SQLTypeToFieldType(int sqlType) {
        // SQL type codes from sql.h
        switch (sqlType) {
            case 4: return FieldType::Integer;      // SQL_INTEGER
            case -5: return FieldType::BigInt;      // SQL_BIGINT
            case 5: return FieldType::SmallInt;     // SQL_SMALLINT
            case -6: return FieldType::TinyInt;     // SQL_TINYINT
            case 6: return FieldType::Float;        // SQL_FLOAT
            case 8: return FieldType::Double;       // SQL_DOUBLE
            case 2: return FieldType::Numeric;      // SQL_NUMERIC
            case 3: return FieldType::Decimal;      // SQL_DECIMAL
            case 1: return FieldType::Char;         // SQL_CHAR
            case 12: return FieldType::VarChar;     // SQL_VARCHAR
            case -1: return FieldType::Text;        // SQL_LONGVARCHAR
            case -8: return FieldType::NChar;       // SQL_WCHAR
            case -9: return FieldType::NVarChar;    // SQL_WVARCHAR
            case -10: return FieldType::NText;      // SQL_WLONGVARCHAR
            case 9: return FieldType::Date;         // SQL_TYPE_DATE
            case 10: return FieldType::Time;        // SQL_TYPE_TIME
            case 11: return FieldType::Timestamp;   // SQL_TYPE_TIMESTAMP
            case 93: return FieldType::DateTime;    // SQL_DATETIME
            case -7: return FieldType::Boolean;     // SQL_BIT
            case -2: return FieldType::Binary;      // SQL_BINARY
            case -3: return FieldType::VarBinary;   // SQL_VARBINARY
            case -4: return FieldType::Blob;        // SQL_LONGVARBINARY
            case -11: return FieldType::UUID;       // SQL_GUID
            default: return FieldType::Unknown;
        }
    }

    // Get SQL type string for CREATE TABLE statements
    std::string GetSQLTypeString(FieldType type, int size, int precision, int scale) {
        std::stringstream ss;
        
        switch (type) {
            case FieldType::Integer:
                return "INTEGER";
            case FieldType::BigInt:
                return "BIGINT";
            case FieldType::SmallInt:
                return "SMALLINT";
            case FieldType::TinyInt:
                return "TINYINT";
            case FieldType::Float:
                return "FLOAT";
            case FieldType::Double:
                return "DOUBLE PRECISION";
            case FieldType::Decimal:
                if (precision > 0) {
                    ss << "DECIMAL(" << precision;
                    if (scale > 0) ss << ", " << scale;
                    ss << ")";
                    return ss.str();
                }
                return "DECIMAL";
            case FieldType::Numeric:
                if (precision > 0) {
                    ss << "NUMERIC(" << precision;
                    if (scale > 0) ss << ", " << scale;
                    ss << ")";
                    return ss.str();
                }
                return "NUMERIC";
            case FieldType::Char:
                if (size > 0) {
                    ss << "CHAR(" << size << ")";
                    return ss.str();
                }
                return "CHAR(1)";
            case FieldType::VarChar:
                if (size > 0) {
                    ss << "VARCHAR(" << size << ")";
                    return ss.str();
                }
                return "VARCHAR(255)";
            case FieldType::Text:
                return "TEXT";
            case FieldType::NChar:
                if (size > 0) {
                    ss << "NCHAR(" << size << ")";
                    return ss.str();
                }
                return "NCHAR(1)";
            case FieldType::NVarChar:
                if (size > 0) {
                    ss << "NVARCHAR(" << size << ")";
                    return ss.str();
                }
                return "NVARCHAR(255)";
            case FieldType::NText:
                return "NTEXT";
            case FieldType::Date:
                return "DATE";
            case FieldType::Time:
                return "TIME";
            case FieldType::DateTime:
                return "DATETIME";
            case FieldType::DateTime2:
                return "DATETIME2";
            case FieldType::Timestamp:
                return "TIMESTAMP";
            case FieldType::Boolean:
                return "BIT";
            case FieldType::Binary:
                if (size > 0) {
                    ss << "BINARY(" << size << ")";
                    return ss.str();
                }
                return "BINARY";
            case FieldType::VarBinary:
                if (size > 0) {
                    ss << "VARBINARY(" << size << ")";
                    return ss.str();
                }
                return "VARBINARY(MAX)";
            case FieldType::Blob:
                return "BLOB";
            case FieldType::UUID:
                return "UNIQUEIDENTIFIER";
            default:
                return "VARCHAR(255)";
        }
    }

    // FieldMetadata implementation
    FieldMetadata::FieldMetadata(const std::string& name, FieldType type, int size,
                                 bool nullable, bool primaryKey, bool autoIncrement,
                                 const std::string& defaultValue)
        : m_Name(name), m_Type(type), m_Size(size), m_Nullable(nullable),
          m_PrimaryKey(primaryKey), m_AutoIncrement(autoIncrement),
          m_DefaultValue(defaultValue) {
    }

    std::string FieldMetadata::GenerateColumnDefinition() const {
        std::stringstream ss;
        
        // Column name
        ss << m_Name << " ";
        
        // Data type
        ss << GetSQLTypeString(m_Type, m_Size, m_Precision, m_Scale);
        
        // Primary key
        if (m_PrimaryKey) {
            ss << " PRIMARY KEY";
        }
        
        // Auto increment
        if (m_AutoIncrement) {
            ss << " IDENTITY(1,1)";  // SQL Server syntax, may need adjustment for other databases
        }
        
        // Unique
        if (m_Unique && !m_PrimaryKey) {
            ss << " UNIQUE";
        }
        
        // Nullable
        if (!m_Nullable) {
            ss << " NOT NULL";
        }
        
        // Default value
        if (!m_DefaultValue.empty()) {
            ss << " DEFAULT " << m_DefaultValue;
        }
        
        return ss.str();
    }

    // Field implementation
    Field::Field(const FieldMetadata& metadata)
        : m_Metadata(metadata), m_Value(std::monostate{}) {
    }

    Field::Field(const FieldMetadata& metadata, const FieldValue& value)
        : m_Metadata(metadata), m_Value(value) {
    }

    std::optional<int32_t> Field::GetInt() const {
        if (std::holds_alternative<int32_t>(m_Value)) {
            return std::get<int32_t>(m_Value);
        }
        return std::nullopt;
    }

    std::optional<int64_t> Field::GetBigInt() const {
        if (std::holds_alternative<int64_t>(m_Value)) {
            return std::get<int64_t>(m_Value);
        }
        return std::nullopt;
    }

    std::optional<float> Field::GetFloat() const {
        if (std::holds_alternative<float>(m_Value)) {
            return std::get<float>(m_Value);
        }
        return std::nullopt;
    }

    std::optional<double> Field::GetDouble() const {
        if (std::holds_alternative<double>(m_Value)) {
            return std::get<double>(m_Value);
        }
        return std::nullopt;
    }

    std::optional<std::string> Field::GetString() const {
        if (std::holds_alternative<std::string>(m_Value)) {
            return std::get<std::string>(m_Value);
        }
        return std::nullopt;
    }

    std::optional<std::vector<uint8_t>> Field::GetBinary() const {
        if (std::holds_alternative<std::vector<uint8_t>>(m_Value)) {
            return std::get<std::vector<uint8_t>>(m_Value);
        }
        return std::nullopt;
    }

    std::optional<bool> Field::GetBool() const {
        if (std::holds_alternative<bool>(m_Value)) {
            return std::get<bool>(m_Value);
        }
        return std::nullopt;
    }

    std::string Field::ToString() const {
        if (IsNull()) {
            return "NULL";
        }

        if (auto val = GetInt()) {
            return std::to_string(*val);
        }
        if (auto val = GetBigInt()) {
            return std::to_string(*val);
        }
        if (auto val = GetFloat()) {
            return std::to_string(*val);
        }
        if (auto val = GetDouble()) {
            return std::to_string(*val);
        }
        if (auto val = GetString()) {
            return *val;
        }
        if (auto val = GetBool()) {
            return *val ? "true" : "false";
        }
        if (auto val = GetBinary()) {
            return "[Binary Data]";
        }

        return "[Unknown]";
    }

    // Record implementation
    void Record::AddField(const Field& field) {
        m_Fields.push_back(field);
    }

    void Record::AddField(const std::string& name, const FieldValue& value) {
        FieldMetadata metadata(name, FieldType::Unknown);
        m_Fields.emplace_back(metadata, value);
    }

    Field* Record::GetField(size_t index) {
        if (index < m_Fields.size()) {
            return &m_Fields[index];
        }
        return nullptr;
    }

    const Field* Record::GetField(size_t index) const {
        if (index < m_Fields.size()) {
            return &m_Fields[index];
        }
        return nullptr;
    }

    Field* Record::GetField(const std::string& name) {
        for (auto& field : m_Fields) {
            if (field.GetMetadata().GetName() == name) {
                return &field;
            }
        }
        return nullptr;
    }

    const Field* Record::GetField(const std::string& name) const {
        for (auto& field : m_Fields) {
            if (field.GetMetadata().GetName() == name) {
                return &field;
            }
        }
        return nullptr;
    }

    bool Record::HasField(const std::string& name) const {
        return GetField(name) != nullptr;
    }

    // Recordset implementation
    void Recordset::AddRecord(const Record& record) {
        m_Records.push_back(record);
    }

    Record* Recordset::GetRecord(size_t index) {
        if (index < m_Records.size()) {
            return &m_Records[index];
        }
        return nullptr;
    }

    const Record* Recordset::GetRecord(size_t index) const {
        if (index < m_Records.size()) {
            return &m_Records[index];
        }
        return nullptr;
    }

} // namespace Database
