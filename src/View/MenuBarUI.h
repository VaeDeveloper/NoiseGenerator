#pragma once 

// #include "Controller/MenuBarController.h"
#include <memory>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

class MenuBarController;

class MenuBarUI
{
public:
	void Initialize();
	void Draw();
	void SetTextureData(GLuint id, int w, int h);
	MenuBarController* GetController() const;
private:
	void DrawFileItem();
	void DrawViewItem();
	void DrawAboutItem();

	void ExportTooltipText(const std::vector<std::string>& lines);
	void ShowShiftOnlyTooltip(float delay, const std::vector<std::string>& lines);

	std::shared_ptr<MenuBarController> Controller;

	GLuint TextureId = 0;
	int TextureWidth = 0;
	int TextureHeight = 0;
};

