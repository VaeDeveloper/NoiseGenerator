#pragma once

#include <string>
#include <algorithm>
#include "nlohmann/json.hpp"
#include "NoiseTypes.h"

namespace NG
{
	namespace StringUtils
	{
		/**
			 * @brief Converts std::string to itself (no-op).
			 * @param Value The string to return.
			 * @return Same input string.
			 */
		inline std::string ToString(const std::string& Value)
		{
			return Value;
		}

		/**
		 * @brief Converts const char* to std::string with null check.
		 * @param Value Pointer to null-terminated character array.
		 * @return String or "null" if pointer is null.
		 */
		inline std::string ToString(const char* Value)
		{
			return Value ? std::string(Value) : "null";
		}

		/**
		 * @brief Converts char* to std::string with null check.
		 * @param Value Pointer to mutable character array.
		 * @return String or "null" if pointer is null.
		 */
		inline std::string ToString(char* Value)
		{
			return Value ? std::string(Value) : "null";
		}

		/**
		 * @brief Converts a boolean value to a string representation ("true" or "false").
		 * @param bValue Boolean value to convert.
		 * @return "true" or "false".
		 */
		inline std::string ToString(bool bValue)
		{
			return bValue ? "true" : "false";
		}

		/**
		 * @brief Converts a single character to a std::string.
		 * @param Char Character to convert.
		 * @return String containing a single character.
		 */
		inline std::string ToString(char Char)
		{
			return std::string(1, Char);
		}

		/**
		 * @brief Converts a numeric type to string using std::to_string.
		 * @tparam T Numeric type (int, float, double, etc.)
		 * @param Value The value to convert.
		 * @return String representation of the numeric value.
		 */
		template<typename T>
		std::string ToString(T Value)
		{
			return std::to_string(Value);
		}

		/**
		 * @brief Converts a string to uppercase.
		 * @param str The string to convert.
		 * @return A new string with all characters converted to uppercase.
		 */
		inline std::string ToUpper(const std::string& str)
		{
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(),
				[] (unsigned char c) { return std::toupper(c); });
			return result;
		}

		/**
		 * @brief Checks whether the given string ends with a specified suffix.
		 * @param str The string to check.
		 * @param suffix The suffix to match against.
		 * @return true if str ends with suffix; false otherwise.
		 */
		inline bool EndsWith(const std::string& str, const std::string& suffix)
		{
			return str.size() >= suffix.size() &&
				str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
		}

		/**
		 * @brief Converts a DistanceMetric enum to string via JSON serialization.
		 * @param val Enum value to convert.
		 * @return String representation of the enum.
		 */
		template<>
		inline std::string ToString<DistanceMetric>(DistanceMetric val) {
			return nlohmann::json(val).get<std::string>();
		}

		/**
		 * @brief Converts a CellularReturnType enum to string via JSON serialization.
		 * @param val Enum value to convert.
		 * @return String representation of the enum.
		 */
		template<>
		inline std::string ToString<CellularReturnType>(CellularReturnType val) {
			return nlohmann::json(val).get<std::string>();
		}

		/**
		 * @brief Safely converts enum or other values to string.
		 *        Falls back to integer representation if serialization fails.
		 * @tparam T Enum or value type.
		 * @param value Value to convert.
		 * @return String representation via JSON or fallback numeric format.
		 */
		template<typename T>
		std::string EnumToStringSafe(T value)
		{
			if constexpr(std::is_enum_v<T>)
			{
				try {
					return nlohmann::json(value).get<std::string>();
				}
				catch(...) {
					return "enum(" + std::to_string(static_cast<int>(value)) + ")";
				}
			}
			else
			{
				return NG::StringUtils::ToString(value);
			}
		}
	}
}