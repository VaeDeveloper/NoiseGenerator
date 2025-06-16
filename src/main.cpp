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
	NGApplication app;
	InitStatus status = app.Init();
	if(status != InitStatus::Success)
	{
		Logger::Err("Init failed: " + NGApplication::StatusToString(status));
		return -1;
	}

	app.Run();
	return 0;
}