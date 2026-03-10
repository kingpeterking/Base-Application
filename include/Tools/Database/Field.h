#pragma once

#include <string>
#include <variant>
#include <optional>
#include <chrono>
#include <vector>

namespace Database {

    // Enumeration for SQL data types
    enum class FieldType {
        Integer,
        BigInt,
        SmallInt,
        TinyInt,
        Float,
        Double,
        Decimal,
        Numeric,
        Char,
        VarChar,
        Text,
        NChar,
        NVarChar,
        NText,
        Date,
        Time,
        DateTime,
        DateTime2,
        Timestamp,
        Boolean,
        Binary,
        VarBinary,
        Blob,
        UUID,
        Unknown
    };

    // Convert FieldType to string
    std::string FieldTypeToString(FieldType type);

    // Convert SQL type code to FieldType
    FieldType SQLTypeToFieldType(int sqlType);

    // Get SQL type string for CREATE TABLE statements
    std::string GetSQLTypeString(FieldType type, int size = 0, int precision = 0, int scale = 0);

    // Field value type - using variant to support multiple types
    using FieldValue = std::variant<
        std::monostate,         // NULL value
        int32_t,                // Integer
        int64_t,                // BigInt
        float,                  // Float
        double,                 // Double
        std::string,            // String types
        std::vector<uint8_t>,   // Binary data
        bool                    // Boolean
    >;

    // Field metadata - describes a column in a table
    class FieldMetadata {
    public:
        FieldMetadata() = default;
        FieldMetadata(const std::string& name, FieldType type, int size = 0,
                     bool nullable = true, bool primaryKey = false,
                     bool autoIncrement = false, const std::string& defaultValue = "");

        // Getters
        const std::string& GetName() const { return m_Name; }
        FieldType GetType() const { return m_Type; }
        int GetSize() const { return m_Size; }
        int GetPrecision() const { return m_Precision; }
        int GetScale() const { return m_Scale; }
        bool IsNullable() const { return m_Nullable; }
        bool IsPrimaryKey() const { return m_PrimaryKey; }
        bool IsAutoIncrement() const { return m_AutoIncrement; }
        bool IsUnique() const { return m_Unique; }
        const std::string& GetDefaultValue() const { return m_DefaultValue; }
        const std::string& GetDescription() const { return m_Description; }

        // Setters
        void SetName(const std::string& name) { m_Name = name; }
        void SetType(FieldType type) { m_Type = type; }
        void SetSize(int size) { m_Size = size; }
        void SetPrecision(int precision) { m_Precision = precision; }
        void SetScale(int scale) { m_Scale = scale; }
        void SetNullable(bool nullable) { m_Nullable = nullable; }
        void SetPrimaryKey(bool primaryKey) { m_PrimaryKey = primaryKey; }
        void SetAutoIncrement(bool autoIncrement) { m_AutoIncrement = autoIncrement; }
        void SetUnique(bool unique) { m_Unique = unique; }
        void SetDefaultValue(const std::string& defaultValue) { m_DefaultValue = defaultValue; }
        void SetDescription(const std::string& description) { m_Description = description; }

        // Generate SQL column definition for CREATE TABLE
        std::string GenerateColumnDefinition() const;

    private:
        std::string m_Name;
        FieldType m_Type = FieldType::Unknown;
        int m_Size = 0;
        int m_Precision = 0;
        int m_Scale = 0;
        bool m_Nullable = true;
        bool m_PrimaryKey = false;
        bool m_AutoIncrement = false;
        bool m_Unique = false;
        std::string m_DefaultValue;
        std::string m_Description;
    };

    // Field instance - represents a field value in a record
    class Field {
    public:
        Field() = default;
        Field(const FieldMetadata& metadata);
        Field(const FieldMetadata& metadata, const FieldValue& value);

        // Getters
        const FieldMetadata& GetMetadata() const { return m_Metadata; }
        const FieldValue& GetValue() const { return m_Value; }
        bool IsNull() const { return std::holds_alternative<std::monostate>(m_Value); }

        // Setters
        void SetValue(const FieldValue& value) { m_Value = value; }
        void SetNull() { m_Value = std::monostate{}; }

        // Type-safe value getters
        std::optional<int32_t> GetInt() const;
        std::optional<int64_t> GetBigInt() const;
        std::optional<float> GetFloat() const;
        std::optional<double> GetDouble() const;
        std::optional<std::string> GetString() const;
        std::optional<std::vector<uint8_t>> GetBinary() const;
        std::optional<bool> GetBool() const;

        // Convert value to string for display
        std::string ToString() const;

    private:
        FieldMetadata m_Metadata;
        FieldValue m_Value;
    };

    // Record - represents a row in a table
    class Record {
    public:
        Record() = default;

        // Add a field to the record
        void AddField(const Field& field);
        void AddField(const std::string& name, const FieldValue& value);

        // Get field by index or name
        Field* GetField(size_t index);
        const Field* GetField(size_t index) const;
        Field* GetField(const std::string& name);
        const Field* GetField(const std::string& name) const;

        // Get field count
        size_t GetFieldCount() const { return m_Fields.size(); }

        // Get all fields
        std::vector<Field>& GetFields() { return m_Fields; }
        const std::vector<Field>& GetFields() const { return m_Fields; }

        // Check if field exists
        bool HasField(const std::string& name) const;

    private:
        std::vector<Field> m_Fields;
    };

    // Recordset - represents a collection of records
    class Recordset {
    public:
        Recordset() = default;

        // Add a record
        void AddRecord(const Record& record);

        // Get record by index
        Record* GetRecord(size_t index);
        const Record* GetRecord(size_t index) const;

        // Get record count
        size_t GetRecordCount() const { return m_Records.size(); }

        // Get all records
        std::vector<Record>& GetRecords() { return m_Records; }
        const std::vector<Record>& GetRecords() const { return m_Records; }

        // Clear all records
        void Clear() { m_Records.clear(); }

        // Iterator support
        std::vector<Record>::iterator begin() { return m_Records.begin(); }
        std::vector<Record>::iterator end() { return m_Records.end(); }
        std::vector<Record>::const_iterator begin() const { return m_Records.begin(); }
        std::vector<Record>::const_iterator end() const { return m_Records.end(); }

    private:
        std::vector<Record> m_Records;
    };

} // namespace Database
