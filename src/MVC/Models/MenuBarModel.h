#pragma once 

#include "IModel.h"

class MenuBarModel : public IModel
{
public:
	virtual std::string GetId() const override;
	virtual void Reset() {};
	virtual std::string SerializeToJson() const { return "{}"; }
	virtual void LoadFromJson(const std::string&) {}

	bool IsFullscreen() const;
	void SetFullscreen(bool bEnable);
	bool IsInfoPanelVisible() const;
	void SetInfoPanelVisible(bool value);
private:
	bool bFullscreen = false;
	bool bShowInfoPanel = true;
};