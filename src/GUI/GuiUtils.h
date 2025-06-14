#pragma once 
#include <string>

#define SHOW_HIDDEN_TAB_BAR(dock_id) \
    do { \
        if ((dock_id) != 0) { \
            if (ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id)) { \
                if (node->WantHiddenTabBarUpdate) node->WantHiddenTabBarToggle = true; \
            } \
        } \
    } while (0)


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
	}
}

