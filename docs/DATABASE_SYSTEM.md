# Database System Documentation

## Overview

This database system provides a comprehensive ODBC-based abstraction layer for working with various database systems including SQL Server (local and Azure), PostgreSQL, MySQL, and others. The system is built on top of the nanodbc library and provides a clean, object-oriented interface for database operations.

## Architecture

The system is organized into four main classes:

### 1. DatabaseManager
The top-level class that manages database connections and database lifecycle operations.

**Key Features:**
- Connection management (connect, disconnect, test connection)
- Database creation and deletion
- Database switching
- ODBC driver enumeration
- Support for both local and cloud databases (Azure SQL, AWS RDS, etc.)

**Example Usage:**
```cpp
Database::DatabaseManager dbManager;

// Configure connection
Database::ConnectionConfig config;
config.DriverName = "SQL Server";
config.ServerAddress = "localhost";
config.DatabaseName = "MyDatabase";
config.TrustedConnection = true;  // Windows authentication

// Connect
if (dbManager.Connect(config)) {
    std::cout << "Connected to " << dbManager.GetDatabaseProduct() << std::endl;
}

// Create a new database
dbManager.CreateDatabase("NewDatabase");

// Switch to it
dbManager.SwitchDatabase("NewDatabase");
```

### 2. Database
Represents a database instance and manages tables within that database.

**Key Features:**
- Table creation and deletion
- Schema inspection and loading
- Transaction management
- SQL execution
- Database information queries

**Example Usage:**
```cpp
auto db = dbManager.GetDatabase();

// Load existing schema
db->LoadSchema();

// Get all tables
auto tables = db->GetTables();

// Get specific table
auto usersTable = db->GetTable("Users");

// Execute custom SQL
db->ExecuteSQL("CREATE INDEX idx_name ON Users(Name)");
```

### 3. Table
Represents a database table with full schema information and CRUD operations.

**Key Features:**
- Schema definition (fields, indexes, foreign keys)
- Table creation and deletion
- Record insertion (single and batch)
- Record updates and deletes
- Select queries with filtering, ordering, pagination
- Custom SQL execution
- Transaction support
- UUID generation

**Example Usage:**
```cpp
// Create a new table
auto table = db->CreateTable("Users");

// Define schema
table->AddFieldMetadata(Database::FieldMetadata("Id", Database::FieldType::UUID, 0, false, true));
table->AddFieldMetadata(Database::FieldMetadata("Name", Database::FieldType::VarChar, 255, false));
table->AddFieldMetadata(Database::FieldMetadata("Email", Database::FieldType::VarChar, 255, false));
table->AddFieldMetadata(Database::FieldMetadata("Age", Database::FieldType::Integer));
table->AddFieldMetadata(Database::FieldMetadata("CreatedDate", Database::FieldType::DateTime));

// Create the table
table->Create();

// Create an index
table->CreateIndex("idx_email", {"Email"}, true);

// Insert a record
Database::Record record;
record.AddField("Id", table->GenerateUUID());
record.AddField("Name", std::string("John Doe"));
record.AddField("Email", std::string("john@example.com"));
record.AddField("Age", 30);
table->Insert(record);

// Query records
auto recordset = table->Select("Age > 25", "Name ASC", 10, 0);
for (const auto& rec : recordset) {
    auto name = rec.GetField("Name")->GetString();
    std::cout << "Name: " << name.value_or("N/A") << std::endl;
}

// Update records
Database::Record updateRecord;
updateRecord.AddField("Age", 31);
table->Update(updateRecord, "Name = 'John Doe'");

// Delete records
table->Delete("Age < 18");

// Count records
int64_t count = table->Count("Age > 25");
```

### 4. Field, FieldMetadata, Record, and Recordset
Data structures representing database fields, records, and result sets.

**Field Types Supported:**
- Integer, BigInt, SmallInt, TinyInt
- Float, Double, Decimal, Numeric
- Char, VarChar, Text, NChar, NVarChar, NText
- Date, Time, DateTime, DateTime2, Timestamp
- Boolean
- Binary, VarBinary, Blob
- UUID

**Example Usage:**
```cpp
// Define field metadata
Database::FieldMetadata fieldMeta("Price", Database::FieldType::Decimal, 0, false);
fieldMeta.SetPrecision(10);
fieldMeta.SetScale(2);

// Create a field with value
Database::Field field(fieldMeta, 99.99);

// Read field value
if (auto price = field.GetDouble()) {
    std::cout << "Price: $" << *price << std::endl;
}

// Create a record
Database::Record record;
record.AddField(field);

// Create a recordset
Database::Recordset recordset;
recordset.AddRecord(record);
```

## Connection to Azure SQL Database

For Azure SQL Database, use the following connection configuration:

```cpp
Database::ConnectionConfig config;
config.DriverName = "ODBC Driver 17 for SQL Server";  // or "ODBC Driver 18 for SQL Server"
config.ServerAddress = "myserver.database.windows.net";
config.ServerPort = 1433;
config.DatabaseName = "MyDatabase";
config.Username = "sqladmin";
config.Password = "MyPassword123!";
config.Encrypt = true;  // Required for Azure SQL
config.AdditionalOptions = "TrustServerCertificate=No;";

if (dbManager.Connect(config)) {
    std::cout << "Connected to Azure SQL Database" << std::endl;
}
```

## Schema Inspection

The system can inspect existing databases and represent their structure:

