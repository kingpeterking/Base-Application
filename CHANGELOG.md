# Changelog

All notable changes to the Base Application project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.3.0] - 2025-01-XX

### Added
- **Multiple Database Connections**: Support for maintaining multiple simultaneous database connections
  - Each connection has a unique name and can be managed independently
  - Active connection selector for choosing which connection to use
  - Connection manager window showing all active connections with their status
- **Duplicate Connection Prevention**: Automatic detection and blocking of duplicate connections
  - Form-based connections: Compares driver, server, database, and credentials
  - Connection String connections: Compares normalized connection strings
  - DSN connections: Compares DSN names to prevent duplicates
  - Special handling for file-based databases (MS Access)
- **Connection History**: Save and restore previous database connections
  - Automatic saving of successful connections to settings
  - Quick reconnect feature with password prompt for SQL Auth connections
  - Timestamp tracking for last-used connections
  - Delete individual history entries or clear all history
- **Database Schema Viewer**: Comprehensive database object browser
  - View all tables, views, columns, indexes, and stored procedures
  - Object type filtering (Tables, Views, Stored Procedures, Indexes)
  - Column information with data types, nullability, and keys
  - Index details including type and uniqueness
  - Stored procedure parameters and definitions
  - Connection selector for multi-connection environments
- **Database Object Inspector**: Detailed information window for database objects
  - Table metadata: Row counts, size, creation date
  - Column details with comprehensive type information
  - Index information with included columns
  - Foreign key relationships
  - Stored procedure source code viewing
- **Connection Name Management**: All connections now have unique, user-friendly names
  - Auto-generated sequential names ("Connection 1", "Connection 2", etc.)
  - DSN connections include DSN name in connection label
  - Custom connection names supported in connection form
  - Proper name display across all UI components

### Changed
- **Database Connection Window**: Improved to support multiple connections
  - Removed connection status blocking that prevented multiple connections
  - Enhanced connection string handling with proper config passing
  - Better error messages for duplicate connection attempts
- **DatabaseManager**: Enhanced with additional connection information methods
  - Added `GetUsername()` method to retrieve connection username
  - Added `GetOriginalConnectionString()` to support duplicate detection
  - Improved `Connect()` method to store normalized connection strings
  - Better connection name handling throughout the class
- **Connection Config**: Enhanced to support new features
  - Added `OriginalConnectionString` field for duplicate detection
  - Better handling of connection names across all connection types
  - Improved MS Access connection string generation

### Fixed
- Schema Viewer displaying "Connection (unnamed)" for all connections
- Connection String and DSN tabs not properly passing connection names
- Duplicate connection prevention not working due to missing `OriginalConnectionString`
- MS Access schema loading errors with improved error handling

### Documentation
- Added comprehensive database schema viewing documentation
- Created MS Access schema fix guide
- Updated database connection window documentation
- Added database inspection windows guide

## [2.2.0] - 2024-XX-XX

### Added
- MS Access database support (.mdb and .accdb files)
- DSN (Data Source Name) connection support
- Quick Connect presets for common database configurations
- Connection string preview and debugging tools
- Architecture detection (32-bit/64-bit) with setup hints

### Changed
- Improved database connection UI with tabbed interface
- Enhanced error reporting for database operations
- Better ODBC driver detection and display

## [2.1.0] - 2024-XX-XX

### Added
- Database connectivity with ODBC support
- nanodbc integration for cross-platform database access
- Database connection management
- Support for SQL Server, PostgreSQL, MySQL, SQLite

### Changed
- Enhanced application architecture for database operations

## [2.0.0] - 2024-XX-XX

### Added
- HTTP/HTTPS client with CURL integration
- File Explorer tool with favorites and filtering
- Cross-platform file system operations
- ImPlot v0.16 for data visualization
- Log viewer with severity levels
- Settings persistence with INI files

### Changed
- Refactored window management system
- Improved application class architecture
- Enhanced build system with precompiled headers

## [1.0.0] - 2024-XX-XX

### Added
- Initial release with Dear ImGui framework
- GLFW window management
- OpenGL 3.2 rendering
- CMake build system
- Cross-platform support (Windows, macOS, Linux)
- Basic window management system
- Main menu interface

