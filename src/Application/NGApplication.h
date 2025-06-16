#pragma once
#include <glad/glad.h>
#include "GUI/GuiManager.h"
#include "Logger/Logger.h"

enum class InitStatus
{
	Success,
	GLFW_InitFailed,
	WindowCreationFailed,
	OpenGL_InitFailed,
	GLAD_InitFailed
};

class NGApplication
{
public:
	NGApplication();
	~NGApplication();

	InitStatus Init();
	void Run();
	void Shutdown();

	static std::string StatusToString(InitStatus status);
private:
	GLFWwindow* window = nullptr;
	GuiManager gui;

	const float WindowWidth = 2000;
	const float WindowHeight = 1120;

	InitStatus InitGLFW();
	InitStatus InitOpenGL();
	void RenderScene();

	void LogGraphicsInfo();

};