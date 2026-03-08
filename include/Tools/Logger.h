#pragma once

#include <string>
#include <vector>
#include <set>
#include <mutex>
#include <chrono>
#include <sstream>
#include <iomanip>

// Log message severity levels
enum class LogLevel
{
    Info,
    Warning,
    Error,
    Debug
};

// Individual log entry
struct LogEntry
{
    std::string timestamp;
    LogLevel level;
    std::string message;
    std::string source;  // Optional: which component generated the log

    LogEntry(LogLevel lvl, const std::string& msg, const std::string& src = "")
        : level(lvl), message(msg), source(src)
    {
        // Generate timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::ostringstream oss;
        std::tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &time);
#else
        localtime_r(&time, &tm_buf);
#endif
        oss << std::put_time(&tm_buf, "%H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        timestamp = oss.str();
    }
};

// Singleton Logger class
class Logger
{
public:
    static Logger& GetInstance()
    {
        static Logger instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Log methods
    void Info(const std::string& message, const std::string& source = "");
    void Warning(const std::string& message, const std::string& source = "");
    void Error(const std::string& message, const std::string& source = "");
    void Debug(const std::string& message, const std::string& source = "");

    // Get all log entries
    std::vector<LogEntry> GetLogs() const;

    // Get filtered logs
    std::vector<LogEntry> GetFilteredLogs(bool showInfo, bool showWarning, bool showError, bool showDebug) const;

    // Get filtered logs by level and source
    std::vector<LogEntry> GetFilteredLogs(bool showInfo, bool showWarning, bool showError, bool showDebug,
                                           const std::set<std::string>& enabledSources) const;

    // Get all unique sources from logs
    std::set<std::string> GetUniqueSources() const;

    // Clear all logs
    void Clear();

    // Get log count by level
    int GetInfoCount() const;
    int GetWarningCount() const;
    int GetErrorCount() const;
    int GetDebugCount() const;
    int GetTotalCount() const;

    // Set maximum log entries (to prevent memory overflow)
    void SetMaxEntries(size_t max) { m_maxEntries = max; }
    size_t GetMaxEntries() const { return m_maxEntries; }

private:
    Logger() : m_maxEntries(1000) {}
    ~Logger() {}

    void AddLog(LogLevel level, const std::string& message, const std::string& source);

    mutable std::mutex m_mutex;
    std::vector<LogEntry> m_logs;
    size_t m_maxEntries;
};

// Convenience macros for logging
#define LOG_INFO(msg) Logger::GetInstance().Info(msg, __FUNCTION__)
#define LOG_WARNING(msg) Logger::GetInstance().Warning(msg, __FUNCTION__)
#define LOG_ERROR(msg) Logger::GetInstance().Error(msg, __FUNCTION__)
#define LOG_DEBUG(msg) Logger::GetInstance().Debug(msg, __FUNCTION__)

// Macros with custom source
#define LOG_INFO_SRC(msg, src) Logger::GetInstance().Info(msg, src)
#define LOG_WARNING_SRC(msg, src) Logger::GetInstance().Warning(msg, src)
#define LOG_ERROR_SRC(msg, src) Logger::GetInstance().Error(msg, src)
#define LOG_DEBUG_SRC(msg, src) Logger::GetInstance().Debug(msg, src)
