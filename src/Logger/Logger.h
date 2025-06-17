#pragma once

#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

 /**
  * Log verbosity level, similar to Unreal's ELogVerbosity.
  */
enum class LogVerbosity
{
	Info,
	Warning,
	Error
};

/**
 * Represents a log entry with category, verbosity and full formatted message.
 */
struct LogEntry
{
	LogVerbosity verbosity;
	std::string category;
	std::string message;
};

/**
 * Logger is a static utility class that provides categorized and leveled logging.
 */
class Logger
{
public:
	static void Log(const std::string& category, LogVerbosity verbosity, const std::string& message);
	static void SaveLogToFile();
	static void Clear();
	static const std::vector<LogEntry>& GetMessages();
	static std::vector<LogEntry> GetMessagesByVerbosity(LogVerbosity verbosity);

	static std::string filePath;

private:
	static std::vector<LogEntry> messages;
	static std::string GetTimestamp();
};

