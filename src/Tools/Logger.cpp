#include "pch.h"
#include "Tools/Logger.h"

void Logger::Info(const std::string& message, const std::string& source)
{
    AddLog(LogLevel::Info, message, source);
}

void Logger::Warning(const std::string& message, const std::string& source)
{
    AddLog(LogLevel::Warning, message, source);
}

void Logger::Error(const std::string& message, const std::string& source)
{
    AddLog(LogLevel::Error, message, source);
}

void Logger::Debug(const std::string& message, const std::string& source)
{
    AddLog(LogLevel::Debug, message, source);
}

void Logger::AddLog(LogLevel level, const std::string& message, const std::string& source)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_logs.emplace_back(level, message, source);
    
    // Enforce maximum entries limit
    if (m_logs.size() > m_maxEntries)
    {
        m_logs.erase(m_logs.begin());
    }
}

std::vector<LogEntry> Logger::GetLogs() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_logs;
}

std::vector<LogEntry> Logger::GetFilteredLogs(bool showInfo, bool showWarning, bool showError, bool showDebug) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<LogEntry> filtered;
    
    for (const auto& entry : m_logs)
    {
        bool include = false;
        switch (entry.level)
        {
            case LogLevel::Info:    include = showInfo; break;
            case LogLevel::Warning: include = showWarning; break;
            case LogLevel::Error:   include = showError; break;
            case LogLevel::Debug:   include = showDebug; break;
        }

        if (include)
        {
            filtered.push_back(entry);
        }
    }
    
    return filtered;
}

std::vector<LogEntry> Logger::GetFilteredLogs(bool showInfo, bool showWarning, bool showError, bool showDebug,
                                               const std::set<std::string>& enabledSources) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<LogEntry> filtered;

    for (const auto& entry : m_logs)
    {
        // Check log level
        bool includeLevelCheck = false;
        switch (entry.level)
        {
            case LogLevel::Info:    includeLevelCheck = showInfo; break;
            case LogLevel::Warning: includeLevelCheck = showWarning; break;
            case LogLevel::Error:   includeLevelCheck = showError; break;
            case LogLevel::Debug:   includeLevelCheck = showDebug; break;
        }

        // Check source - only include if source is in the enabled list
        bool includeSourceCheck = enabledSources.find(entry.source) != enabledSources.end();

        if (includeLevelCheck && includeSourceCheck)
        {
            filtered.push_back(entry);
        }
    }

    return filtered;
}

std::set<std::string> Logger::GetUniqueSources() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::set<std::string> sources;

    for (const auto& entry : m_logs)
    {
        if (!entry.source.empty())
        {
            sources.insert(entry.source);
        }
    }

    return sources;
}

void Logger::Clear()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logs.clear();
}

int Logger::GetInfoCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& entry : m_logs)
    {
        if (entry.level == LogLevel::Info)
            count++;
    }
    return count;
}

int Logger::GetWarningCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& entry : m_logs)
    {
        if (entry.level == LogLevel::Warning)
            count++;
    }
    return count;
}

int Logger::GetErrorCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& entry : m_logs)
    {
        if (entry.level == LogLevel::Error)
            count++;
    }
    return count;
}

int Logger::GetDebugCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& entry : m_logs)
    {
        if (entry.level == LogLevel::Debug)
            count++;
    }
    return count;
}

int Logger::GetTotalCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_logs.size());
}
