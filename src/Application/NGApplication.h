#pragma once


#include <glad/glad.h>

#include "GuiManager.h"
#include "LoggerMacro.h"

/**
 * Enum representing the status of application initialization.
 */
enum class InitStatus 
{
	Success,
	GLFW_InitFailed,
	WindowCreationFailed,
	OpenGL_InitFailed,
	GLAD_InitFailed,
	Fail
};

/**
 * Main application class responsible for managing lifecycle,
 * including initialization, main loop execution, and shutdown.
 */
class NGApplication final
{
public:
	NGApplication();
	~NGApplication();

	/**
	 * Initializes the application subsystems.
	 * @return InitStatus Enum value indicating the result of initialization.
	 */
	[[nodiscard]] InitStatus InitializeApplication();

	/**
	 * Starts the main application loop.
	 */
	void RunApplication();

	/**
	 * Cleans up all resources and shuts down the application.
	 */
	void Shutdown();

	/**
	 * Checks whether the application was successfully initialized.
	 * @return true if initialized, false otherwise.
	 */
	bool IsInitialized() const noexcept;

	/**
	 * Converts InitStatus enum to a human-readable string.
	 * @param status InitStatus value.
	 * @return String representation of the initialization status.
	 */
	static const char* GetInitStatus(InitStatus status) noexcept;

private:
	/**
	 * Pointer to the GLFW window instance.
	 */
	GLFWwindow* window = nullptr;

	/**
	 * GUI system manager instance.
	 */
	GuiManager GUI;

	/**
	 * Window width in pixels.
	 */
	int WindowWidth = 2000;

	/**
	 * Window height in pixels.
	 */
	int WindowHeight = 1120;


	/**
	 * Initializes the GLFW library and creates a window.
	 * @return InitStatus result of the initialization step.
	 */
	[[nodiscard]] InitStatus InitializeGLFW();

	/**
	 * Initializes OpenGL and related graphics settings.
	 * @return InitStatus result of the initialization step.
	 */
	[[nodiscard]] InitStatus InitializeOpenGL();

	/**
	 * Renders the main scene for the current frame.
	 */
	void RenderScene() noexcept;

	/**
	 * Logs details about the graphics hardware and drivers.
	 */
	void LogGraphicsInfo() const noexcept;

	/**
	 * Flag indicating if the application has been successfully initialized.
	 */
	bool bIsInitialized = false;
};