#pragma once


#include <glad/glad.h>

#include "GuiManager.h"
#include "LoggerMacro.h"

enum class InitStatus
{
	Success,
	GLFW_InitFailed,
	WindowCreationFailed,
	OpenGL_InitFailed,
	GLAD_InitFailed,
	Fail

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
};