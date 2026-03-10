# Database Connection Window

## Overview
The Database Connection Window provides a user-friendly ImGui interface for connecting to various databases using ODBC.

## Features

### Connection Methods

The window provides three ways to connect to a database:

#### 1. Connection Form Tab
A comprehensive form-based interface with the following fields:

**Connection Settings:**
- **Driver**: Dropdown selection of ODBC drivers
  - Shows common drivers (SQL Server, PostgreSQL, MySQL, SQLite, Oracle)
  - Lists all installed ODBC drivers on the system
- **Server**: Server hostname or IP address
- **Port**: Database server port number
- **Database**: Target database name

**Authentication:**
- **Trusted Connection**: Toggle for Windows Authentication (SQL Server)
- **Username**: Database username (when not using trusted connection)
- **Password**: Secure password input (masked)

**Options:**
- **Encrypt Connection**: Enable SSL/TLS encryption
- **Connection Timeout**: Timeout for establishing connection (5-300 seconds)
- **Command Timeout**: Timeout for executing commands (5-300 seconds)

**Actions:**
- **Connect**: Establish connection to the database
- **Test Connection**: Verify connection parameters without establishing a persistent connection
- **Disconnect**: Close active database connection

#### 2. Connection String Tab
For advanced users who prefer raw ODBC connection strings:

- Multiline text input for entering complete ODBC connection strings
- **Examples provided** for:
  - SQL Server (Windows Auth)
  - SQL Server (SQL Auth)
  - Azure SQL Database
  - PostgreSQL
  - MySQL

#### 3. Quick Connect Tab
Preset configurations for common database setups:

- **Local SQL Server (Windows Auth)**
  - Driver: SQL Server
  - Server: localhost:1433
  - Database: master
  - Authentication: Windows (Trusted)

- **Local PostgreSQL**
  - Driver: PostgreSQL Unicode
  - Server: localhost:5432
  - Database: postgres
  - Username: postgres

- **Local MySQL**
  - Driver: MySQL ODBC 8.0 Driver
  - Server: localhost:3306
  - Database: mysql
  - Username: root

- **Azure SQL Database**
  - Driver: ODBC Driver 17 for SQL Server
  - Server: *.database.windows.net:1433
  - Encryption: Enabled
  - Ready for cloud deployment

## Connection Information Display

When connected, the window displays comprehensive connection details:

| Property | Description |
|----------|-------------|
| Server | Server hostname or IP |
| Database Product | DBMS name (e.g., "Microsoft SQL Server", "PostgreSQL") |
| Product Version | Database version |
| Driver Name | ODBC driver being used |
| Driver Version | Driver version |
| Current Database | Active database name |

## Status Indicators

- **Green "Connected" badge**: When connection is active
- **Status text**: Shows connection status, error messages, or operation results
- **Real-time updates**: Status updates immediately after connect/disconnect operations

## Integration with Application

The connection window integrates with:

- **Application class**: Stores connection state and configuration
- **DatabaseManager**: Manages actual database connections
- **Logger**: All connection operations are logged
- **Menu System**: Accessible via `Tools > Database > Database Connection`

## Usage Workflow

### Quick Start (Local SQL Server)
1. Open window from menu: `Tools > Database > Database Connection`
2. Go to "Quick Connect" tab
3. Click "Local SQL Server (Windows Auth)"
4. Switch to "Connection Form" tab
5. Click "Connect"

### Custom Connection
1. Open "Connection Form" tab
2. Select your ODBC driver from dropdown
3. Enter server address and port
4. Enter database name
5. Configure authentication (Windows Auth or username/password)
6. Optionally enable encryption
7. Click "Test Connection" to verify settings
8. Click "Connect" to establish connection

### Connection String Method
1. Open "Connection String" tab
2. Enter or paste ODBC connection string
3. Use examples as reference
4. Click "Test Connection" to verify
5. Click "Connect" to establish connection

