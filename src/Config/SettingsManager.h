#pragma once 

#include <string>
#include "INIReader.h"


class SettingsManager final
{
public:
	static SettingsManager& Get();

	bool Load(const std::string& path = "../config/settings.ini");


	int GetWindowWidth() const;
	int GetWindowHeight() const;

	float GetFontSize() const;
	std::string GetTheme() const;

private:
	SettingsManager() = default;
	INIReader reader{ "config/settings.ini" };
};