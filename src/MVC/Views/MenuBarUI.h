#pragma once 

// #include "Controller/MenuBarController.h"
#include <memory>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include "IView.h"
#include "BoilerplateMacro.h"

#include "Delegates.h"

DECLARE_DELEGATE_ThreeParams(OnTextureParamChanged, unsigned int, int, int);

class MenuBarController;

class MenuBarUI : public IView, public IViewTyped<MenuBarController>
{
	IMPL_TYPED_CONTROLLER(MenuBarController, controller);
public:
	virtual void Initialize() override;
	virtual void Draw() override;
	
	void SetTextureData(GLuint id, int w, int h);

	OnTextureParamChanged TextureParamsChanged;
private:
	void DrawFileItem();
	void DrawViewItem();
	void DrawAboutItem();

	void ExportTooltipText(const std::vector<std::string>& lines);
	void ShowShiftOnlyTooltip(float delay, const std::vector<std::string>& lines);


	GLuint textureId = 0;
	int textureWidth = 0;
	int textureHeight = 0;

	std::shared_ptr<MenuBarController> controller;
};

