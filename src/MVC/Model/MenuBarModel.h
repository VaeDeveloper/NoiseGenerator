#pragma once 

class MenuBarModel
{
public:
	bool IsFullscreen() const { return bFullscreen; }
	void SetFullscreen(bool bEnable) { bFullscreen = bEnable; }
	bool IsInfoPanelVisible() const { return bShowInfoPanel; }
	void SetInfoPanelVisible(bool value) { bShowInfoPanel = value; }
private:

	bool bFullscreen = false;
	bool bShowInfoPanel = true;
};