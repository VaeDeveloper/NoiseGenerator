#include "Logger.h"


std::vector<LogEntry> Logger::messages;
std::string Logger::filePath = "log.txt";


std::string CenterText(const std::string& text, size_t width)
{
	if(text.length() >= width)
		return text;

	size_t totalPadding = width - text.length();
	size_t padLeft = totalPadding / 2;
	size_t padRight = totalPadding - padLeft;

	return std::string(padLeft, ' ') + text + std::string(padRight, ' ');
}

std::string Logger::GetTimestamp()
{
	auto now = std::chrono::system_clock::now();
	std::time_t now_c = std::chrono::system_clock::to_time_t(now);

	std::tm local_tm;
#ifdef _WIN32
	localtime_s(&local_tm, &now_c);
#else
	localtime_r(&now_c, &local_tm);
#endif

	char buffer[30];
	if(std::strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M:%S", &local_tm) == 0)
	{
		return "??-???-???? ??:??:??";
	}
	return std::string(buffer);
}

void Logger::Log(const std::string& category, LogVerbosity verbosity, const std::string& message)
{
	std::string prefix;
	switch(verbosity)
	{
	case LogVerbosity::Info:    prefix = "LOGGING"; break;
	case LogVerbosity::Warning: prefix = "WARNING"; break;
	case LogVerbosity::Error:   prefix = " ERROR "; break;
	}

	std::ostringstream oss;
	std::string centeredCategory = CenterText(category, 16); 

	oss << prefix << ": "
		<< "[" << centeredCategory << "]    "
		<< "[" << GetTimestamp() << "] "
		<< message;

	messages.emplace_back(LogEntry{ verbosity, category, oss.str() });
}

void Logger::SaveLogToFile()
{
	std::ofstream file(filePath, std::ios::trunc);
	if(!file.is_open())
	{
		Logger::Log("Logger", LogVerbosity::Error, "Failed to open log file: " + filePath);
		throw std::runtime_error("Failed to open log file: " + filePath);
	}

	for(const auto& entry : messages)
	{
		file << entry.message << "\n";
	}
}

void Logger::Clear()
{
	messages.clear();
}

const std::vector<LogEntry>& Logger::GetMessages()
{
	return messages;
}

std::vector<LogEntry> Logger::GetMessagesByVerbosity(LogVerbosity verbosity)
{
	std::vector<LogEntry> result;
	for(const auto& entry : messages)
	{
		if(entry.verbosity == verbosity)
			result.push_back(entry);
	}
	return result;
}