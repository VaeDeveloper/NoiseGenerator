#pragma once 

#include "IModel.h"

/**
 * Model class representing the state of the application's menu bar.
 *
 * Stores and manages state information such as fullscreen mode and
 * the visibility of the information panel.
 */
class MenuBarModel : public IModel
{
public:
	virtual std::string GetId() const override;
	virtual void Reset() {};

	/**
	 * Checks whether the application is in fullscreen mode.
	 * 
	 * @return True if fullscreen is enabled, false otherwise.
	 */
	bool IsFullscreen() const;

	/**
	 * Sets the fullscreen mode.
	 * 
	 * @param bEnable Whether to enable fullscreen mode.
	 */
	void SetFullscreen(bool bEnable);

	/**
	 * Sets the visibility of the info panel.
	 * 
	 * @param value Whether the info panel should be visible.
	 */
	bool IsInfoPanelVisible() const;
	
	/**
	 * Checks whether the info panel is visible.
	 * 
	 * @return True if the info panel is visible, false otherwise.
	 */
	void SetInfoPanelVisible(bool value);

private:
	/** Whether fullscreen mode is currently enabled. */
	bool bFullscreen = false;

	/** Whether the information panel is currently shown. */
	bool bShowInfoPanel = true;
};