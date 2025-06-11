#pragma once
#include <glad/glad.h>
#include "GUI/GuiManager.h"

class NGApplication
{
public:
	NGApplication();
	~NGApplication();

	bool Init();
	void Run();
	void Shutdown();

private:
	GLFWwindow* window = nullptr;
	GuiManager gui;

	bool InitGLFW();
	bool InitOpenGL();
	void RenderScene();
};