## Technical Implementation

### State Management
All connection configuration is stored in the Application class:
- Form fields use fixed-size buffers (256 bytes each)
- Connection string uses 1024-byte buffer
- State persists between window opens/closes

### Thread Safety
- DatabaseManager operations are synchronous
- All UI interactions run on the main thread
- No threading issues with ODBC operations

### Error Handling
- All connection errors are caught and displayed in the status
- Errors are logged to the application's log system
- Provides clear, actionable error messages

## Supported Databases

Via ODBC, the application supports:

**Cloud Databases:**
- Azure SQL Database
- Azure Database for PostgreSQL
- Azure Database for MySQL
- Amazon RDS (SQL Server, PostgreSQL, MySQL)
- Google Cloud SQL

**Local Databases:**
**Local Databases:**
- Microsoft SQL Server
- PostgreSQL
- MySQL
- SQLite
- Oracle Database
- MariaDB
- **Microsoft Access** (.mdb, .accdb)
- Any database with ODBC driver support

## MS Access-Specific Notes

Microsoft Access databases are file-based rather than server-based. When connecting to Access:

### Connection Form Method
1. Select "Microsoft Access Driver (*.mdb, *.accdb)" as the driver
2. In the **Server** field, enter the full path to your .mdb or .accdb file
   - Example: `C:\Users\YourName\Documents\MyDatabase.accdb`
   - Example: `D:\Projects\Data\SalesData.mdb`
3. Leave the **Port** field empty (not used for file-based databases)
4. Leave the **Database** field empty (not used for file-based databases)
5. If your Access database has a password:
   - Uncheck "Trusted Connection"
   - Leave Username empty
   - Enter the database password in the Password field

### Connection String Method
Use one of these formats:

**Without Password:**
```
Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\MyDatabase.accdb;
```

**With Password:**
```
Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=C:\MyDatabase.accdb;PWD=mypassword;
```

**Legacy .mdb format:**
```
Driver={Microsoft Access Driver (*.mdb)};DBQ=C:\MyDatabase.mdb;
```

### Quick Connect Preset
The "MS Access Database" quick connect preset provides:
- Pre-configured driver selection
- Template file path (`C:\path\to\database.accdb`)
- Proper settings for file-based access
- Just edit the Server field to point to your actual .accdb or .mdb file

### Important Notes
- **File Path**: Must be a valid absolute path to an existing Access database file
- **File Formats**: Supports both .mdb (Access 97-2003) and .accdb (Access 2007+)
- **32-bit vs 64-bit**: Ensure your ODBC driver matches your application architecture
  - 64-bit application requires 64-bit Access Database Engine
  - Download from: Microsoft Access Database Engine Redistributable
- **Read-Only Access**: If the file is read-only or locked, connection may succeed but writes will fail
- **Concurrent Access**: Access has limitations on concurrent users

## Prerequisites

Before using the database connection window:

1. **Install ODBC Drivers**: Install appropriate ODBC drivers for your target database
2. **Windows**: Most SQL Server drivers pre-installed
3. **Configure DSN** (optional): Can use DSN-less connections
4. **Network Access**: Ensure firewall rules allow database connections
5. **Credentials**: Have valid database credentials ready

## Next Steps

After establishing a connection, users can:
- Browse database schema (future window)
- Execute SQL queries (future window)
- Manage tables and data (future windows)
- Perform batch operations
- Set up replication

## Code Location

- **Header**: `include/WindowFunctions/WindowFunctions.h`
- **Implementation**: `src/WindowFunctions/WindowFunctions.cpp` (RenderDatabaseConnectionWindow function)
- **Application State**: `include/Application.h` (database-related members)
- **Application Logic**: `src/Application.cpp` (initialization and delegation)

## Menu Access

**Menu Path**: `Tools > Database > Database Connection`

The window can be toggled on/off from the main menu and will remember its visibility state through the WindowManager system.
