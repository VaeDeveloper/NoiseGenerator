#include "Application/NGApplication.h"
#include "Windows.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	NGApplication app;
	if(!app.Init())
		return -1;

	app.Run();
	return 0;
}