#!/usr/bin/env python3
"""
Script to split WindowFunctions.cpp into organized files
"""

import re

# Read the original file
with open('src/WindowFunctions/WindowFunctions.cpp', 'r', encoding='utf-8') as f:
    content = f.read()

# Function boundaries (using regex to find function definitions)
functions = {
    'RenderFileExplorerWindow': 'FileSystem',
    'RenderLogViewerWindow': 'FileSystem',
    'RenderDatabaseConnectionWindow': 'Database',
    'RenderDatabaseConnectionsManagerWindow': 'Database'
}

# Extract each function
function_code = {}
for func_name, category in functions.items():
    pattern = rf'void WindowFunctions::{func_name}\(bool\* isOpen\)[\s\S]*?^\}}\s*$'
    match = re.search(pattern, content, re.MULTILINE)
    if match:
        function_code[func_name] = match.group(0)
        print(f"✓ Extracted {func_name} ({len(match.group(0))} chars)")
    else:
        print(f"✗ Failed to extract {func_name}")

# Create FileSystem file
filesystem_header = '''#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "Tools/Logger.h"

// ============================================================================
// FILESYSTEM WINDOW FUNCTIONS
// ============================================================================
// File Explorer, Log Viewer
// ============================================================================

'''

filesystem_code = filesystem_header
filesystem_code += function_code.get('RenderFileExplorerWindow', '') + '\n\n'
filesystem_code += function_code.get('RenderLogViewerWindow', '')

with open('src/WindowFunctions/WindowFunctions_FileSystem.cpp', 'w', encoding='utf-8') as f:
    f.write(filesystem_code)
print(f"\n✓ Created WindowFunctions_FileSystem.cpp ({len(filesystem_code)} chars)")

# Create Database file  
database_header = '''#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "Tools/Logger.h"

// ============================================================================
// DATABASE WINDOW FUNCTIONS
// ============================================================================
// Database Connection, Database Connections Manager
// ============================================================================

'''

database_code = database_header
database_code += function_code.get('RenderDatabaseConnectionWindow', '') + '\n\n'
database_code += function_code.get('RenderDatabaseConnectionsManagerWindow', '')

with open('src/WindowFunctions/WindowFunctions_Database.cpp', 'w', encoding='utf-8') as f:
    f.write(database_code)
print(f"✓ Created WindowFunctions_Database.cpp ({len(database_code)} chars)")

# Create new main file with just constructor/destructor
main_code = '''#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"

// ============================================================================
// WINDOW FUNCTIONS - MAIN FILE
// ============================================================================
// Constructor and Destructor only
// Individual window rendering functions are split into:
//   - WindowFunctions_Core.cpp (Main Menu, Demo, Application, ImPlot)
//   - WindowFunctions_Web.cpp (URL Request, Web Server)
//   - WindowFunctions_FileSystem.cpp (File Explorer, Log Viewer)
//   - WindowFunctions_Database.cpp (Database Connection, Connections Manager)
// ============================================================================

WindowFunctions::WindowFunctions(Application* app)
    : m_app(app)
{
}

WindowFunctions::~WindowFunctions()
{
}
'''

with open('src/WindowFunctions/WindowFunctions.cpp', 'w', encoding='utf-8') as f:
    f.write(main_code)
print(f"✓ Updated WindowFunctions.cpp ({len(main_code)} chars)")

print("\n✅ Split complete! Don't forget to update CMakeLists.txt")
