#include "Logger.h"

std::vector<LogEntry> Logger::messages;
std::string Logger::filePath = "log.txt";

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    char buffer[30];
    std::strftime(buffer, sizeof(buffer), "%d-%b-%Y %H:%M:%S", std::localtime(&now_c));
    return std::string(buffer);
}

void Logger::Log(const std::string& message) {
    std::string full = "LOG: [" + GetTimestamp() + "] " + message;
    std::cout << "\x1B[32m" << full << "\033[0m" << std::endl;
    messages.push_back({ LogType::Info, full });
}

void Logger::Warn(const std::string& message) {
    std::string full = "WARN: [" + GetTimestamp() + "] " + message;
    std::cout << "\x1B[33m" << full << "\033[0m" << std::endl;
    messages.push_back({ LogType::Warning, full });
}

void Logger::Err(const std::string& message) {
    std::string full = "ERROR: [" + GetTimestamp() + "] " + message;
    std::cerr << "\x1B[91m" << full << "\033[0m" << std::endl;
    messages.push_back({ LogType::Error, full });
}

void Logger::SaveLogToFile() {
    std::ofstream file(filePath, std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + filePath);
    }

    for (const auto& entry : messages) {
        file << entry.message << "\n";
    }
    file.close();
}

void Logger::Clear() {
    messages.clear();
}

const std::vector<LogEntry>& Logger::GetMessages() {
    return messages;
}