```cpp
// Load all tables and their schemas
db->LoadSchema();

// Iterate through tables
for (const auto& [tableName, table] : db->GetTables()) {
    std::cout << "Table: " << tableName << std::endl;
    
    // Iterate through fields
    for (const auto& field : table->GetFields()) {
        std::cout << "  - " << field.GetName() 
                  << " (" << Database::FieldTypeToString(field.GetType()) << ")";
        if (field.IsPrimaryKey()) std::cout << " PRIMARY KEY";
        if (field.IsAutoIncrement()) std::cout << " AUTO_INCREMENT";
        std::cout << std::endl;
    }
    
    // Show indexes
    for (const auto& index : table->GetIndexes()) {
        std::cout << "  Index: " << index.Name << " on (";
        for (size_t i = 0; i < index.Columns.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << index.Columns[i];
        }
        std::cout << ")" << std::endl;
    }
}
```

## Transaction Support

```cpp
// Begin transaction
table->BeginTransaction();

try {
    // Perform multiple operations
    table->Insert(record1);
    table->Insert(record2);
    table->Update(record3, "Id = '...'");
    
    // Commit if all succeed
    table->CommitTransaction();
}
catch (const std::exception& e) {
    // Rollback on error
    table->RollbackTransaction();
}
```

## UUID Support

The system includes UUID generation for distributed systems:

```cpp
std::string uuid = table->GenerateUUID();
// Example: "a1b2c3d4-e5f6-4a7b-8c9d-0e1f2a3b4c5d"
```

## Error Handling

All classes provide error information:

```cpp
if (!table->Insert(record)) {
    std::cerr << "Insert failed: " << table->GetLastError() << std::endl;
}

if (!dbManager.Connect(config)) {
    std::cerr << "Connection failed: " << dbManager.GetLastError() << std::endl;
}
```

## Future Extensions

The current implementation provides the foundation for:

1. **Class Serialization**: Automatic mapping of C++ classes to database tables
2. **Batch Operations**: Optimized bulk load and extract operations
3. **Replication**: Database synchronization across multiple servers
4. **Connection Pooling**: Efficient connection management
5. **Migration System**: Schema versioning and migration
6. **Query Builder**: Fluent API for building complex queries
7. **ORM Features**: Object-relational mapping capabilities

## Prerequisites

### Windows
- ODBC drivers installed (usually pre-installed with Windows)
- For SQL Server: SQL Server Native Client or ODBC Driver 17/18 for SQL Server

### Linux
- unixODBC: `sudo apt-get install unixodbc unixodbc-dev`
- Database-specific drivers (e.g., `sudo apt-get install odbc-postgresql`)

### macOS
- iODBC (usually pre-installed) or unixODBC
- Database-specific drivers

### Azure SQL Database
- ODBC Driver 17 or 18 for SQL Server
- Firewall rules configured to allow your IP address

## Checking Available Drivers

```cpp
auto drivers = Database::DatabaseManager::GetAvailableDrivers();
for (const auto& driver : drivers) {
    std::cout << "Available driver: " << driver << std::endl;
}
```

## Notes

- All database operations are logged using the application's Logger
- The system uses prepared statements for security against SQL injection
- Connection strings are built automatically but can also be provided manually
- The system supports both synchronous and transaction-based operations
- Schema metadata is cached after loading for performance

## Example: Complete Workflow

```cpp
#include "Tools/Database/DatabaseManager.h"

void DatabaseExample() {
    // Create manager
    Database::DatabaseManager dbManager;
    
    // Configure connection
    Database::ConnectionConfig config;
    config.DriverName = "SQL Server";
    config.ServerAddress = "localhost";
    config.DatabaseName = "TestDB";
    config.TrustedConnection = true;
    
    // Connect
    if (!dbManager.Connect(config)) {
        std::cerr << "Failed to connect: " << dbManager.GetLastError() << std::endl;
        return;
    }
    
    // Get database
    auto db = dbManager.GetDatabase();
    
    // Create table
    auto table = db->CreateTable("Products");
    table->AddFieldMetadata(Database::FieldMetadata("Id", Database::FieldType::UUID, 0, false, true));
    table->AddFieldMetadata(Database::FieldMetadata("Name", Database::FieldType::VarChar, 255, false));
    table->AddFieldMetadata(Database::FieldMetadata("Price", Database::FieldType::Decimal, 0, false));
    table->GetField("Price")->SetPrecision(10);
    table->GetField("Price")->SetScale(2);
    
    if (table->Create()) {
        std::cout << "Table created successfully" << std::endl;
        
        // Create index
        table->CreateIndex("idx_name", {"Name"}, false);
        
        // Insert data
        Database::Record record;
        record.AddField("Id", table->GenerateUUID());
        record.AddField("Name", std::string("Widget"));
        record.AddField("Price", 19.99);
        
        if (table->Insert(record)) {
            std::cout << "Record inserted" << std::endl;
        }
        
        // Query data
        auto results = table->Select("", "Name ASC");
        std::cout << "Found " << results.GetRecordCount() << " records" << std::endl;
        
        for (const auto& rec : results) {
            auto name = rec.GetField("Name")->GetString();
            auto price = rec.GetField("Price")->GetDouble();
            std::cout << name.value_or("N/A") << ": $" << price.value_or(0.0) << std::endl;
        }
    }
    
    // Cleanup
    dbManager.Disconnect();
}
```
