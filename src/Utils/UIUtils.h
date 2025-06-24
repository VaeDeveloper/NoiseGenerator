#pragma once 

#include "Logger/LoggerMacro.h"
#include "GUI/IconRegistry.h"
#include <unordered_map>
#include "imgui.h"
#include "Utils/StringUtils.h"
#include "Utils/Constants.h"
DECLARE_LOG_CATEGORY_EXTERN(LogGUI);



namespace NG
{
	constexpr int spaceOffset = 24;

	template<typename WidgetFunc>
	void LabeledWidgetWithLock(const char* lockID, bool* lockState, WidgetFunc widget)
	{
		ImVec2 originalPadding = ImGui::GetStyle().FramePadding;
		float targetHeight = 22.0f;
		float lineHeight = ImGui::GetTextLineHeight();
		float newPaddingY = (targetHeight - lineHeight) * 0.5f;
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(originalPadding.x, newPaddingY));

		if(*lockState) ImGui::BeginDisabled(true);
		widget();
		if(*lockState) ImGui::EndDisabled();

		ImGui::PopStyleVar();
		ImGui::SameLine();
		float space = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space - spaceOffset);

		ImGui::PushID(lockID);
		if(ImGui::Button(*lockState ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN, ImVec2(24, 22)))
		{
			*lockState = !*lockState;
			NGLOG(LogGUI, Info, std::string("Lock toggled: ")
				+ lockID + " = " + (*lockState ? "true" : "false"));
		}
		ImGui::PopID();
	}

	template<typename T>
	void LogWidget(const char* label, T* value, std::function<bool()> widget)
	{
		if(widget())
		{
			NGLOG(LogGUI, Info, std::string(label) + " = " + NG::StringUtils::ToString(*value));
		}
	}

	template<typename IndexType>
	void LogWidgetComboWithNames(const char* label, IndexType* index, const char* const* names, int count)
	{
		if(ImGui::Combo(label, index, names, count))
		{
			const char* name = (*index >= 0 && *index < count) ? names[*index] : "Unknown";
			NGLOG(LogGUI, Info, std::string(label) + " = " + name);
		}
	}


	inline void ShowCustomTooltipWithDelay(float delaySeconds, std::function<void()> content)
	{
		static std::unordered_map<ImGuiID, float> hoverTimers;

		ImGuiID itemId = ImGui::GetItemID();
		if(itemId == 0) return;

		if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		{
			hoverTimers[itemId] += ImGui::GetIO().DeltaTime;
			if(hoverTimers[itemId] >= delaySeconds)
			{
				// ћ€гкий голубой фон и почти белый текст
				ImVec4 bgColor = ImVec4(0.3f, 0.5f, 0.8f, 1.0f);  // светло-голубой
				ImVec4 textColor = ImVec4(0.95f, 0.95f, 0.98f, 1.0f); // почти белый

				ImGui::PushStyleColor(ImGuiCol_PopupBg, bgColor);
				ImGui::PushStyleColor(ImGuiCol_Text, textColor);

				ImGui::BeginTooltip();
				content();
				ImGui::EndTooltip();

				ImGui::PopStyleColor(2);
			}
		}
		else
		{
			hoverTimers[itemId] = 0.0f;
		}
	}


	inline void ShowTooltipFromLines(const std::vector<std::string>& lines,float delaySeconds = 0.6f)
	{
		if(lines.empty()) return;

		ShowCustomTooltipWithDelay(delaySeconds, [&] ()
			{
				ImGui::TextUnformatted(lines[0].c_str());

				if(lines.size() > 1)
				{
					ImGui::Separator();
					for(size_t i = 1; i < lines.size(); ++i)
					{
						ImGui::TextUnformatted(lines[i].c_str());
					}
				}
			});
	}

	inline void ShowShiftOnlyTooltip(const std::vector<std::string>& lines,float delaySeconds = 0.6f)
	{
		const ImGuiIO& io = ImGui::GetIO();
		if(ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled) && io.KeyShift)
		{
			ShowTooltipFromLines(lines, delaySeconds);
		}
	}
}