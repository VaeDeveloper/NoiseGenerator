#include "Application/NGApplication.h"


int main()
{
	NGApplication app;
	if(!app.Init())
		return -1;

	app.Run();
	return 0;
}