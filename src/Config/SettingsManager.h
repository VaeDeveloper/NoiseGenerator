#pragma once 

#include <string>
#include "INIReader.h"


/**
 * @brief Singleton class responsible for loading and providing application settings from a config file.
 */
class SettingsManager final
{
public:
	/**
	 * @brief Returns the singleton instance of the SettingsManager.
	 * @return Reference to the singleton instance.
	 */
	static SettingsManager& Get();

	/**
	 * @brief Loads settings from the specified configuration file.
	 * @param path Path to the .ini file. Defaults to "/config/settings.ini".
	 * @return true if the file was successfully loaded and parsed; false otherwise.
	 */
	bool Load(const std::string& path = "/config/settings.ini");

	/**
	 * @brief Returns the configured window width from settings.
	 * @return Integer value representing the window width in pixels.
	 */
	int GetWindowWidth() const;

	/**
	 * @brief Returns the configured window height from settings.
	 * @return Integer value representing the window height in pixels.
	 */
	int GetWindowHeight() const;

	/**
	 * @brief Returns the configured font size from settings.
	 * @return Floating point value of font size.
	 */
	float GetFontSize() const;

	/**
	 * @brief Returns the selected UI theme name.
	 * @return String representing the theme (e.g., "dark", "light").
	 */
	std::string GetTheme() const;

private:
	/**
	 * @brief Private constructor to enforce singleton pattern.
	 */
	SettingsManager() = default;

	/**
	 * @brief Internal INI file reader used to parse configuration settings.
	 */
	INIReader reader{ "config/settings.ini" };
};