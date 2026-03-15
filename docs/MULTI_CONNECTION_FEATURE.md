# Multi-Connection Database Feature

## Overview
The application now supports **multiple simultaneous database connections**, allowing you to connect to multiple databases at the same time and switch between them seamlessly.

## Key Features

### 1. Multiple Active Connections
- Open connections to multiple databases simultaneously
- Each connection maintains its own independent state
- Switch between connections using the "Set Active" button
- Close individual connections without affecting others

### 2. Connection Naming
- Each connection has a unique name for easy identification
- Auto-generated names: "Connection 1", "Connection 2", etc.
- Custom names supported in the Connection Form
- DSN connections automatically named with DSN name

### 3. Connections Manager Window
Access via: **Tools > Database > Connections Manager**

#### Active Connections Tab
- View all open database connections in a table
- See connection details: Name, Driver, Server, Database, Status
- **Active indicator** (✓) shows which connection is currently active
- **Set Active** button to switch between connections
- **Disconnect** button to close individual connections
- Real-time status updates (Connected/Disconnected)

#### Connection History Tab
- Automatically saves all successful connections
- Shows last used timestamp for each connection
- **Reconnect** button for quick re-connection
- Password prompt when needed (passwords not saved for security)
- **Delete** button to remove entries from history
- **Clear All History** button to reset history
- Maximum 50 history entries (oldest removed automatically)

### 4. Connection Methods

All connection methods now create new connections and add them to the manager:

#### Connection Form
1. Enter connection details (driver, server, database, credentials)
2. Optionally enter a **Connection Name** or click **Auto-Generate**
3. Click **Connect** to create a new connection
4. Connection automatically added to active connections list
5. Set as active connection
6. Added to connection history

#### Connection String
1. Enter ODBC connection string
2. Click **Connect**
3. Auto-generates unique connection name
4. Added to connections list and history

#### Quick Connect
1. Click preset button (SQL Server, MySQL, PostgreSQL, etc.)
2. Switch to Connection Form tab
3. Customize settings if needed
4. Click **Connect**

#### DSN (Data Source Name)
1. Enter DSN name
2. Optional credentials
3. Click **Connect**
4. Connection named with DSN name + number

### 5. Backward Compatibility
- Original `m_databaseManager` still maintained for legacy code
- Single connection window still works as before
- Existing code continues to function without changes

## Usage Examples

### Example 1: Connect to Multiple Databases

```
1. Open Tools > Database > Database Connection
2. Connect to first database:
   - Connection Name: "Production DB"
   - Driver: SQL Server
   - Server: prod.server.com
   - Database: ProductionData
   - Click Connect

3. Connect to second database:
   - Connection Name: "Test DB"
   - Driver: SQL Server
   - Server: test.server.com
   - Database: TestData
   - Click Connect

4. Open Tools > Database > Connections Manager
5. View both connections in Active Connections tab
6. Use "Set Active" to switch between them
```

### Example 2: Reconnect from History

```
1. Open Tools > Database > Connections Manager
2. Go to Connection History tab
3. Find previously used connection
4. Click "Reconnect"
5. Enter password if prompted
6. New connection created and set as active
```

### Example 3: Manage Multiple Active Connections

```
1. Open Connections Manager
2. Active Connections tab shows all open connections
3. Click "Set Active" on any connection to make it current
4. Click "Disconnect" to close a connection
5. Active connection index adjusts automatically
```

## Technical Details

### Architecture

**Application.h:**
```cpp
// Multiple connections support
std::vector<std::shared_ptr<Database::DatabaseManager>> m_databaseConnections;
int m_activeConnectionIndex; // -1 = none

// Connection history
struct ConnectionHistoryEntry {
    Database::ConnectionConfig config;
    std::string lastUsedTimestamp;
};
std::vector<ConnectionHistoryEntry> m_connectionHistory;
```

