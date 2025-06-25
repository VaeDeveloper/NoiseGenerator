#include "MenuBarController.h"

DEFINE_LOG_CATEGORY(MBControllerLog);

void MenuBarController::Initialize()
{
	model = std::make_shared<MenuBarModel>();
}

void MenuBarController::ToggleFullScreen()
{
	if(!model) 
	{
		NGLOG(MBControllerLog, Error, std::string("Model don't init  ") + __FUNCTION__ + 
			" at line " + std::to_string(__LINE__));
		return;
	}

	const bool bNewState = !model->IsFullscreen();
	model->SetFullscreen(bNewState);

	GLFWwindow* window = glfwGetCurrentContext();
	if(!window)
	{
		NGLOG(MBControllerLog, Error, std::string("Error getter current context : ") + __FUNCTION__ +
			" at line-" + std::to_string(__LINE__));
		return;
	}

	if(bNewState)
	{
		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		const int width = SettingsManager::Get().GetWindowWidth();
		const int height = SettingsManager::Get().GetWindowHeight();
		glfwSetWindowMonitor(window, nullptr, 100, 100, width, height, 0);
	}
}

void MenuBarController::ExportAs(const std::string& format, GLuint textureId, int width, int height)
{
	if(textureId == 0 || width <= 0 || height <= 0)
	{
		NGLOG(MBControllerLog, Error, std::string("Invalid export parameters.  ") + __FUNCTION__ + std::to_string(__LINE__));
		return;
	}

	std::string filter = format;
	std::string extension = "." + format;

	nfdchar_t* outPath = nullptr;
	if(NFD_SaveDialog(filter.c_str(), nullptr, &outPath) == NFD_OKAY)
	{
		std::string pathStr = NG::EnsureExtension(outPath, extension);
		bool success = false;

		if(format == "png")
		{
			success = ImageExporter::SavePNG(pathStr, textureId, width, height);
		}
		else if(format == "tga")
		{
		}
		else if(format == "bmp")
		{
			success = ImageExporter::SaveBMP(pathStr, textureId, width, height);
		}
		else if(format == "jpg") 
		{
			success = ImageExporter::SaveJPG(pathStr, textureId, width, height, 90);
		}

		free(outPath);
	}
}

bool MenuBarController::IsFullscreen() const
{
	if(!model)
	{
		NGLOG(MBControllerLog, Error, std::string("Model don't init") + __FUNCTION__ +
			" at line " + std::to_string(__LINE__));
		return false;
	}

	return model->IsFullscreen();
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
		NGLOG(MBControllerLog, Error, "Failed to open URL: " + std::string(url));
	}
	else
	{
		NGLOG(MBControllerLog, Info, "Opened URL: " + std::string(url));
	}
}

void MenuBarController::RequestExit()
{
	exit(0);
}

void MenuBarController::ToggleInfoPanel()
{
	bool newState = !model->IsInfoPanelVisible();
	model->SetInfoPanelVisible(newState);

	OnInfoPanelToggled.Execute(newState);
}


