#include "MenuBarModel.h"

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
