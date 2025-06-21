#include "IconRegistry.h"
#include "imgui.h"
#include "Logger/LoggerMacro.h"

DEFINE_LOG_CATEGORY(LogIcon);


const char* IconRegistry::Get(const std::string& Name)
{
	const auto& Map = GetMap();
	auto it = Map.find(Name);
	if(it != Map.end())
	{
		return it->second;
	}

	NGLOG(LogIcon, Warning, "Icon not found: " + Name);
	return "";
}

void IconRegistry::InitializeIcons(const ImGuiIO& io, float fontSize)
{
	ImFontConfig config;
	config.MergeMode = true;
	config.PixelSnapH = true;

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	std::string fontPath = NG::FileSystem::FindPath("fonts/fa-solid-900.ttf");
	if(fontPath.empty())
	{
		NGLOG(LogIcon, Error, "Font Awesome not found!");
		return;
	}

	if(!io.Fonts->AddFontFromFileTTF(fontPath.c_str(), fontSize, &config, icon_ranges))
	{
		NGLOG(LogIcon, Error, "Failed to load Font Awesome: " + fontPath);
	}
	else
	{
		NGLOG(LogIcon, Info, "Font Awesome loaded from: " + fontPath);
	}
}

inline const std::map<std::string, const char*>& IconRegistry::GetMap()
{
	return IconMap;
}
