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

	const float WindowWidth = 1280;
	const float WindowHeight = 880;

	bool InitGLFW();
	bool InitOpenGL();
	void RenderScene();
};