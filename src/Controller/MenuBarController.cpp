#include "MenuBarController.h"

DEFINE_LOG_CATEGORY(ExportLog);
DEFINE_LOG_CATEGORY(MenuBarControllerLog);

void MenuBarController::ToggleFullScreen()
{
	if(!Model) 
	{
		NGLOG(MenuBarControllerLog, Error, std::string("Model don't init") + __FUNCTION__ + " at line " + std::to_string(__LINE__));
		return;
	}

	bool bNewState = !Model->IsFullscreen();
	Model->SetFullscreen(bNewState);

	GLFWwindow* window = glfwGetCurrentContext();
	if(!window) return;

	if(bNewState)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		int width = SettingsManager::Get().GetWindowWidth();
		int height = SettingsManager::Get().GetWindowHeight();
		glfwSetWindowMonitor(window, nullptr, 100, 100, width, height, 0);
	}
}

void MenuBarController::ExportAs(const std::string& format, GLuint textureId, int width, int height)
{
	if(textureId == 0 || width <= 0 || height <= 0)
	{
		NGLOG(ExportLog, Error, std::string("Invalid export parameters.  ") + __FUNCTION__);
		return;
	}

	std::string filter = format;
	std::string extension = "." + format;

	nfdchar_t* outPath = nullptr;
	if(NFD_SaveDialog(filter.c_str(), nullptr, &outPath) == NFD_OKAY)
	{
		std::string pathStr = NG::EnsureExtension(outPath, extension);
		bool success = false;

		if(format == "png")      success = ImageExporter::SavePNG(pathStr, textureId, width, height);
		else if(format == "tga") success = ImageExporter::SaveTGA(pathStr, textureId, width, height);
		else if(format == "bmp") success = ImageExporter::SaveBMP(pathStr, textureId, width, height);
		else if(format == "jpg") success = ImageExporter::SaveJPG(pathStr, textureId, width, height, 90);

		free(outPath);
	}
}

bool MenuBarController::IsFullscreen() const
{
	if(!Model)
	{
		NGLOG(MenuBarControllerLog, Error, std::string("Model don't init") + __FUNCTION__ + " at line " + std::to_string(__LINE__));
		return false;
	}

	return Model->IsFullscreen();
}

void MenuBarController::OpenAbout(const char* url)
{
	std::string command;

#if defined(_WIN32)
	command = "start \"\" \"" + std::string(url) + "\"";
#elif defined(__APPLE__)
	command = "open \"" + std::string(url) + "\"";
#elif defined(__linux__)
	command = "xdg-open \"" + std::string(url) + "\"";
#else
	NGLOG(LogGUI, Error, "Unsupported platform for opening URLs");
	return;
#endif

	int result = std::system(command.c_str());
	if(result != 0)
	{
		NGLOG(MenuBarControllerLog, Error, "Failed to open URL: " + std::string(url));
	}
	else
	{
		NGLOG(MenuBarControllerLog, Info, "Opened URL: " + std::string(url));
	}
}

void MenuBarController::RequestExit()
{
	exit(0);
}

void MenuBarController::ToggleInfoPanel()
{
	bool newState = !Model->IsInfoPanelVisible();
	Model->SetInfoPanelVisible(newState);

	OnInfoPanelToggled.Broadcast(newState); // уведомим подписчиков
}


