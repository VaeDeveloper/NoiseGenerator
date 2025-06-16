#include "SettingsManager.h"
#include "Logger/Logger.h"


SettingsManager& SettingsManager::Get()
{
	static SettingsManager instance;
	return instance;
}

bool SettingsManager::Load(const std::string& path) 
{
	reader = INIReader(path);
	if(reader.ParseError() < 0) 
	{
		Logger::Err("Failed to load config file: " + path);
		return false;
	}

	Logger::Log("Loaded config: " + path);
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