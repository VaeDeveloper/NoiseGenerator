/*
							  $$\   $$\           $$\
							  $$$\  $$ |          \__|
							  $$$$\ $$ | $$$$$$\  $$\  $$$$$$$\  $$$$$$\
							  $$ $$\$$ |$$  __$$\ $$ |$$  _____|$$  __$$\
							  $$ \$$$$ |$$ /  $$ |$$ |\$$$$$$\  $$$$$$$$ |
							  $$ |\$$$ |$$ |  $$ |$$ | \____$$\ $$   ____|
							  $$ | \$$ |\$$$$$$  |$$ |$$$$$$$  |\$$$$$$$\
							  \__|  \__| \______/ \__|\_______/  \_______|
			 $$$$$$\                                                    $$\
			$$  __$$\                                                   $$ |
			$$ /  \__| $$$$$$\  $$$$$$$\   $$$$$$\   $$$$$$\  $$$$$$\ $$$$$$\    $$$$$$\   $$$$$$\
			$$ |$$$$\ $$  __$$\ $$  __$$\ $$  __$$\ $$  __$$\ \____$$\\_$$  _|  $$  __$$\ $$  __$$\
			$$ |\_$$ |$$$$$$$$ |$$ |  $$ |$$$$$$$$ |$$ |  \__|$$$$$$$ | $$ |    $$ /  $$ |$$ |  \__|
			$$ |  $$ |$$   ____|$$ |  $$ |$$   ____|$$ |     $$  __$$ | $$ |$$\ $$ |  $$ |$$ |
			\$$$$$$  |\$$$$$$$\ $$ |  $$ |\$$$$$$$\ $$ |     \$$$$$$$ | \$$$$  |\$$$$$$  |$$ |
			 \______/  \_______|\__|  \__| \_______|\__|      \_______|  \____/  \______/ \__|

								__				   ___      ______          __        __
							___/ /__ _  __		  / _ \__ _/_  __/__  ___  / /__ ___ / /_
						   / _  / -_) |/ /		 / // /\ \ // / / _ \/ _ \/ (_-</ -_) __/
						   \_,_/\__/|___/		 \___//_\_\/_/  \___/\___/_/___/\__/\__/

				An interactive C++ desktop application for generating and visualizing procedural 2D noise
			(including Perlin noise) with customizable parameters, preview rendering, and export options.
*/

#include "Application/NGApplication.h"
#include <thread>
#include <string>
#include <filesystem>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

DEFINE_LOG_CATEGORY(LogTemp);

void LogSystemInfo()
{
	NGLOG(LogTemp, Info, "==== System Info ====");

#ifdef _WIN32
	NGLOG(LogTemp, Info, "Platform          : Windows");
#elif __APPLE__
	NGLOG(LogTemp, Info, "Platform          : macOS");
#elif __linux__
	NGLOG(LogTemp, Info, "Platform          : Linux");
#else
	NGLOG(LogTemp, Info, "Platform          : Unknown");
#endif

	unsigned int cores = std::thread::hardware_concurrency();
	NGLOG(LogTemp, Info, "CPU Cores         : " + std::to_string(cores));

#ifdef _WIN32
	MEMORYSTATUSEX memStatus;
	memStatus.dwLength = sizeof(memStatus);
	if(GlobalMemoryStatusEx(&memStatus)) {
		auto availMB = memStatus.ullAvailPhys / (1024 * 1024);
		auto totalMB = memStatus.ullTotalPhys / (1024 * 1024);
		NGLOG(LogTemp, Info, "Memory Available  : " + std::to_string(availMB) + " MB");
		NGLOG(LogTemp, Info, "Memory Total      : " + std::to_string(totalMB) + " MB");
	}

	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	int screenWidth = desktop.right;
	int screenHeight = desktop.bottom;
	NGLOG(LogTemp, Info, "Screen Resolution : " + std::to_string(screenWidth) + "x" + std::to_string(screenHeight));
#endif

	NGLOG(LogTemp, Info, "=====================");
}

int RunApplication()
{
	try 
	{
		NGApplication app;
		InitStatus status = app.InitializeApplication();
		if(status != InitStatus::Success) 
		{
			NGLOG(LogTemp, Error, std::string("Init failed: ") + NGApplication::GetInitStatus(status));
			return -1;
		}

		app.RunApplication();
		return 0;
	}
	catch(const std::exception& e) 
	{
		NGLOG(LogTemp, Error, std::string("Unhandled exception: ") + e.what());
		return -2;
	}
	catch(...) 
	{
		NGLOG(LogTemp, Error, "Unknown unhandled exception occurred");
		return -3;
	}
}

#ifdef _WIN32
#ifdef _DEBUG
int main(int argc, char** argv) 
{
	LogSystemInfo();
	return RunApplication();
}
#else
#include <Windows.h>
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int) 
{
	LogSystemInfo();
	return RunApplication();
}
#endif
#else
int main(int argc, char** argv) 
{
	return RunApplication();
}
#endif


