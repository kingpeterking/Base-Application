# Database Schema Viewing

## Overview
Schema viewing capabilities have been added to all database object browser windows, allowing users to inspect the structure and metadata of tables, views, and stored procedures.

## Features by Window

### 1. Database Tables Window
**Location:** Tools → Database → Tables

#### View Schema Button
Clicking "View Schema" on any table opens a modal popup window displaying comprehensive table metadata.

**Schema Information Displayed:**

##### Columns Tab
- **Column Name**: The name of each column
- **Data Type**: SQL data type (VARCHAR, INT, DATETIME, etc.)
- **Size**: Maximum length for character types
- **Nullable**: Whether the column allows NULL values
- **PK (Primary Key)**: Highlighted in yellow if part of primary key
- **Auto Inc**: Whether the column auto-increments
- **Default**: Default value for the column

##### Indexes Tab
- **Index Name**: Name of the index
- **Columns**: Comma-separated list of columns in the index
- **Unique**: Whether the index enforces uniqueness
- **Primary Key**: Highlighted in yellow if it's the primary key index
- **Clustered**: Whether it's a clustered index (SQL Server)

##### Foreign Keys Tab
- **FK Name**: Name of the foreign key constraint
- **Column**: Local column name
- **Referenced Table**: Table being referenced
- **Referenced Column**: Column in the referenced table
- **On Delete**: Action on delete (CASCADE, SET NULL, NO ACTION, etc.)
- **On Update**: Action on update

**How to Use:**
1. Open **Tools → Database → Tables**
2. Find the table you want to inspect
3. Click the **"View Schema"** button for that table
4. Browse the three tabs (Columns, Indexes, Foreign Keys)
5. Click **"Close"** to return to the tables list

### 2. Database Object Browser
**Location:** Tools → Database → Object Browser

#### View Table Schema Button
When a **table** is selected in the Object Browser, a "View Table Schema" button appears in the details panel.

**Schema Information:**
- **Columns Tab**: Column names, types, sizes, nullable status, primary keys
- **Indexes Tab**: Index names, columns, uniqueness

**How to Use:**
1. Open **Tools → Database → Object Browser**
2. Select a table from the object list
3. Click **"View Table Schema"** in the details panel
4. Review the schema information
5. Click **"Close"** when done

### 3. Database Views Window
**Location:** Tools → Database → Views

#### View Schema Button
Displays column information for the selected view.

**Schema Information:**
- **Column Name**: Name of each column in the view
- **Data Type**: SQL data type of the column
- **Size**: Maximum length for character types
- **Nullable**: Whether the column allows NULL values

**Current Status:**
- Button displays "View Schema"
- Queries `INFORMATION_SCHEMA.COLUMNS` for SQL Server and compatible databases
- For MS Access, uses alternative method
- Currently shows a message that full implementation is pending
- Users can still view the SQL definition to understand the view structure

**How to Use:**
1. Open **Tools → Database → Views**
2. Select a view from the list
3. Click **"View Schema"** button
4. View column information in the popup
5. Click **"Close"** to return

### 4. Database Stored Procedures Window
**Location:** Tools → Database → Stored Procedures

#### View Parameters Button
Displays parameter information for the selected stored procedure.

**Parameter Information:**
- **Parameter Name**: Name of the parameter
- **Data Type**: SQL data type
- **Mode**: 
  - **IN** (Input parameter) - displayed in light blue
  - **OUT** (Output parameter) - displayed in orange
  - **INOUT** (Input/Output parameter) - displayed in yellow-green
- **Max Length**: Maximum length for character parameters

**How to Use:**
1. Open **Tools → Database → Stored Procedures**
2. Select a procedure from the list
3. Click **"View Parameters"** button
4. Review parameter information
5. Click **"Close"** to return

**Database Compatibility:**
- **SQL Server**: Full parameter information via `INFORMATION_SCHEMA.PARAMETERS`
- **Other Databases**: May have limited or no parameter metadata depending on system catalog support

