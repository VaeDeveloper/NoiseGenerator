#pragma once
#include <glad/glad.h>
#include "GUI/GuiManager.h"
#include "Logger/LoggerMacro.h"



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

	InitStatus InitializeApplication();
	void RunApplication();
	void Shutdown();
	bool IsInitialized() const;

	static std::string GetInitStatus(InitStatus status);
private:
	GLFWwindow* window = nullptr;
	GuiManager GUI;

	int WindowWidth = 2000;
	int WindowHeight = 1120;

	InitStatus InitializeGLFW();
	InitStatus InitializeOpenGL();
	void RenderScene();
	void LogGraphicsInfo();

	bool bIsInitialized = false;

	void LogSystemInfo();

};