
#include "Application/NGApplication.h"
#include <thread>
#include <string>


DEFINE_LOG_CATEGORY(LogTemp);

#ifdef APIENTRY
#undef APIENTRY
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _DEBUG
#pragma message("Debug build!")
#else
#pragma message("Release build!")
#endif


// LogSystemInfo
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

#ifdef _DEBUG
#pragma message("Debug build!")
#else
#pragma message("Release build!")
#endif


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
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