## Implementation Details

### Table Schema Loading
```cpp
// Create Table object and load schema
auto table = db->GetTable(tableName);
if (table && table->LoadSchema())
{
    // Access metadata
    const auto& fields = table->GetFields();
    const auto& indexes = table->GetIndexes();
    const auto& foreignKeys = table->GetForeignKeys();
}
```

The `LoadSchema()` method queries:
- **Column information** via `INFORMATION_SCHEMA.COLUMNS` or ODBC catalog
- **Index information** via `sys.indexes` (SQL Server) or database-specific system tables
- **Foreign key information** via `sys.foreign_keys` (SQL Server) or database-specific system tables

### View Schema Loading
```cpp
std::string sql = "SELECT COLUMN_NAME, DATA_TYPE, CHARACTER_MAXIMUM_LENGTH, IS_NULLABLE "
                  "FROM INFORMATION_SCHEMA.COLUMNS "
                  "WHERE TABLE_NAME = '" + viewName + "' "
                  "ORDER BY ORDINAL_POSITION";
```

### Procedure Parameters Loading
```cpp
std::string sql = "SELECT PARAMETER_NAME, DATA_TYPE, PARAMETER_MODE, CHARACTER_MAXIMUM_LENGTH "
                  "FROM INFORMATION_SCHEMA.PARAMETERS "
                  "WHERE SPECIFIC_NAME = '" + procedureName + "' "
                  "ORDER BY ORDINAL_POSITION";
```

## User Interface Design

### Modal Popups
All schema viewing uses modal popup windows that:
- Center on screen when opened
- Have fixed size (can be resized)
- Block interaction with parent window
- Close with "Close" button or ESC key

### Tab-Based Navigation
Table schema uses tabs to organize information:
- **Columns**: Most frequently needed information
- **Indexes**: Performance-related metadata
- **Foreign Keys**: Relationship information

### Color Coding
- **Primary Keys**: Yellow highlight (`ImVec4(1.0f, 0.8f, 0.0f, 1.0f)`)
- **IN Parameters**: Light blue (`ImVec4(0.4f, 0.8f, 1.0f, 1.0f)`)
- **OUT Parameters**: Orange (`ImVec4(1.0f, 0.7f, 0.3f, 1.0f)`)
- **INOUT Parameters**: Yellow-green (`ImVec4(0.8f, 1.0f, 0.4f, 1.0f)`)

### Table Layout
All schema information uses ImGui tables with:
- **Borders**: For clear separation
- **Row Background**: Alternating colors for readability
- **Resizable Columns**: User can adjust widths
- **Scrollable**: For large result sets

## Database Compatibility

### Microsoft SQL Server
✅ **Full Support**
- Complete column metadata including defaults
- Full index information including clustered/non-clustered
- Foreign key constraints with actions
- Stored procedure parameters with modes
- Extended properties for descriptions

### PostgreSQL
✅ **Good Support**
- Column metadata via `INFORMATION_SCHEMA`
- Index information via system catalogs
- Foreign key constraints
- Procedure parameters (limited)

### MySQL
✅ **Good Support**
- Column metadata via `INFORMATION_SCHEMA`
- Index information via `INFORMATION_SCHEMA.STATISTICS`
- Foreign key constraints via `INFORMATION_SCHEMA.KEY_COLUMN_USAGE`

### Microsoft Access
⚠️ **Limited Support**
- Column metadata via ODBC catalog functions
- Basic index information
- Limited foreign key support
- No stored procedure parameters (Access uses VBA modules)
- **Note**: Uses alternative methods when `INFORMATION_SCHEMA` is not available

### SQLite
⚠️ **Limited Support**
- Column metadata via `PRAGMA table_info()`
- Basic index information via `PRAGMA index_list()`
- No foreign key metadata in older versions

## Error Handling

### Schema Loading Failures
If schema loading fails:
- Error is logged via Logger
- Popup displays error message: "Failed to load table schema"
- User can close popup and try again

