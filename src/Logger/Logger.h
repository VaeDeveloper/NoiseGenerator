#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <ctime>

enum class LogType 
{
	Info,
	Warning,
	Error
};

struct LogEntry 
{
	LogType type;
	std::string message;
};

class Logger 
{
public:
	static void Log(const std::string& message);
	static void Warn(const std::string& message);
	static void Err(const std::string& message);

	static void SaveLogToFile();
	static void Clear();
	static const std::vector<LogEntry>& GetMessages();

	static std::string filePath;

private:
	static std::vector<LogEntry> messages;
	static std::string GetTimestamp();
};

