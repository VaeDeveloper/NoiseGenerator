#pragma once 

#include "Logger/LoggerMacro.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGUI);

namespace NG
{
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
			NGLOG(LogGUI, Info, std::string("Lock toggled: ") + lockID + " = " + (*lockState ? "true" : "false"));
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
}