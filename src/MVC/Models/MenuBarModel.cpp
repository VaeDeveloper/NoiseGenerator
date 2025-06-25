#include "MenuBarModel.h"

std::string MenuBarModel::GetId() const
{
	return "menu_bar_model";
}

bool MenuBarModel::IsFullscreen() const
{
	return bFullscreen;
}

void MenuBarModel::SetFullscreen(bool bEnable)
{
	bFullscreen = bEnable;
}

bool MenuBarModel::IsInfoPanelVisible() const
{
	return bShowInfoPanel;
}

void MenuBarModel::SetInfoPanelVisible(bool value)
{
	bShowInfoPanel = value;
}
