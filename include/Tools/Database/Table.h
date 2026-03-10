#pragma once

#include "Tools/Database/Field.h"
#include <nanodbc/nanodbc.h>
#include <memory>
#include <vector>
#include <string>
#include <map>

namespace Database {

    // Index metadata
    struct IndexMetadata {
        std::string Name;
        std::vector<std::string> Columns;
        bool IsUnique = false;
        bool IsPrimaryKey = false;
        bool IsClustered = false;
    };

    // Foreign key metadata
    struct ForeignKeyMetadata {
        std::string Name;
        std::string Column;
        std::string ReferencedTable;
        std::string ReferencedColumn;
        std::string OnDelete;  // CASCADE, SET NULL, NO ACTION, etc.
        std::string OnUpdate;
    };

    // Table class - represents a database table
    class Table {
    public:
        Table(const std::string& name, std::shared_ptr<nanodbc::connection> connection);
        ~Table() = default;

        // Basic properties
        const std::string& GetName() const { return m_Name; }
        const std::string& GetSchema() const { return m_Schema; }
        void SetSchema(const std::string& schema) { m_Schema = schema; }

        // Field management
        void AddFieldMetadata(const FieldMetadata& field);
        const std::vector<FieldMetadata>& GetFields() const { return m_Fields; }
        FieldMetadata* GetField(const std::string& name);
        const FieldMetadata* GetField(const std::string& name) const;
        bool HasField(const std::string& name) const;

        // Index management
        void AddIndex(const IndexMetadata& index);
        const std::vector<IndexMetadata>& GetIndexes() const { return m_Indexes; }

        // Foreign key management
        void AddForeignKey(const ForeignKeyMetadata& fk);
        const std::vector<ForeignKeyMetadata>& GetForeignKeys() const { return m_ForeignKeys; }

        // Schema operations
        bool Create();
        bool Drop();
        bool Exists();
        bool Truncate();

        // Load schema from existing table
        bool LoadSchema();

        // Create index
        bool CreateIndex(const std::string& indexName, const std::vector<std::string>& columns, 
                        bool unique = false, bool clustered = false);

        // Create foreign key
        bool CreateForeignKey(const std::string& fkName, const std::string& column,
                             const std::string& refTable, const std::string& refColumn,
                             const std::string& onDelete = "NO ACTION",
                             const std::string& onUpdate = "NO ACTION");

        // Record operations
        bool Insert(const Record& record);
        bool InsertBatch(const std::vector<Record>& records);
        bool Update(const Record& record, const std::string& whereClause);
        bool Delete(const std::string& whereClause);

        // Query operations
        Recordset Select(const std::string& whereClause = "", 
                        const std::string& orderBy = "",
                        int limit = 0, int offset = 0);
        Recordset SelectColumns(const std::vector<std::string>& columns,
                               const std::string& whereClause = "",
                               const std::string& orderBy = "",
                               int limit = 0, int offset = 0);

        // Execute custom SQL
        bool ExecuteSQL(const std::string& sql);
        Recordset ExecuteQuery(const std::string& sql);

        // Count records
        int64_t Count(const std::string& whereClause = "");

        // Transaction support (requires external transaction management)
        bool BeginTransaction();
        bool CommitTransaction();
        bool RollbackTransaction();

        // Get last error
        const std::string& GetLastError() const { return m_LastError; }

        // UUID generation
        std::string GenerateUUID();

    private:
        std::string m_Name;
        std::string m_Schema;
        std::shared_ptr<nanodbc::connection> m_Connection;
        std::unique_ptr<nanodbc::transaction> m_Transaction;
        std::vector<FieldMetadata> m_Fields;
        std::vector<IndexMetadata> m_Indexes;
        std::vector<ForeignKeyMetadata> m_ForeignKeys;
        std::string m_LastError;

        // Helper methods
        std::string GetFullTableName() const;
        std::string BuildInsertSQL(const Record& record);
        std::string BuildUpdateSQL(const Record& record, const std::string& whereClause);
        std::string BuildSelectSQL(const std::vector<std::string>& columns,
                                  const std::string& whereClause,
                                  const std::string& orderBy,
                                  int limit, int offset);
        Recordset ConvertResultToRecordset(nanodbc::result& result);
        void SetError(const std::string& error);
    };

} // namespace Database