**Connection Config Enhancement:**
```cpp
struct ConnectionConfig {
    std::string ConnectionName;  // NEW: User-friendly name
    std::string DriverName;
    std::string ServerAddress;
    int ServerPort;
    std::string DatabaseName;
    std::string Username;
    std::string Password;
    bool TrustedConnection;
    bool Encrypt;
    // ... other fields
};
```

### Helper Methods

```cpp
// Get the currently active connection
std::shared_ptr<Database::DatabaseManager> GetActiveConnection();

// Set a connection as active by index
void SetActiveConnection(int index);

// Add successful connection to history
void AddConnectionToHistory(const Database::ConnectionConfig& config);

// Generate unique connection number
int GenerateUniqueConnectionNumber();

// Load/Save history from settings.ini
void LoadConnectionHistory();
void SaveConnectionHistory();
```

### Settings Storage

Connection history is stored in `settings.ini`:

```ini
[ConnectionHistory]
Count=3

[ConnectionHistory_0]
Name=Production DB
Driver=SQL Server
Server=prod.server.com
Port=1433
Database=ProductionData
Username=sa
TrustedConnection=false
Encrypt=true
LastUsed=2024-01-15 14:30:22

[ConnectionHistory_1]
Name=Test DB
Driver=SQL Server
Server=test.server.com
Port=1433
Database=TestData
Username=testuser
TrustedConnection=false
Encrypt=false
LastUsed=2024-01-15 15:45:10
```

**Note:** Passwords are **never** stored for security reasons.

### Connection Creation Flow

1. User enters connection details in Database Connection Window
2. Clicks "Connect" button
3. System generates unique connection name if not provided
4. Creates new `DatabaseManager` instance
5. Attempts connection with provided config
6. On success:
   - Adds to `m_databaseConnections` vector
   - Sets as active connection (`m_activeConnectionIndex`)
   - Adds to connection history
   - Updates status message
7. On failure:
   - Shows error message
   - Connection not added to list

## Benefits

### 1. Productivity
- Work with multiple databases simultaneously
- No need to disconnect/reconnect when switching databases
- Quick reconnect from history saves time

### 2. Comparison & Migration
- Compare data between production and test environments
- Migrate data between databases
- Cross-database queries (when implemented)

### 3. Development Workflow
- Connect to dev, test, and prod environments simultaneously
- Switch between databases without losing connections
- Test against multiple database versions

### 4. Connection Management
- Clear visual indicator of active connection
- Easy disconnect without affecting other connections
- History helps recall frequently used connections

## Future Enhancements

Planned improvements:
- [ ] Export/Import connection configurations
- [ ] Connection groups/favorites
- [ ] Connection status notifications
- [ ] Auto-reconnect on connection loss
- [ ] Connection pooling
- [ ] Cross-connection queries
- [ ] Connection sharing between windows
- [ ] Custom connection icons/colors
- [ ] Connection search/filter in manager
- [ ] Bulk operations across multiple connections

## Security Notes

1. **Passwords Not Stored**: For security, passwords are never saved in connection history
2. **Password Prompt**: When reconnecting, you'll need to re-enter passwords
3. **Trusted Connections**: Windows Authentication connections don't require password storage
4. **Settings File**: Connection history is stored in plain text `settings.ini` - keep it secure

## Troubleshooting

### Issue: "No active connection selected"
**Solution:** Open Connections Manager and click "Set Active" on a connection

### Issue: Can't find my connection
**Solution:** Check Connections Manager > Active Connections tab, or reconnect from History tab

### Issue: Connection history not saving
**Solution:** Ensure `settings.ini` file is writable in the application directory

### Issue: Too many connections in history
**Solution:** Use "Clear All History" button in Connection History tab, or manually delete entries

## See Also

- [Database Connection Window Documentation](DATABASE_CONNECTION_WINDOW.md)
- [Database System Overview](DATABASE_SYSTEM.md)
- [MS Access Support](MS_ACCESS_SUPPORT.md)
- [Build Guide](../BUILD_GUIDE.md)
