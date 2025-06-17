#include "Windows.h"
#include "Application/NGApplication.h"

DEFINE_LOG_CATEGORY(LogTemp);

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