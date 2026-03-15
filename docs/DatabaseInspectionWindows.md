# Database Inspection ImGui Windows

## Overview
Four new ImGui windows have been created to provide database inspection functionality for tables, views, stored procedures, and a comprehensive object browser.

## New Windows

### 1. Database Object Browser (`RenderDatabaseObjectBrowserWindow`)
**Location:** Tools → Database → Object Browser

**Features:**
- **Unified View**: Browse all database objects (tables, views, procedures) in one window
- **Filter Controls**: 
  - Filter by object type (All, Tables Only, Views Only, Procedures Only)
  - Real-time search/filter by name
  - Refresh button to reload objects
- **Split Panel Layout**:
  - **Left Panel**: Scrollable list of objects with color-coded type indicators
    - Tables: Light blue
    - Views: Yellow-green
    - Procedures: Orange
  - **Right Panel**: Detailed object information
    - Object name, type, schema
    - Remarks/descriptions
    - **"View Table Schema" button** for tables (NEW) - Opens schema viewer
    - Full SQL definition for views and procedures
    - Copy definition button
- **Smart Loading**: Caches objects and only reloads when filter changes

**Schema Viewing (Tables Only):**
- **Columns Tab**: Column names, data types, sizes, nullable, primary keys
- **Indexes Tab**: Index names, columns, unique flags

### 2. Database Tables Window (`RenderDatabaseTablesWindow`)
**Location:** Tools → Database → Tables

**Features:**
- Lists all tables in the current database
- Search/filter tables by name
- Refresh button to reload table list
- **Table schema viewer** (NEW) - View columns, indexes, and foreign keys
- Table actions:
  - **View Schema** - Opens modal popup showing table structure (columns, indexes, foreign keys)
  - View Data (placeholder for future implementation)
  - Details (placeholder for future implementation)
- Shows total table count

**Schema Viewing:**
- **Columns Tab**: Column names, data types, sizes, nullable, primary keys, auto-increment, defaults
- **Indexes Tab**: Index names, columns, unique/primary key/clustered flags
- **Foreign Keys Tab**: FK names, local/referenced columns, ON DELETE/UPDATE actions

### 3. Database Views Window (`RenderDatabaseViewsWindow`)
**Location:** Tools → Database → Views

**Features:**
- Lists all views (queries) in the database
- Search/filter views by name
- Refresh button to reload view list
- **Split Panel Layout**:
  - **Left Panel**: List of view names (selectable)
  - **Right Panel**: View details
    - View name and header
    - **"View Schema"** button - Shows column information (NEW)
    - "Query View Data" button (placeholder)
    - "Copy Name" button
    - **Full SQL Definition** displayed in read-only text box
    - "Copy Definition" button
- Special note for MS Access databases (views are called "queries")
- Shows message if view definitions aren't available for the database type

**Schema Viewing:**
- Displays columns with name, data type, size, and nullable status
- Uses INFORMATION_SCHEMA.COLUMNS for SQL Server and compatible databases

### 4. Database Stored Procedures Window (`RenderDatabaseProceduresWindow`)
**Location:** Tools → Database → Stored Procedures

**Features:**
- Lists all stored procedures in the database
- Search/filter procedures by name
- Refresh button to reload procedure list
- **Split Panel Layout**:
  - **Left Panel**: List of procedure names (selectable)
  - **Right Panel**: Procedure details
    - Procedure name and header
    - **"View Parameters"** button - Shows parameter information (NEW)
    - "Execute" button (placeholder for future implementation)
    - "Copy Name" button
    - Schema information
    - Remarks/description
    - **Full SQL Definition** displayed in read-only text box
    - "Copy Definition" button
- Special note for MS Access databases (limited procedure support)
- Shows message if procedure definitions aren't available

**Parameter Viewing:**
- Displays parameters with name, data type, mode (IN/OUT/INOUT), and max length
- Color-coded parameter modes:
  - IN parameters: Light blue
  - OUT parameters: Orange
  - INOUT parameters: Yellow-green
- Uses INFORMATION_SCHEMA.PARAMETERS for SQL Server and compatible databases

## Technical Implementation

### Connection Handling
All windows check for active database connection:
```cpp
auto activeConn = m_app->GetActiveConnection();
if (!activeConn || !activeConn->IsConnected())
{
    // Show warning message
    return;
}
```

### Database API Integration
Uses the new Database inspection methods:
- `GetTableNames()` - Get list of table names
- `GetViewNames()` - Get list of view names
- `GetStoredProcedureNames()` - Get list of procedure names
- `GetDatabaseObjects(typeFilter)` - Get detailed object information

### Static Variables for State Management
Each window uses static variables to maintain state:
- `static std::vector<std::string> tableNames` - Cached data
- `static bool tablesLoaded` - Loading state flag
- `static int selectedIndex` - Selected item in split views
- `static char searchBuffer[256]` - Search text
- `static int filterType` - Filter selection (Object Browser only)

### Search/Filter Implementation
Real-time case-insensitive search:
```cpp
std::string searchStr = searchBuffer;
std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

for (const auto& item : items)
{
    std::string lower = item;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (searchStr.empty() || lower.find(searchStr) != std::string::npos)
    {
        filteredItems.push_back(item);
    }
}
```

