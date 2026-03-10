# MS Access Database Support

## Overview
The Database Connection Window now includes comprehensive support for Microsoft Access databases (.mdb and .accdb files).

## What Was Added

### 1. Driver Support
- Added "Microsoft Access Driver (*.mdb, *.accdb)" to the common drivers list in the Connection Form
- Automatically detected through ODBC driver enumeration

### 2. Quick Connect Preset
A dedicated quick connect button for MS Access databases:
- **Button**: "MS Access Database"
- **Pre-configured Settings**:
  - Driver: Microsoft Access Driver (*.mdb, *.accdb)
  - Server field: `C:\path\to\database.accdb` (template path)
  - Port: Empty (not used)
  - Database: Empty (not used)
  - Trusted Connection: Disabled
  - Encryption: Disabled

### 3. UI Enhancements
**Context-Sensitive Hints:**
When an Access driver is selected, the window displays helpful tips:
- 💡 Tip displayed below the Server/Port fields
- Explains that Server field should contain the full file path
- Clarifies that Port and Database fields should be left empty

### 4. Connection String Examples
Added two MS Access examples in the Connection String tab:

**Without Password:**
```
Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\MyDatabase.accdb;
```

**With Password:**
```
Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\MyDatabase.accdb;PWD=mypassword;
```

## How to Connect to MS Access

### Method 1: Quick Connect (Fastest)
1. Open: `Tools > Database > Database Connection`
2. Switch to "Quick Connect" tab
3. Click "MS Access Database" button
4. Switch to "Connection Form" tab
5. Replace the Server field value with your actual database path
   - Example: `C:\Users\YourName\Documents\Sales.accdb`
   - Example: `D:\Projects\Inventory.mdb`
6. If database has a password:
   - Uncheck "Trusted Connection"
   - Enter password in Password field
   - Leave Username empty
7. Click "Test Connection" or "Connect"

### Method 2: Connection Form (Full Control)
1. Open: `Tools > Database > Database Connection`
2. Go to "Connection Form" tab
3. Select driver: "Microsoft Access Driver (*.mdb, *.accdb)"
4. Enter file path in Server field: `C:\MyDatabase.accdb`
5. Leave Port field empty
6. Leave Database field empty
7. Configure password if needed (see above)
8. Click "Test Connection" or "Connect"

### Method 3: Connection String (Advanced)
1. Open: `Tools > Database > Database Connection`
2. Go to "Connection String" tab
3. Enter connection string:
   ```
   Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\path\to\your\database.accdb;
   ```
4. Add password if needed: `;PWD=yourpassword`
5. Click "Test Connection" or "Connect"

## Supported Access Formats

| Format | Extension | Description | Notes |
|--------|-----------|-------------|-------|
| Access 2007+ | .accdb | Modern Access format | Recommended for new databases |
| Access 97-2003 | .mdb | Legacy Access format | Still widely supported |

## Key Differences from Server Databases

### File-Based vs Server-Based
- **Server Field**: Contains file path instead of hostname
  - Example: `C:\Databases\Sales.accdb`
- **Port Field**: Not used (leave empty)
- **Database Field**: Not used (leave empty)
- **Authentication**: Usually no username, only database password if set

### Connection String Format
MS Access uses `DBQ` (Database Qualifier) parameter for file path:
```
DBQ=C:\path\to\database.accdb
```

Instead of Server/Database parameters:
```
Server=hostname;Database=dbname  (server databases)
```

## Prerequisites

### Required ODBC Drivers

