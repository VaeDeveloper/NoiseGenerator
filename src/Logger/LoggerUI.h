#pragma once

#include "Logger.h"
#include "imgui.h"
#include "GUI/IconRegistry.h"

inline void DrawLoggerWindow(bool* open = nullptr)
{
	ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;

	if(!ImGui::Begin("Output Log", open, windowFlags))
	{
		ImGui::End();
		return;
	}

	// === Filter Section ===
	static int currentTypeFilter = 0;
	static const char* typeOptions[] = { "All", "Info", "Warning", "Error" };

	static std::string currentCategory = "All";
	static std::vector<std::string> knownCategories;

	// Rebuild category list (can be cached/optimized later)
	knownCategories.clear();
	knownCategories.push_back("All");
	for(const auto& entry : Logger::GetMessages())
	{
		if(std::find(knownCategories.begin(), knownCategories.end(), entry.category) == knownCategories.end())
		{
			knownCategories.push_back(entry.category);
		}
	}

	// Type filter
	ImGui::Text("Type:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(100);
	ImGui::Combo("##LogTypeFilter", &currentTypeFilter, typeOptions, IM_ARRAYSIZE(typeOptions));
	ImGui::SameLine();

	// === Category filter with checkboxes inside combo ===
	static std::unordered_map<std::string, bool> categoryFilterStates;

	for(const auto& entry : Logger::GetMessages()) 
	{
		if(categoryFilterStates.find(entry.category) == categoryFilterStates.end()) 
		{
			categoryFilterStates[entry.category] = false; // off default
		}
	}

	int selectedCount = 0;
	for(const auto& [_, enabled] : categoryFilterStates) 
	{
		if(enabled) selectedCount++;
	}

	std::string comboLabel = " -- select --";

	ImGui::Text("Categories:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	if(ImGui::BeginCombo("##CategoryCombo", comboLabel.c_str())) {


		if(ImGui::Button("Reset",ImVec2(110,20)))
		{
			for(auto& [_, enabled] : categoryFilterStates) enabled = false;
		}

		ImGui::Separator();

		for(auto& [category, enabled] : categoryFilterStates) 
		{
			ImGui::Checkbox(category.c_str(), &enabled);
		}

		ImGui::EndCombo();
	}


	// === Text Search ===
	static char searchBuffer[256] = "";
	ImGui::Text("Search:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);
	ImGui::InputText("##SearchFilter", searchBuffer, IM_ARRAYSIZE(searchBuffer));
	std::string searchFilter = searchBuffer;

	// === Log entries ===
	const auto& logs = Logger::GetMessages();

	ImGui::BeginChild("LogScrollRegion", ImVec2(0, -40), true, ImGuiWindowFlags_HorizontalScrollbar);
	for(const auto& entry : logs)
	{
		// Apply type filter
		if(currentTypeFilter != 0 && static_cast<int>(entry.verbosity) != currentTypeFilter - 1)
			continue;

		// Apply category filter
		bool anyCategoryChecked = false;
		for(const auto& [_, enabled] : categoryFilterStates) 
		{
			if(enabled) 
			{
				anyCategoryChecked = true;
				break;
			}
		}

		if(anyCategoryChecked) 
		{
			if(categoryFilterStates.find(entry.category) != categoryFilterStates.end() && !categoryFilterStates[entry.category])
				continue;
		}

		// Apply text filter
		if(!searchFilter.empty() && entry.message.find(searchFilter) == std::string::npos)
			continue;

		ImVec4 color;
		switch(entry.verbosity)
		{
		case LogVerbosity::Info:    color = ImVec4(1, 1, 1, 1); break;
		case LogVerbosity::Warning: color = ImVec4(1, 1, 0.6f, 1); break;
		case LogVerbosity::Error:   color = ImVec4(1, 0.4f, 0.4f, 1); break;
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

	// === Buttons ===
	if(ImGui::Button(WITH_ICON("Save", "Save to File")))
	{
		try
		{
			Logger::SaveLogToFile();
			Logger::Log("System", LogVerbosity::Info, "Log saved to " + Logger::filePath);
		}
		catch(const std::exception& e)
		{
			Logger::Log("System", LogVerbosity::Error, e.what());
		}
	}

	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("Trash", "Clear")))
	{
		Logger::Clear();
	}

	ImGui::End();
}