### Missing Information
When information is not available:
- Displays "-" for empty values
- Shows warning message for unsupported features
- Provides helpful hints about database limitations

### Query Failures
If `INFORMATION_SCHEMA` queries fail:
- Catches exception and logs error
- Displays user-friendly message
- Suggests checking database permissions or driver support

## Performance Considerations

### Caching
- Table schema is cached while popup is open
- Schema is reloaded only when popup is closed and reopened
- Parameter information is cached per procedure
- View column information is cached per view

### Lazy Loading
- Schema is loaded only when "View Schema" button is clicked
- Not loaded during initial window display
- Reduces memory usage for databases with many objects

### Query Optimization
- Uses indexed system tables/views when available
- Filters by object name to reduce result set
- Orders results by position for correct column order

## Future Enhancements

### Planned Features
1. **Table Statistics**
   - Row count
   - Table size
   - Index size
   - Last modified date

2. **Column Statistics**
   - Distinct values count
   - NULL percentage
   - Min/Max values

3. **Dependency Viewer**
   - Show tables that reference this table (via foreign keys)
   - Show views that use this table
   - Show procedures that access this table

4. **Export Schema**
   - Export to SQL CREATE script
   - Export to CSV
   - Export to JSON

5. **Compare Schemas**
   - Compare two tables
   - Highlight differences
   - Generate ALTER script

6. **View Data**
   - Quick preview of table/view data
   - Integrated with schema viewer
   - Sortable and filterable

7. **Execute Procedure**
   - Parameter input dialog
   - Use parameter metadata to validate inputs
   - Display results

## Troubleshooting

### "Failed to load table schema"
**Causes:**
- Database connection lost
- Insufficient permissions to query system tables
- Table no longer exists
- ODBC driver limitations

**Solutions:**
- Check database connection status
- Verify user has SELECT permission on system tables
- Refresh the tables list
- Try reconnecting to database

### "No column information available"
**Causes:**
- Database type doesn't support `INFORMATION_SCHEMA`
- View definition is complex or encrypted
- ODBC driver doesn't support catalog functions

**Solutions:**
- View the SQL definition manually
- Use database-specific tools
- Check ODBC driver documentation

### "No parameters found"
**Causes:**
- Procedure genuinely has no parameters
- Database doesn't support `INFORMATION_SCHEMA.PARAMETERS`
- MS Access procedure (uses VBA instead)

**Solutions:**
- Check procedure definition
- Verify database type supports parameter metadata
- Use database-specific tools for MS Access

## Best Practices

### When to Use Schema Viewing
- **Before writing queries**: Understand column names and types
- **Debugging**: Verify table structure matches expectations
- **Documentation**: Export schema information for reference
- **Performance tuning**: Review indexes
- **Data integrity**: Check foreign key constraints

### Tips
1. Use **Object Browser** for quick overview of multiple object types
2. Use **Tables Window** for detailed table analysis
3. Check **Indexes** tab to understand query performance
4. Review **Foreign Keys** to understand relationships
5. Use **View Parameters** before executing stored procedures

### Security
- Schema viewing requires SELECT permission on system tables
- Some databases restrict access to certain metadata
- Extended properties may not be visible to all users
- Be aware of information disclosure when sharing schema details

## Examples

### Example 1: Find Primary Key Columns
1. Open Tables window
2. Click "View Schema" on your table
3. Look for yellow-highlighted "YES" in the PK column

### Example 2: Check Foreign Key Relationships
1. Open Tables window
2. Click "View Schema" on your table
3. Go to "Foreign Keys" tab
4. Review referenced tables and actions

### Example 3: Understand Procedure Parameters
1. Open Stored Procedures window
2. Select your procedure
3. Click "View Parameters"
4. Check parameter modes (IN/OUT/INOUT) and data types

### Example 4: Browse All Database Objects
1. Open Object Browser
2. Select "All Objects" filter
3. Click on any table
4. Click "View Table Schema" for details
