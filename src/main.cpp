#include "Windows.h"
#include "Application/NGApplication.h"
#include "Logger/Logger.h"


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
			Logger::Err("Init failed: " + NGApplication::GetInitStatus(status));
			return -1;
		}

		app.RunApplication();
		return 0;
	}
	catch(const std::exception& e)
	{
		Logger::Err(std::string("Unhandled exception: ") + e.what());
		return -2;
	}
	catch(...)
	{
		Logger::Err("Unknown unhandled exception occurred");
		return -3;
	}
}