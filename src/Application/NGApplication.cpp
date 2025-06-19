#include "NGApplication.h"
#include "Config/SettingsManager.h"



DEFINE_LOG_CATEGORY(SystemLog);

DEFINE_LOG_CATEGORY(LogApp);

DEFINE_LOG_CATEGORY(LogGLFW);
DEFINE_LOG_CATEGORY(LogOpenGL);

NGApplication::NGApplication()
{
	LogSystemInfo();
}

NGApplication::~NGApplication()
{
	Shutdown();
}

void NGApplication::LogSystemInfo()
{
//	NGLOG(SystemLog, Info, "==== System Info ====");
//
//#ifdef _WIN32
//	NGLOG(SystemLog, Info, "Platform          : Windows");
//#elif __APPLE__
//	NGLOG(SystemLog, Info, "Platform          : macOS");
//#elif __linux__
//	NGLOG(SystemLog, Info, "Platform          : Linux");
//#else
//	NGLOG(SystemLog, Info, "Platform          : Unknown");
//#endif
//
//	// CPU Cores
//	unsigned int cores = std::thread::hardware_concurrency();
//	NGLOG(LogApp, Info, "CPU Cores         : " + std::to_string(cores));
//
//#ifdef _WIN32
//	// Memory Info
//	MEMORYSTATUSEX memStatus;
//	memStatus.dwLength = sizeof(memStatus);
//	if (GlobalMemoryStatusEx(&memStatus))
//	{
//		auto availMB = memStatus.ullAvailPhys / (1024 * 1024);
//		auto totalMB = memStatus.ullTotalPhys / (1024 * 1024);
//		NGLOG(SystemLog, Info, "Memory Available  : " + std::to_string(availMB) + " MB");
//		NGLOG(SystemLog, Info, "Memory Total      : " + std::to_string(totalMB) + " MB");
//	}
//
//	// Screen Resolution
//	RECT desktop;
//	const HWND hDesktop = GetDesktopWindow();
//	GetWindowRect(hDesktop, &desktop);
//	int screenWidth = desktop.right;
//	int screenHeight = desktop.bottom;
//	NGLOG(SystemLog, Info, "Screen Resolution : " + std::to_string(screenWidth) + "x" + std::to_string(screenHeight));
//#endif
//
//	NGLOG(SystemLog, Info, "=====================");
}

InitStatus NGApplication::InitializeApplication()
{
	if (bIsInitialized)
	{
		return InitStatus::Success;
	}

	SettingsManager::Get().Load();
	
	WindowWidth = SettingsManager::Get().GetWindowWidth();
	WindowHeight = SettingsManager::Get().GetWindowHeight();

	auto glfwStatus = InitializeGLFW();
	if (glfwStatus != InitStatus::Success)
	{
		NGLOG(LogGLFW, Error, "Failed to initialize GLFW");
		return glfwStatus;
	}

	auto glStatus = InitializeOpenGL();
	if (glStatus != InitStatus::Success)
	{
		NGLOG(LogGLFW, Error, "Failed to initialize OpenGL");
		return glStatus;
	}

	LogGraphicsInfo();

	assert(window && "GLFW window is null before GUI initialization");
	GUI.Init(window);

	NGLOG(LogApp, Info, "Application initialized");
	bIsInitialized = true;
	return InitStatus::Success;
}

InitStatus NGApplication::InitializeGLFW()
{
	if (!glfwInit())
	{
		NGLOG(LogGLFW, Error, "Failed to initialize GLFW");
		return InitStatus::GLFW_InitFailed;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	assert(WindowWidth > 0 && WindowHeight > 0 && "Invalid window size");
	window = glfwCreateWindow(WindowWidth, WindowHeight, "Noise Generator", nullptr, nullptr);
	if (!window)
	{
		NGLOG(LogApp, Error, "Failed to create GLFW window");
		glfwTerminate();
		return InitStatus::WindowCreationFailed;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	NGLOG(LogGLFW, Info, "GLFW initialized");
	int glfwMajor, glfwMinor, glfwRev;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRev);

	NGLOG(LogGLFW, Info, "GLFW Version      : " + std::to_string(glfwMajor) + "." + std::to_string(glfwMinor) + "." + std::to_string(glfwRev));

	return InitStatus::Success;
}

InitStatus NGApplication::InitializeOpenGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		NGLOG(LogApp, Error, "Failed to initialize GLAD");
		return InitStatus::GLAD_InitFailed;
	}

	return InitStatus::Success;
}

void NGApplication::RenderScene()
{
	glViewport(0, 0, WindowWidth, WindowHeight);
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void NGApplication::LogGraphicsInfo()
{
	NGLOG(LogOpenGL, Info, "OpenGL Version    : " + std::string((const char*)glGetString(GL_VERSION)));
	NGLOG(LogOpenGL, Info, "GLSL Version      : " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	NGLOG(LogOpenGL, Info, "GL Vendor         : " + std::string((const char*)glGetString(GL_VENDOR)));
	NGLOG(LogOpenGL, Info, "GL Renderer       : " + std::string((const char*)glGetString(GL_RENDERER)));
}

std::string NGApplication::GetInitStatus(InitStatus status)
{
	switch (status)
	{
	case InitStatus::Success:
		return "Success";
	case InitStatus::GLFW_InitFailed:
		return "GLFW initialization failed";
	case InitStatus::WindowCreationFailed:
		return "Window creation failed";
	case InitStatus::OpenGL_InitFailed:
		return "OpenGL initialization failed";
	case InitStatus::GLAD_InitFailed:
		return "GLAD loading failed";
	case InitStatus::Fail:
		return "failed loaded";
	default:
		return "Unknown error";
	}
}

void NGApplication::RunApplication()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		GUI.BeginFrame();
		GUI.DrawUI();

		/* render */
		RenderScene();

		GUI.Render();

		glfwSwapBuffers(window);
	}
}

void NGApplication::Shutdown()
{
	GUI.Shutdown();

	if (window)
	{
		NGLOG(LogApp, Info, "Destroying window...");
		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
	NGLOG(LogApp, Info, "Application shutdown");
}

bool NGApplication::IsInitialized() const
{
	return bIsInitialized;
}