## User Interface Design

### Common UI Elements
All windows share consistent design patterns:

1. **Connection Header**
   ```
   Connected to: [Connection Name] - [Database Name]
   ```

2. **Action Bar**
   - Refresh button
   - Search/filter controls
   - Object count display

3. **Color Coding**
   - Warnings: Yellow-orange `ImVec4(1.0f, 0.7f, 0.2f, 1.0f)`
   - Errors: Red `ImVec4(1.0f, 0.2f, 0.2f, 1.0f)`
   - Info: Light blue `ImVec4(0.4f, 0.8f, 1.0f, 1.0f)`
   - Success: Green `ImVec4(0.2f, 1.0f, 0.2f, 1.0f)`

4. **SQL Definition Display**
   - Dark background `ImVec4(0.1f, 0.1f, 0.1f, 1.0f)`
   - Read-only multiline text box
   - Full window height utilization
   - Copy button above definition

### Window Sizing
- Object Browser: 1000x700 (larger for dual-panel view)
- Tables/Views/Procedures: 900x600
- All use `ImGuiCond_FirstUseEver` for size hints

## Integration with Application

### Window Manager Registration
All windows are registered in `Application::SetupWindows()`:
```cpp
m_windowManager.AddWindow("Tools", "Database", "Object Browser", 
    [this](bool* isOpen) { RenderDatabaseObjectBrowserWindow(isOpen); });
```

### Menu Organization
```
Tools
└── Database
    ├── Database Connection
    ├── Connections Manager
    ├── Object Browser        ← NEW
    ├── Tables                ← NEW
    ├── Views                 ← NEW
    └── Stored Procedures     ← NEW
```

### Delegator Pattern
Application class delegates to WindowFunctions:
```cpp
void Application::RenderDatabaseObjectBrowserWindow(bool* isOpen)
{
    m_windowFunctions->RenderDatabaseObjectBrowserWindow(isOpen);
}
```

## Future Enhancements

### Schema Viewing (IMPLEMENTED)
✅ **Tables**: Full schema viewing with columns, indexes, and foreign keys
✅ **Views**: Column information display (basic implementation)
✅ **Procedures**: Parameter information display

See [DatabaseSchemaViewing.md](DatabaseSchemaViewing.md) for complete documentation.

### Placeholder Implementations
Several buttons are marked with TODO for future implementation:

1. **Tables Window**
   - "View Data" - Execute SELECT * query and display results
   - "View Schema" - Show column definitions, types, constraints
   - "Details" - Show additional table metadata (size, row count, indexes)

2. **Views Window**
   - "Query View Data" - Execute SELECT * FROM view and display results

3. **Procedures Window**
   - "Execute" - Prompt for parameters and execute procedure
   - Parameter input dialog
   - Result set display

### Potential Features
- **Context Menus**: Right-click menu on objects
- **Export**: Export object definitions to SQL files
- **Compare**: Compare objects between databases
- **Dependencies**: Show object dependencies (views using tables, etc.)
- **Modify**: Edit view/procedure definitions (with confirmation)
- **Create**: Create new objects through UI
- **Drop**: Delete objects (with confirmation)
- **Statistics**: Show object statistics and usage information
- **Schema Diagrams**: Visual representation of relationships

## Database Type Compatibility

### Microsoft SQL Server
- ✅ Full support for all features
- ✅ Extended properties (MS_Description) displayed as remarks
- ✅ Complete object definitions via OBJECT_DEFINITION()

### Microsoft Access
- ✅ Tables and Views fully supported
- ⚠️ Limited procedure support (Access uses VBA modules instead)
- ℹ️ Helpful notes displayed in UI about Access limitations

### PostgreSQL, MySQL, Other ISO SQL Databases
- ✅ Tables and Views fully supported via INFORMATION_SCHEMA
- ✅ Procedures supported where available
- ✅ Definitions included where database provides them

## Performance Considerations

### Caching Strategy
- Objects are loaded once and cached in static variables
- Refresh button allows manual reload
- Filter changes trigger reload in Object Browser
- No automatic refresh on timer (prevents unnecessary queries)

### Efficient Queries
- Uses database-specific optimized queries
- Leverages indexes on system tables
- Filters applied at database level when possible

### UI Responsiveness
- Search/filter performed in-memory on cached data
- No database queries during search/typing
- Immediate visual feedback

## Error Handling

### Connection Checks
All windows verify:
1. Active connection exists
2. Connection is still connected
3. Database object is accessible

### Graceful Degradation
- Empty result sets show informative messages
- Missing definitions show appropriate notes
- Database-specific features noted when unavailable

### User Feedback
- Error messages logged to Logger
- Visual warnings in UI
- Clear status messages

## Code Quality

### Consistency
- All windows follow same patterns
- Consistent naming conventions
- Similar UI layouts and behaviors

### Maintainability
- Clear separation of concerns
- Static variables scoped to functions
- Reusable search/filter logic
- Well-commented sections

### Standards Compliance
- C++17 features
- ImGui best practices
- Follows existing codebase patterns
