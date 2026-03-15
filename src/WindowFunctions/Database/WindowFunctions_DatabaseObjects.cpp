#include "pch.h"
#include "WindowFunctions/WindowFunctions.h"
#include "Application.h"
#include "Tools/Logger.h"

// DATABASE OBJECT BROWSER WINDOW FUNCTIONS

void WindowFunctions::RenderDatabaseObjectBrowserWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Object Browser", isOpen))
    {
        ImGui::End();
        return;
    }

    auto activeConn = m_app->GetActiveConnection();
    if (!activeConn || !activeConn->IsConnected())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No active database connection.");
        ImGui::Text("Please connect to a database first.");
        ImGui::End();
        return;
    }

    auto db = activeConn->GetDatabase();
    if (!db)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Unable to access database.");
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Connected to:");
    ImGui::SameLine();
    ImGui::Text("%s - %s", activeConn->GetConnectionName().c_str(), db->GetName().c_str());
    ImGui::Separator();

    // Static variables for caching and filtering
    static std::vector<Database::Database::DatabaseObjectInfo> allObjects;
    static bool objectsLoaded = false;
    static int filterType = 0; // 0 = All, 1 = Tables, 2 = Views, 3 = Procedures
    static int prevFilterType = -1;
    static char searchBuffer[256] = "";
    static int selectedIndex = -1;

    // Filter type selection
    ImGui::Text("Filter by Type:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(200);
    const char* filterTypes[] = { "All Objects", "Tables Only", "Views Only", "Procedures Only" };
    if (ImGui::Combo("##filterType", &filterType, filterTypes, IM_ARRAYSIZE(filterTypes)))
    {
        objectsLoaded = false;
        selectedIndex = -1;
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh"))
    {
        objectsLoaded = false;
        selectedIndex = -1;
    }

    // Search filter
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer));

    ImGui::Separator();

    // Load objects if needed
    if (!objectsLoaded || prevFilterType != filterType)
    {
        std::string typeFilterStr = "";
        if (filterType == 1) typeFilterStr = "TABLE";
        else if (filterType == 2) typeFilterStr = "VIEW";
        else if (filterType == 3) typeFilterStr = "PROCEDURE";

        allObjects = db->GetDatabaseObjects(typeFilterStr);
        objectsLoaded = true;
        prevFilterType = filterType;
        LOG_INFO("Loaded " + std::to_string(allObjects.size()) + " database objects");
    }

    // Apply search filter
    std::vector<Database::Database::DatabaseObjectInfo> filteredObjects;
    std::string searchStr = searchBuffer;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    for (const auto& obj : allObjects)
    {
        std::string objName = obj.Name;
        std::transform(objName.begin(), objName.end(), objName.begin(), ::tolower);

        if (searchStr.empty() || objName.find(searchStr) != std::string::npos)
        {
            filteredObjects.push_back(obj);
        }
    }

    ImGui::Text("Objects: %d", static_cast<int>(filteredObjects.size()));

    // Split panel layout
    float leftPanelWidth = 350.0f;
    ImVec2 availSize = ImGui::GetContentRegionAvail();

    // Left panel - Object list
    ImGui::BeginChild("ObjectList", ImVec2(leftPanelWidth, availSize.y), true);
    {
        if (filteredObjects.empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No objects found.");
        }
        else
        {
            for (int i = 0; i < static_cast<int>(filteredObjects.size()); ++i)
            {
                const auto& obj = filteredObjects[i];

                // Color code by type
                ImVec4 typeColor;
                const char* typeLabel;
                if (obj.Type == "Table")
                {
                    typeColor = ImVec4(0.4f, 0.8f, 1.0f, 1.0f); // Light blue
                    typeLabel = "[T]";
                }
                else if (obj.Type == "View")
                {
                    typeColor = ImVec4(0.8f, 1.0f, 0.4f, 1.0f); // Yellow-green
                    typeLabel = "[V]";
                }
                else // Procedure
                {
                    typeColor = ImVec4(1.0f, 0.7f, 0.3f, 1.0f); // Orange
                    typeLabel = "[P]";
                }

                ImGui::PushID(i);
                ImGui::TextColored(typeColor, typeLabel);
                ImGui::SameLine();

                if (ImGui::Selectable(obj.Name.c_str(), selectedIndex == i))
                {
                    selectedIndex = i;
                }

                ImGui::PopID();
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - Object details
    ImGui::BeginChild("ObjectDetails", ImVec2(0, availSize.y), true);
    {
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(filteredObjects.size()))
        {
            const auto& obj = filteredObjects[selectedIndex];

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Object Details");
            ImGui::Separator();

            ImGui::Text("Name:"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", obj.Name.c_str());
            ImGui::Text("Type:"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", obj.Type.c_str());

            if (!obj.Schema.empty())
            {
                ImGui::Text("Schema:"); ImGui::SameLine(); ImGui::Text("%s", obj.Schema.c_str());
            }

            if (!obj.Remarks.empty())
            {
                ImGui::Text("Remarks:"); ImGui::SameLine(); ImGui::TextWrapped("%s", obj.Remarks.c_str());
            }

            ImGui::Spacing();

            if (!obj.Definition.empty())
            {
                ImGui::Text("SQL Definition:");
                if (ImGui::Button("Copy Definition"))
                {
                    ImGui::SetClipboardText(obj.Definition.c_str());
                    LOG_INFO("Copied definition to clipboard");
                }

                ImGui::Spacing();
                ImVec2 textSize = ImVec2(-1, ImGui::GetContentRegionAvail().y - 10);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                ImGui::InputTextMultiline("##definition", const_cast<char*>(obj.Definition.c_str()),
                    obj.Definition.size() + 1, textSize, ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No definition available for this object type.");
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select an object to view details.");
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void WindowFunctions::RenderDatabaseTablesWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Tables", isOpen))
    {
        ImGui::End();
        return;
    }

    auto activeConn = m_app->GetActiveConnection();
    if (!activeConn || !activeConn->IsConnected())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No active database connection.");
        ImGui::Text("Please connect to a database first.");
        ImGui::End();
        return;
    }

    auto db = activeConn->GetDatabase();
    if (!db)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Unable to access database.");
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Connected to:");
    ImGui::SameLine();
    ImGui::Text("%s - %s", activeConn->GetConnectionName().c_str(), db->GetName().c_str());
    ImGui::Separator();

    // Static variables for caching
    static std::vector<std::string> tableNames;
    static bool tablesLoaded = false;
    static char searchBuffer[256] = "";

    // Refresh button
    if (ImGui::Button("Refresh Tables"))
    {
        tablesLoaded = false;
    }

    ImGui::SameLine();
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer));

    ImGui::Separator();

    // Load tables if needed
    if (!tablesLoaded)
    {
        tableNames = db->GetTableNames();
        tablesLoaded = true;
        LOG_INFO("Loaded " + std::to_string(tableNames.size()) + " tables");
    }

    // Apply search filter
    std::vector<std::string> filteredTables;
    std::string searchStr = searchBuffer;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    for (const auto& tableName : tableNames)
    {
        std::string lowerName = tableName;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (searchStr.empty() || lowerName.find(searchStr) != std::string::npos)
        {
            filteredTables.push_back(tableName);
        }
    }

    ImGui::Text("Total Tables: %d", static_cast<int>(filteredTables.size()));
    ImGui::Separator();

    // Table list
    ImVec2 availSize = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("TableList", availSize, true);
    {
        if (filteredTables.empty())
        {
            if (tableNames.empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No tables found in database.");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No tables match search criteria.");
            }
        }
        else
        {
            if (ImGui::BeginTable("TablesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Table Name", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 250.0f);
                ImGui::TableHeadersRow();

                for (const auto& tableName : filteredTables)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(tableName.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::PushID(tableName.c_str());

                    if (ImGui::SmallButton("View Data"))
                    {
                        // TODO: Implement view data functionality
                        LOG_INFO("View data for table: " + tableName);
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void WindowFunctions::RenderDatabaseViewsWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Views", isOpen))
    {
        ImGui::End();
        return;
    }

    auto activeConn = m_app->GetActiveConnection();
    if (!activeConn || !activeConn->IsConnected())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No active database connection.");
        ImGui::Text("Please connect to a database first.");
        ImGui::End();
        return;
    }

    auto db = activeConn->GetDatabase();
    if (!db)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Unable to access database.");
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Connected to:");
    ImGui::SameLine();
    ImGui::Text("%s - %s", activeConn->GetConnectionName().c_str(), db->GetName().c_str());
    ImGui::Separator();

    // Note for MS Access
    std::string driverName = activeConn->GetDriverName();
    if (driverName.find("Access") != std::string::npos)
    {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Note: MS Access calls views 'queries'");
        ImGui::Separator();
    }

    // Static variables for caching
    static std::vector<Database::Database::DatabaseObjectInfo> viewObjects;
    static bool viewsLoaded = false;
    static char searchBuffer[256] = "";
    static int selectedIndex = -1;

    // Refresh button
    if (ImGui::Button("Refresh Views"))
    {
        viewsLoaded = false;
        selectedIndex = -1;
    }

    ImGui::SameLine();
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer));

    ImGui::Separator();

    // Load views if needed
    if (!viewsLoaded)
    {
        viewObjects = db->GetDatabaseObjects("VIEW");
        viewsLoaded = true;
        LOG_INFO("Loaded " + std::to_string(viewObjects.size()) + " views");
    }

    // Apply search filter
    std::vector<Database::Database::DatabaseObjectInfo> filteredViews;
    std::string searchStr = searchBuffer;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    for (const auto& viewObj : viewObjects)
    {
        std::string lowerName = viewObj.Name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (searchStr.empty() || lowerName.find(searchStr) != std::string::npos)
        {
            filteredViews.push_back(viewObj);
        }
    }

    ImGui::Text("Total Views: %d", static_cast<int>(filteredViews.size()));
    ImGui::Separator();

    // Split panel layout
    float leftPanelWidth = 300.0f;
    ImVec2 availSize = ImGui::GetContentRegionAvail();

    // Left panel - View list
    ImGui::BeginChild("ViewList", ImVec2(leftPanelWidth, availSize.y), true);
    {
        if (filteredViews.empty())
        {
            if (viewObjects.empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No views found in database.");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No views match search criteria.");
            }
        }
        else
        {
            for (int i = 0; i < static_cast<int>(filteredViews.size()); ++i)
            {
                if (ImGui::Selectable(filteredViews[i].Name.c_str(), selectedIndex == i))
                {
                    selectedIndex = i;
                }
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - View details
    ImGui::BeginChild("ViewDetails", ImVec2(0, availSize.y), true);
    {
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(filteredViews.size()))
        {
            const auto& viewObj = filteredViews[selectedIndex];

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "View: %s", viewObj.Name.c_str());
            ImGui::Separator();

            if (ImGui::Button("Query View Data"))
            {
                // TODO: Implement query view data functionality
                LOG_INFO("Query data for view: " + viewObj.Name);
            }
            ImGui::SameLine();
            if (ImGui::Button("Copy Name"))
            {
                ImGui::SetClipboardText(viewObj.Name.c_str());
                LOG_INFO("Copied view name to clipboard");
            }

            ImGui::Spacing();

            if (!viewObj.Definition.empty())
            {
                ImGui::Text("SQL Definition:");
                if (ImGui::Button("Copy Definition"))
                {
                    ImGui::SetClipboardText(viewObj.Definition.c_str());
                    LOG_INFO("Copied view definition to clipboard");
                }

                ImGui::Spacing();
                ImVec2 textSize = ImVec2(-1, ImGui::GetContentRegionAvail().y - 10);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                ImGui::InputTextMultiline("##definition", const_cast<char*>(viewObj.Definition.c_str()),
                    viewObj.Definition.size() + 1, textSize, ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), 
                    "View definition not available for this database type.");
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select a view to view details.");
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void WindowFunctions::RenderDatabaseProceduresWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Stored Procedures", isOpen))
    {
        ImGui::End();
        return;
    }

    auto activeConn = m_app->GetActiveConnection();
    if (!activeConn || !activeConn->IsConnected())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No active database connection.");
        ImGui::Text("Please connect to a database first.");
        ImGui::End();
        return;
    }

    auto db = activeConn->GetDatabase();
    if (!db)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Unable to access database.");
        ImGui::End();
        return;
    }

    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Connected to:");
    ImGui::SameLine();
    ImGui::Text("%s - %s", activeConn->GetConnectionName().c_str(), db->GetName().c_str());
    ImGui::Separator();

    // Note for MS Access
    std::string driverName = activeConn->GetDriverName();
    if (driverName.find("Access") != std::string::npos)
    {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), 
            "Note: MS Access has limited stored procedure support (uses VBA modules instead)");
        ImGui::Separator();
    }

    // Static variables for caching
    static std::vector<Database::Database::DatabaseObjectInfo> procedureObjects;
    static bool proceduresLoaded = false;
    static char searchBuffer[256] = "";
    static int selectedIndex = -1;
    static std::string selectedProcedureForSchema = "";
    static bool showProcedureSchemaPopup = false;

    struct ProcedureParameter {
        std::string Name;
        std::string DataType;
        std::string Mode; // IN, OUT, INOUT
        int MaxLength;
    };
    static std::vector<ProcedureParameter> cachedProcedureParameters;

    // Refresh button
    if (ImGui::Button("Refresh Procedures"))
    {
        proceduresLoaded = false;
        selectedIndex = -1;
    }

    ImGui::SameLine();
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300);
    ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer));

    ImGui::Separator();

    // Load procedures if needed
    if (!proceduresLoaded)
    {
        procedureObjects = db->GetDatabaseObjects("PROCEDURE");
        proceduresLoaded = true;
        LOG_INFO("Loaded " + std::to_string(procedureObjects.size()) + " stored procedures");
    }

    // Apply search filter
    std::vector<Database::Database::DatabaseObjectInfo> filteredProcedures;
    std::string searchStr = searchBuffer;
    std::transform(searchStr.begin(), searchStr.end(), searchStr.begin(), ::tolower);

    for (const auto& procObj : procedureObjects)
    {
        std::string lowerName = procObj.Name;
        std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

        if (searchStr.empty() || lowerName.find(searchStr) != std::string::npos)
        {
            filteredProcedures.push_back(procObj);
        }
    }

    ImGui::Text("Total Procedures: %d", static_cast<int>(filteredProcedures.size()));
    ImGui::Separator();

    // Split panel layout
    float leftPanelWidth = 300.0f;
    ImVec2 availSize = ImGui::GetContentRegionAvail();

    // Left panel - Procedure list
    ImGui::BeginChild("ProcedureList", ImVec2(leftPanelWidth, availSize.y), true);
    {
        if (filteredProcedures.empty())
        {
            if (procedureObjects.empty())
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No stored procedures found in database.");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No procedures match search criteria.");
            }
        }
        else
        {
            for (int i = 0; i < static_cast<int>(filteredProcedures.size()); ++i)
            {
                if (ImGui::Selectable(filteredProcedures[i].Name.c_str(), selectedIndex == i))
                {
                    selectedIndex = i;
                }
            }
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right panel - Procedure details
    ImGui::BeginChild("ProcedureDetails", ImVec2(0, availSize.y), true);
    {
        if (selectedIndex >= 0 && selectedIndex < static_cast<int>(filteredProcedures.size()))
        {
            const auto& procObj = filteredProcedures[selectedIndex];

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Procedure: %s", procObj.Name.c_str());
            ImGui::Separator();

            if (ImGui::Button("Execute"))
            {
                // TODO: Implement execute procedure functionality (with parameter dialog)
                LOG_INFO("Execute procedure: " + procObj.Name);
            }
            ImGui::SameLine();
            if (ImGui::Button("View Parameters"))
            {
                selectedProcedureForSchema = procObj.Name;
                showProcedureSchemaPopup = true;
                cachedProcedureParameters.clear();

                // Query procedure parameters using INFORMATION_SCHEMA
                try {
                    std::string dbProduct = db->GetDatabaseProduct();
                    std::string sql;

                    if (dbProduct.find("SQL Server") != std::string::npos || dbProduct.find("Microsoft") != std::string::npos)
                    {
                        sql = "SELECT PARAMETER_NAME, DATA_TYPE, PARAMETER_MODE, CHARACTER_MAXIMUM_LENGTH "
                              "FROM INFORMATION_SCHEMA.PARAMETERS "
                              "WHERE SPECIFIC_NAME = '" + procObj.Name + "' "
                              "ORDER BY ORDINAL_POSITION";

                        auto recordset = db->ExecuteQuery(sql);
                        auto records = recordset.GetRecords();

                        for (const auto& record : records)
                        {
                            ProcedureParameter param;

                            // Get PARAMETER_NAME
                            if (auto field = record.GetField("PARAMETER_NAME"))
                            {
                                if (auto val = field->GetString())
                                    param.Name = *val;
                            }

                            // Get DATA_TYPE
                            if (auto field = record.GetField("DATA_TYPE"))
                            {
                                if (auto val = field->GetString())
                                    param.DataType = *val;
                            }

                            // Get PARAMETER_MODE
                            if (auto field = record.GetField("PARAMETER_MODE"))
                            {
                                if (auto val = field->GetString())
                                    param.Mode = *val;
                                else
                                    param.Mode = "IN";
                            }
                            else
                            {
                                param.Mode = "IN";
                            }

                            // Get CHARACTER_MAXIMUM_LENGTH
                            if (auto field = record.GetField("CHARACTER_MAXIMUM_LENGTH"))
                            {
                                if (auto val = field->GetInt())
                                    param.MaxLength = *val;
                                else
                                    param.MaxLength = 0;
                            }
                            else
                            {
                                param.MaxLength = 0;
                            }

                            cachedProcedureParameters.push_back(param);
                        }
                    }

                    LOG_INFO("Loaded " + std::to_string(cachedProcedureParameters.size()) + " parameters for procedure: " + procObj.Name);
                }
                catch (const std::exception& e) {
                    LOG_ERROR("Failed to load procedure parameters: " + std::string(e.what()));
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Copy Name"))
            {
                ImGui::SetClipboardText(procObj.Name.c_str());
                LOG_INFO("Copied procedure name to clipboard");
            }

            ImGui::Spacing();

            if (!procObj.Schema.empty())
            {
                ImGui::Text("Schema:"); ImGui::SameLine(); ImGui::Text("%s", procObj.Schema.c_str());
            }

            if (!procObj.Remarks.empty())
            {
                ImGui::Text("Remarks:");
                ImGui::TextWrapped("%s", procObj.Remarks.c_str());
                ImGui::Spacing();
            }

            // Display procedure definition
            if (!procObj.Definition.empty())
            {
                ImGui::Text("SQL Definition:");
                if (ImGui::Button("Copy Definition"))
                {
                    ImGui::SetClipboardText(procObj.Definition.c_str());
                    LOG_INFO("Copied procedure definition to clipboard");
                }

                ImGui::Spacing();
                ImVec2 textSize = ImVec2(-1, ImGui::GetContentRegionAvail().y - 10);
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
                ImGui::InputTextMultiline("##definition", const_cast<char*>(procObj.Definition.c_str()),
                    procObj.Definition.size() + 1, textSize, ImGuiInputTextFlags_ReadOnly);
                ImGui::PopStyleColor();
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), 
                    "Procedure definition not available for this database type.");
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Select a procedure to view details.");
        }
    }
    ImGui::EndChild();

    // Procedure Parameters window (regular window, not modal)
    static bool showProcedureSchemaWindow = false;
    if (showProcedureSchemaPopup)
    {
        showProcedureSchemaWindow = true;
        showProcedureSchemaPopup = false;
    }

    if (showProcedureSchemaWindow)
    {
        ImGui::SetNextWindowSize(ImVec2(700, 450), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Procedure Parameters##ProceduresWindow", &showProcedureSchemaWindow))
        {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Parameters for Procedure: %s", selectedProcedureForSchema.c_str());
        ImGui::Separator();

        if (cachedProcedureParameters.empty())
        {
            ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No parameters found or procedure has no parameters.");
            ImGui::Text("This could mean:");
            ImGui::BulletText("The procedure has no parameters");
            ImGui::BulletText("Parameter information is not available for this database type");
            ImGui::BulletText("Failed to query INFORMATION_SCHEMA.PARAMETERS");
        }
        else
        {
            ImGui::Text("Total Parameters: %d", static_cast<int>(cachedProcedureParameters.size()));
            ImGui::Separator();

            if (ImGui::BeginTable("ParametersTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2(650, 300)))
            {
                ImGui::TableSetupColumn("Parameter Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                ImGui::TableSetupColumn("Data Type", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                ImGui::TableSetupColumn("Mode", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                ImGui::TableSetupColumn("Max Length", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const auto& param : cachedProcedureParameters)
                {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("%s", param.Name.c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%s", param.DataType.c_str());

                    ImGui::TableSetColumnIndex(2);
                    // Color code the mode
                    if (param.Mode == "IN")
                        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", param.Mode.c_str());
                    else if (param.Mode == "OUT")
                        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "%s", param.Mode.c_str());
                    else if (param.Mode == "INOUT")
                        ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.4f, 1.0f), "%s", param.Mode.c_str());
                    else
                        ImGui::Text("%s", param.Mode.c_str());

                    ImGui::TableSetColumnIndex(3);
                    if (param.MaxLength > 0)
                        ImGui::Text("%d", param.MaxLength);
                    else
                        ImGui::Text("-");
                }

                ImGui::EndTable();
            }
        }

        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            showProcedureSchemaWindow = false;
        }
        }
        ImGui::End();
    }

    ImGui::End();
}

// DATABASE SCHEMA VIEWER WINDOW - Dedicated window for viewing table schemas
void WindowFunctions::RenderDatabaseSchemaViewerWindow(bool* isOpen)
{
    ImGui::SetNextWindowSize(ImVec2(1000, 700), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin("Database Schema Viewer", isOpen))
    {
        ImGui::End();
        return;
    }

    const auto& connections = m_app->m_databaseConnections;

    if (connections.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No database connections available.");
        ImGui::Text("Please create a connection using the Database Connection window.");
        ImGui::End();
        return;
    }

    // Static state for the schema viewer
    static int selectedConnectionIndex = -1;
    static int selectedTableIndex = -1;
    static std::vector<Database::Database::DatabaseObjectInfo> availableTables;
    static std::shared_ptr<Database::Table> currentTableSchema;
    static std::string currentTableName;
    static bool tablesLoaded = false;
    static bool schemaLoaded = false;
    static bool needToLoadSchema = false;  // Defer schema loading to avoid flickering

    // Connection selection dropdown
    ImGui::Text("Select Database Connection:");
    ImGui::SetNextItemWidth(400);

    std::vector<std::string> connectionNames;
    for (const auto& conn : connections)
    {
        std::string displayName = conn->GetConnectionName();
        if (displayName.empty())
        {
            displayName = "Unnamed Connection";
        }
        if (conn->IsConnected())
            displayName += " (Connected)";
        else
            displayName += " (Disconnected)";
        connectionNames.push_back(displayName);
    }

    // Create combo items
    std::string currentConnectionPreview = selectedConnectionIndex >= 0 ? 
        connectionNames[selectedConnectionIndex] : "Select a connection...";

    if (ImGui::BeginCombo("##ConnectionSelect", currentConnectionPreview.c_str()))
    {
        for (int i = 0; i < static_cast<int>(connectionNames.size()); ++i)
        {
            bool isSelected = (selectedConnectionIndex == i);
            if (ImGui::Selectable(connectionNames[i].c_str(), isSelected))
            {
                if (selectedConnectionIndex != i)
                {
                    selectedConnectionIndex = i;
                    selectedTableIndex = -1;
                    availableTables.clear();
                    currentTableSchema.reset();
                    currentTableName.clear();
                    tablesLoaded = false;
                    schemaLoaded = false;
                    needToLoadSchema = false;
                }
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (selectedConnectionIndex < 0 || selectedConnectionIndex >= static_cast<int>(connections.size()))
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Please select a database connection.");
        ImGui::End();
        return;
    }

    auto selectedConnection = connections[selectedConnectionIndex];
    if (!selectedConnection->IsConnected())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "Selected connection is not connected.");
        ImGui::Text("Please connect to the database first.");
        ImGui::End();
        return;
    }

    auto db = selectedConnection->GetDatabase();
    if (!db)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Unable to access database.");
        ImGui::End();
        return;
    }

    ImGui::Separator();

    // Load tables if not already loaded
    if (!tablesLoaded)
    {
        availableTables = db->GetDatabaseObjects("TABLE");
        tablesLoaded = true;
        LOG_INFO("Loaded " + std::to_string(availableTables.size()) + " tables for schema viewer");
    }

    // Table selection dropdown
    ImGui::Text("Select Table:");
    ImGui::SetNextItemWidth(400);

    std::string currentTablePreview = selectedTableIndex >= 0 ? 
        availableTables[selectedTableIndex].Name : "Select a table...";

    if (ImGui::BeginCombo("##TableSelect", currentTablePreview.c_str()))
    {
        for (int i = 0; i < static_cast<int>(availableTables.size()); ++i)
        {
            bool isSelected = (selectedTableIndex == i);
            if (ImGui::Selectable(availableTables[i].Name.c_str(), isSelected))
            {
                if (selectedTableIndex != i)
                {
                    selectedTableIndex = i;
                    currentTableName = availableTables[i].Name;
                    schemaLoaded = false;
                    needToLoadSchema = true;  // Defer loading until after combo closes
                }
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Load schema after combo closes to avoid flickering
    if (needToLoadSchema)
    {
        needToLoadSchema = false;
        currentTableSchema = db->GetTable(currentTableName);
        if (currentTableSchema && currentTableSchema->LoadSchema())
        {
            schemaLoaded = true;
            LOG_INFO("Loaded schema for table: " + currentTableName);
        }
        else
        {
            LOG_ERROR("Failed to load schema for table: " + currentTableName);
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Refresh Tables"))
    {
        availableTables = db->GetDatabaseObjects("TABLE");
        selectedTableIndex = -1;
        currentTableSchema.reset();
        currentTableName.clear();
        schemaLoaded = false;
        needToLoadSchema = false;
        LOG_INFO("Refreshed table list");
    }

    ImGui::Separator();

    // Display schema if loaded
    if (selectedTableIndex < 0)
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Please select a table to view its schema.");
        ImGui::End();
        return;
    }

    // Show loading indicator while schema is being loaded
    if (needToLoadSchema || (!schemaLoaded && !currentTableSchema))
    {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loading schema for table: %s", currentTableName.c_str());
        ImGui::Spacing();
        ImGui::Text("Please wait...");
        ImGui::End();
        return;
    }

    if (!schemaLoaded || !currentTableSchema)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "Failed to load schema for table: %s", currentTableName.c_str());
        ImGui::Spacing();
        ImGui::Text("Check the log window for detailed error information.");
        ImGui::End();
        return;
    }

    // Display table name
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Table: %s", currentTableName.c_str());

    // Show database type information
    std::string driverName = selectedConnection->GetDriverName();
    if (driverName.find("Access") != std::string::npos)
    {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "(MS Access)");
    }

    ImGui::Separator();

    const auto& fields = currentTableSchema->GetFields();
    const auto& indexes = currentTableSchema->GetIndexes();
    const auto& foreignKeys = currentTableSchema->GetForeignKeys();

    if (fields.empty())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No column information could be loaded.");
        ImGui::Spacing();
        ImGui::Text("Possible reasons:");
        ImGui::BulletText("ODBC driver may not support catalog functions");
        ImGui::BulletText("Table name may contain special characters");
        ImGui::BulletText("Insufficient permissions to read table metadata");
        ImGui::Spacing();
        ImGui::Text("Last error: %s", 
            currentTableSchema->GetLastError().empty() ? "None reported" : currentTableSchema->GetLastError().c_str());
    }
    else
    {
        // Tab bar for different schema information
        if (ImGui::BeginTabBar("SchemaViewerTabBar"))
        {
            // Columns tab
            if (ImGui::BeginTabItem("Columns"))
            {
                ImGui::Text("Total Columns: %d", static_cast<int>(fields.size()));
                ImGui::Separator();

                if (ImGui::BeginTable("SchemaColumnsTable", 7, 
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                    ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, 
                    ImVec2(0, ImGui::GetContentRegionAvail().y - 10)))
                {
                    ImGui::TableSetupColumn("Column Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                    ImGui::TableSetupColumn("Data Type", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                    ImGui::TableSetupColumn("Nullable", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                    ImGui::TableSetupColumn("PK", ImGuiTableColumnFlags_WidthFixed, 40.0f);
                    ImGui::TableSetupColumn("Auto Inc", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                    ImGui::TableSetupColumn("Default", ImGuiTableColumnFlags_WidthStretch);
                    ImGui::TableHeadersRow();

                    for (const auto& field : fields)
                    {
                        ImGui::TableNextRow();

                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("%s", field.GetName().c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%s", Database::FieldTypeToString(field.GetType()).c_str());

                        ImGui::TableSetColumnIndex(2);
                        if (field.GetSize() > 0)
                            ImGui::Text("%d", field.GetSize());
                        else
                            ImGui::Text("-");

                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("%s", field.IsNullable() ? "Yes" : "No");

                        ImGui::TableSetColumnIndex(4);
                        if (field.IsPrimaryKey())
                            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "YES");
                        else
                            ImGui::Text("-");

                        ImGui::TableSetColumnIndex(5);
                        ImGui::Text("%s", field.IsAutoIncrement() ? "Yes" : "-");

                        ImGui::TableSetColumnIndex(6);
                        ImGui::Text("%s", field.GetDefaultValue().empty() ? "-" : field.GetDefaultValue().c_str());
                    }

                    ImGui::EndTable();
                }
                ImGui::EndTabItem();
            }

            // Indexes tab
            if (ImGui::BeginTabItem("Indexes"))
            {
                ImGui::Text("Total Indexes: %d", static_cast<int>(indexes.size()));
                ImGui::Separator();

                if (indexes.empty())
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No index information available.");
                }
                else
                {
                    if (ImGui::BeginTable("SchemaIndexesTable", 5, 
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, 
                        ImVec2(0, ImGui::GetContentRegionAvail().y - 10)))
                    {
                        ImGui::TableSetupColumn("Index Name", ImGuiTableColumnFlags_WidthFixed, 200.0f);
                        ImGui::TableSetupColumn("Columns", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableSetupColumn("Unique", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                        ImGui::TableSetupColumn("Primary Key", ImGuiTableColumnFlags_WidthFixed, 90.0f);
                        ImGui::TableSetupColumn("Clustered", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                        ImGui::TableHeadersRow();

                        for (const auto& index : indexes)
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%s", index.Name.c_str());

                            ImGui::TableSetColumnIndex(1);
                            std::string cols;
                            for (size_t i = 0; i < index.Columns.size(); ++i)
                            {
                                if (i > 0) cols += ", ";
                                cols += index.Columns[i];
                            }
                            ImGui::Text("%s", cols.c_str());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", index.IsUnique ? "Yes" : "No");

                            ImGui::TableSetColumnIndex(3);
                            if (index.IsPrimaryKey)
                                ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "YES");
                            else
                                ImGui::Text("No");

                            ImGui::TableSetColumnIndex(4);
                            ImGui::Text("%s", index.IsClustered ? "Yes" : "No");
                        }

                        ImGui::EndTable();
                    }
                }
                ImGui::EndTabItem();
            }

            // Foreign Keys tab
            if (ImGui::BeginTabItem("Foreign Keys"))
            {
                ImGui::Text("Total Foreign Keys: %d", static_cast<int>(foreignKeys.size()));
                ImGui::Separator();

                if (foreignKeys.empty())
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.2f, 1.0f), "No foreign key information available.");
                }
                else
                {
                    if (ImGui::BeginTable("SchemaForeignKeysTable", 6, 
                        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                        ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, 
                        ImVec2(0, ImGui::GetContentRegionAvail().y - 10)))
                    {
                        ImGui::TableSetupColumn("FK Name", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                        ImGui::TableSetupColumn("Column", ImGuiTableColumnFlags_WidthFixed, 120.0f);
                        ImGui::TableSetupColumn("Referenced Table", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                        ImGui::TableSetupColumn("Referenced Column", ImGuiTableColumnFlags_WidthFixed, 150.0f);
                        ImGui::TableSetupColumn("On Delete", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                        ImGui::TableSetupColumn("On Update", ImGuiTableColumnFlags_WidthStretch);
                        ImGui::TableHeadersRow();

                        for (const auto& fk : foreignKeys)
                        {
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("%s", fk.Name.c_str());

                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%s", fk.Column.c_str());

                            ImGui::TableSetColumnIndex(2);
                            ImGui::Text("%s", fk.ReferencedTable.c_str());

                            ImGui::TableSetColumnIndex(3);
                            ImGui::Text("%s", fk.ReferencedColumn.c_str());

                            ImGui::TableSetColumnIndex(4);
                            ImGui::Text("%s", fk.OnDelete.c_str());

                            ImGui::TableSetColumnIndex(5);
                            ImGui::Text("%s", fk.OnUpdate.c_str());
                        }

                        ImGui::EndTable();
                    }
                }
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }

    ImGui::End();
}
