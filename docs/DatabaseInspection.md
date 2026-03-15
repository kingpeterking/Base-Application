# Database Inspection API

## Overview
The Database and DatabaseManager classes now include comprehensive database inspection functionality based on ISO SQL standards, with specialized support for MS Access and MS SQL Server.

## New Methods

### Database Class

#### `std::vector<std::string> GetViewNames()`
Retrieves all view names (queries in MS Access) from the database.
- **MS Access**: Uses ODBC catalog to find views
- **Other databases**: Uses INFORMATION_SCHEMA.VIEWS (ISO SQL standard)

#### `std::vector<std::string> GetStoredProcedureNames()`
Retrieves all stored procedure names from the database.
- **MS Access**: Uses ODBC catalog to find procedures (limited support)
- **SQL Server**: Uses INFORMATION_SCHEMA.ROUTINES with ROUTINE_TYPE='PROCEDURE'
- **Other databases**: Uses ISO SQL standard INFORMATION_SCHEMA.ROUTINES

#### `std::vector<std::string> GetAllObjectNames(const std::string& objectType = "")`
Retrieves all database object names, optionally filtered by type.
- **objectType**: "TABLE", "VIEW", "PROCEDURE", or empty string for all types
- Returns combined list of matching objects

#### `std::vector<DatabaseObjectInfo> GetDatabaseObjects(const std::string& typeFilter = "")`
Retrieves detailed information about database objects.

**DatabaseObjectInfo Structure:**
```cpp
struct DatabaseObjectInfo {
    std::string Name;        // Object name
    std::string Type;        // TABLE, VIEW, PROCEDURE, FUNCTION, etc.
    std::string Schema;      // Schema/owner
    std::string Definition;  // SQL definition (for views and procedures)
    std::string Remarks;     // Comments/description
};
```

**Implementation Details:**
- **MS Access**: 
  - Uses ODBC catalog methods
  - Retrieves tables, views, and procedures with schema and remarks
  
- **SQL Server**:
  - Uses sys.objects and sys.extended_properties
  - Retrieves object definitions using OBJECT_DEFINITION()
  - Includes MS_Description extended properties as remarks
  
- **Other Databases**:
  - Uses INFORMATION_SCHEMA (ISO SQL standard)
  - Retrieves tables, views with definitions, and procedures

### DatabaseManager Class

The DatabaseManager provides the same inspection methods, which delegate to the current database:

- `std::vector<std::string> GetTableNames()`
- `std::vector<std::string> GetViewNames()`
- `std::vector<std::string> GetStoredProcedureNames()`
- `std::vector<Database::DatabaseObjectInfo> GetDatabaseObjects(const std::string& typeFilter = "")`

All methods return empty vectors and set an error if no database is currently selected.

## Usage Examples

### Example 1: List All Tables and Views
```cpp
DatabaseManager dbManager;
if (dbManager.Connect(config)) {
    // Get tables
    auto tables = dbManager.GetTableNames();
    for (const auto& table : tables) {
        std::cout << "Table: " << table << std::endl;
    }
    
    // Get views
    auto views = dbManager.GetViewNames();
    for (const auto& view : views) {
        std::cout << "View: " << view << std::endl;
    }
}
```

### Example 2: Get Detailed Object Information
```cpp
DatabaseManager dbManager;
if (dbManager.Connect(config)) {
    // Get all objects with details
    auto objects = dbManager.GetDatabaseObjects();
    
    for (const auto& obj : objects) {
        std::cout << "Type: " << obj.Type << std::endl;
        std::cout << "Name: " << obj.Name << std::endl;
        std::cout << "Schema: " << obj.Schema << std::endl;
        if (!obj.Definition.empty()) {
            std::cout << "Definition: " << obj.Definition << std::endl;
        }
        if (!obj.Remarks.empty()) {
            std::cout << "Remarks: " << obj.Remarks << std::endl;
        }
        std::cout << "---" << std::endl;
    }
}
```

### Example 3: Filter by Object Type
```cpp
DatabaseManager dbManager;
if (dbManager.Connect(config)) {
    // Get only stored procedures
    auto procedures = dbManager.GetDatabaseObjects("PROCEDURE");
    
    for (const auto& proc : procedures) {
        std::cout << "Procedure: " << proc.Name << std::endl;
        if (!proc.Definition.empty()) {
            std::cout << proc.Definition << std::endl;
        }
    }
}
```

### Example 4: Direct Database Access
```cpp
DatabaseManager dbManager;
if (dbManager.Connect(config)) {
    auto db = dbManager.GetDatabase();
    
    // Get stored procedures
    auto procedures = db->GetStoredProcedureNames();
    
    // Get all objects
    auto allObjects = db->GetAllObjectNames();
}
```

## Database-Specific Notes

### Microsoft Access
- **Views**: Called "Queries" in Access terminology, retrieved as views
- **Stored Procedures**: Limited support; Access uses macros and modules instead
- **Catalog API**: Uses ODBC catalog functions for best compatibility

### Microsoft SQL Server
- **Extended Information**: Retrieves MS_Description extended properties as remarks
- **Definitions**: Full SQL definitions available for views and procedures
- **System Views**: Uses sys.objects for comprehensive object metadata
- **INFORMATION_SCHEMA**: Used as fallback for compatibility

### Other Databases (PostgreSQL, MySQL, etc.)
- **ISO SQL Standard**: Uses INFORMATION_SCHEMA views
- **Portability**: Should work with any ISO SQL compliant database
- **Definitions**: View and procedure definitions included where available

## Error Handling

All methods catch exceptions and:
1. Set the last error message (accessible via `GetLastError()`)
2. Log the error using the Logger
3. Return empty collections on failure

Check `GetLastError()` after calling these methods to diagnose issues:
```cpp
auto views = dbManager.GetViewNames();
if (views.empty()) {
    std::cerr << "Error: " << dbManager.GetLastError() << std::endl;
}
```

## Performance Considerations

- **GetDatabaseObjects()**: This method can be expensive on large databases with many objects
- **Caching**: Consider caching results if you need to access them multiple times
- **Filtering**: Use the typeFilter parameter to reduce query overhead
- **Large Definitions**: View and procedure definitions can be large; they're only retrieved when necessary

## Standards Compliance

The implementation follows:
- **ISO/IEC 9075** (SQL Standard) for INFORMATION_SCHEMA
- **ODBC 3.x** catalog functions for portability
- **SQL Server** system views and functions for extended features
