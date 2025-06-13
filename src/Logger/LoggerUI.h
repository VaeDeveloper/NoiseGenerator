#pragma once

#include "Logger.h"
#include "imgui.h"
#include "ThirdParty/IconsFontAwesome5.h"
inline void DrawLoggerWindow(bool* open = nullptr)
{
	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

	if(!ImGui::Begin("Output Log", open, windowFlags)) 
	{
		ImGui::End();
		return;
	}

	const auto& logs = Logger::GetMessages();

	ImGui::BeginChild("LogScrollRegion", ImVec2(0, -40), true, ImGuiWindowFlags_HorizontalScrollbar);
	for(const auto& entry : logs) 
	{
		ImVec4 color;
		switch(entry.type) 
		{
			case LogType::Info:    color = ImVec4(1, 1, 1, 1); break;
			case LogType::Warning: color = ImVec4(1, 1, 0.6f, 1); break;
			case LogType::Error:   color = ImVec4(1, 0.4f, 0.4f, 1); break;
		}

		ImGui::PushStyleColor(ImGuiCol_Text, color);
		ImGui::TextWrapped("%s", entry.message.c_str());
		ImGui::PopStyleColor();
	}

	if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f) 
	{
		ImGui::SetScrollHereY(1.0f);
	}
	ImGui::EndChild();

	if(ImGui::Button(ICON_FA_SAVE " Save to File"))
	{
		try
		{
			Logger::SaveLogToFile();
			Logger::Log("Log saved to " + Logger::filePath);
		}
		catch(const std::exception& e)
		{
			Logger::Err(e.what());
		}
	}

	ImGui::SameLine();

	if(ImGui::Button(ICON_FA_TRASH " Clear"))
	{
		Logger::Clear();
	}

	ImGui::End();
}
