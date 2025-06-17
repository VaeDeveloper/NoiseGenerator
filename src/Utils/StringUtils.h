#pragma once

#include <string>

namespace NG
{
	namespace StringUtils
	{
		// Converts FString to itself (no-op)
		inline std::string ToString(const std::string& Value)
		{
			return Value;
		}

		// Converts const char* to std::string with null check
		inline std::string ToString(const char* Value)
		{
			return Value ? std::string(Value) : "null";
		}

		// Converts char* to std::string with null check
		inline std::string ToString(char* Value)
		{
			return Value ? std::string(Value) : "null";
		}

		// Converts bool to "true" or "false"
		inline std::string ToString(bool bValue)
		{
			return bValue ? "true" : "false";
		}

		// Converts a single character to a string
		inline std::string ToString(char Char)
		{
			return std::string(1, Char);
		}

		// Generic template for numeric types using std::to_string
		template<typename T>
		std::string ToString(T Value)
		{
			return std::to_string(Value);
		}

		inline std::string ToUpper(const std::string& str)
		{
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(),
				[] (unsigned char c) { return std::toupper(c); });
			return result;
		}

		inline bool EndsWith(const std::string& str, const std::string& suffix)
		{
			return str.size() >= suffix.size() &&
				str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
		}
	}
}