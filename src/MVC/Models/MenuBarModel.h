#pragma once 

#include "../IModel.h"

class MenuBarModel : public IModel
{
public:
#pragma region IModelInterface
	virtual std::string GetId() const override
	{
		return "menu_bar_model";
	}

	virtual void Reset() {};

	// Serialization from JSON 
	virtual std::string SerializeToJson() const { return "{}"; }
	virtual void LoadFromJson(const std::string&) {}
#pragma endregion

	bool IsFullscreen() const;
	void SetFullscreen(bool bEnable);
	bool IsInfoPanelVisible() const;
	void SetInfoPanelVisible(bool value);
private:

	bool bFullscreen = false;
	bool bShowInfoPanel = true;
};