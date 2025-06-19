#include "SettingsManager.h"
#include "Logger/LoggerMacro.h"
#include <filesystem>
#include "Utils/FileUtils.h"

DEFINE_LOG_CATEGORY(LogSettings);



SettingsManager& SettingsManager::Get()
{
	static SettingsManager instance;
	return instance;
}

bool SettingsManager::Load(const std::string& path) 
{
	auto configPath = NG::FileSystem::FindPath("config/settings.ini");
	if(configPath.empty()) 
	{
		NGLOG(LogSettings, Error, "Config file not found after searching parent folders.");
		return false;
	}
	reader = INIReader(configPath);

	if(reader.ParseError() < 0) 
	{
		NGLOG(LogSettings, Error, "Failed to load config file: " + path);
		return false;
	}

	NGLOG(LogSettings, Info, "Loaded config: " + path);
	return true;
}

int SettingsManager::GetWindowWidth() const 
{
	return reader.GetInteger("window", "width", 1280);
}

int SettingsManager::GetWindowHeight() const 
{
	return reader.GetInteger("window", "height", 720);
}

float SettingsManager::GetFontSize() const 
{
	return static_cast<float>(reader.GetReal("imgui", "font_size", 12.0));
}

std::string SettingsManager::GetTheme() const 
{
	return reader.Get("imgui", "theme", "dark");
}