**64-bit Application:**
- Microsoft Access Database Engine 2016 Redistributable (64-bit)
- Download: [Microsoft Download Center](https://www.microsoft.com/en-us/download/details.aspx?id=54920)

**32-bit Application:**
- Microsoft Access Database Engine 2016 Redistributable (32-bit)
- Usually included with Microsoft Office

**Check Installed Drivers:**
1. Open ODBC Data Source Administrator
   - Windows: Search "ODBC Data Sources"
   - Use 64-bit version for 64-bit apps
2. Go to "Drivers" tab
3. Look for "Microsoft Access Driver (*.mdb, *.accdb)"

### File Permissions
- **Read Access**: Application needs read permissions to the .accdb/.mdb file
- **Write Access**: Required for INSERT/UPDATE/DELETE operations
- **Network Shares**: Can connect to Access databases on network drives
- **Exclusive Access**: Some operations may require exclusive file access

## Common Issues & Solutions

### Issue: Driver Not Found
**Error**: `[Microsoft][ODBC Driver Manager] Data source name not found`

**Solutions:**
1. Install Microsoft Access Database Engine Redistributable
2. Ensure driver architecture (32/64-bit) matches your application
3. Check driver name in ODBC Administrator

### Issue: File Not Found
**Error**: `Could not find file 'C:\path\to\database.accdb'`

**Solutions:**
1. Verify file path is correct and file exists
2. Use absolute paths, not relative paths
3. Check for typos in file path
4. Ensure file extension is correct (.accdb vs .mdb)

### Issue: File Locked
**Error**: `The Microsoft Access database engine cannot open or write to the file...`

**Solutions:**
1. Close the file in Microsoft Access
2. Check if another application has the file open
3. Verify file is not read-only
4. Check file permissions

### Issue: Permission Denied
**Solutions:**
1. Run application with appropriate permissions
2. Check file permissions in Windows Explorer
3. Ensure anti-virus isn't blocking access
4. Move database to a location with proper permissions

## Limitations

### MS Access Limitations
- **Concurrent Users**: Limited to ~5-10 simultaneous users
- **File Size**: 2 GB maximum database size
- **Performance**: Not optimized for large datasets
- **Network Performance**: Slower over network shares
- **No Stored Procedures**: Limited to queries and VBA (VBA not accessible via ODBC)

### Recommended Use Cases
✅ **Good for:**
- Small to medium datasets (< 1 GB)
- Single-user or low-concurrency applications
- Desktop applications
- Prototyping and development
- Legacy system integration
- Offline/portable databases

❌ **Not recommended for:**
- High-traffic web applications
- Large datasets (> 1 GB)
- High-concurrency scenarios (> 10 users)
- Mission-critical production systems
- Distributed systems

## Migration Path

If your Access database grows beyond its limitations:

1. **SQL Server Express** (Free):
   - Up to 10 GB database size
   - Unlimited users
   - Full SQL Server features

2. **PostgreSQL** (Free, Open Source):
   - Unlimited size
   - High performance
   - Excellent concurrency

3. **Azure SQL Database** (Cloud):
   - Fully managed
   - Scalable
   - High availability

## Example Workflows

### Quick Test of Local Access Database
```
1. Click "MS Access Database" in Quick Connect
2. Edit Server field: C:\MyProjects\Inventory.accdb
3. Click "Test Connection"
4. If successful, click "Connect"
```

### Password-Protected Access Database
```
1. Select driver: Microsoft Access Driver (*.mdb, *.accdb)
2. Server: C:\SecureData\Protected.accdb
3. Uncheck "Trusted Connection"
4. Leave Username empty
5. Password: your_database_password
6. Click "Connect"
```

### Read-Only Access
```
Connection String:
Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\Reports\Data.accdb;ReadOnly=1;
```

## Integration with Application Features

Once connected to an Access database, you can use all standard database features:

- ✅ Execute SELECT queries
- ✅ Execute INSERT/UPDATE/DELETE commands
- ✅ Browse tables and schemas
- ✅ View table data
- ✅ Access field metadata
- ✅ Transaction support
- ✅ Multiple recordsets

## Technical Details

### ODBC Connection String Parameters

| Parameter | Description | Example |
|-----------|-------------|---------|
| DBQ | Database file path (required) | `DBQ=C:\Data\Sales.accdb` |
| PWD | Database password | `PWD=secret123` |
| ReadOnly | Open in read-only mode | `ReadOnly=1` |
| Exclusive | Open in exclusive mode | `Exclusive=1` |
| SystemDB | System database path | `SystemDB=C:\System.mdw` |

### DatabaseManager Integration

The DatabaseManager handles Access databases through the same interface:

```cpp
// Form-based connection
Database::ConnectionConfig config;
config.DriverName = "Microsoft Access Driver (*.mdb, *.accdb)";
config.ServerAddress = "C:\\MyDatabase.accdb";  // File path
config.ServerPort = 0;                          // Not used
config.DatabaseName = "";                       // Not used
config.Password = "dbpassword";                 // If database is password-protected

// Connection string method
std::string connStr = "Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\\MyDatabase.accdb;";
m_databaseManager.ConnectWithConnectionString(connStr);
```

## Testing Your Access Database

### Create a Test Database
1. Open Microsoft Access
2. Create new blank database: `TestDB.accdb`
3. Create a simple table:
   ```sql
   CREATE TABLE Users (
       ID AUTOINCREMENT PRIMARY KEY,
       Name TEXT(50),
       Email TEXT(100)
   );
   ```
4. Insert test data
5. Save and close Access
6. Connect using the application

### Verify Connection
1. Use "Test Connection" button first
2. Check connection information table
3. Verify Database Product shows "ACCESS"
4. Check driver version is correct

## Future Enhancements

Planned improvements for Access support:
- [ ] File browser button for selecting .accdb/.mdb files
- [ ] Recent databases list
- [ ] Database creation from application
- [ ] Compact and repair functionality
- [ ] Import/Export wizards
- [ ] Linked table manager

## See Also

- [Database Connection Window Documentation](DATABASE_CONNECTION_WINDOW.md)
- [Database System Overview](DATABASE_SYSTEM.md)
- [Build Guide](../BUILD_GUIDE.md)

## Resources

- [Microsoft Access Database Engine Downloads](https://www.microsoft.com/en-us/download/details.aspx?id=54920)
- [ODBC Connection Strings for Access](https://www.connectionstrings.com/access/)
- [Access Database Engine Architecture](https://docs.microsoft.com/en-us/office/troubleshoot/access/determine-office-version-and-architecture)
