#include "NGApplication.h"
#include "Config/SettingsManager.h"


NGApplication::NGApplication(){}

NGApplication::~NGApplication() 
{
	Shutdown();
}

InitStatus NGApplication::InitializeApplication()
{
	if(bIsInitialized)
	{
		return InitStatus::Success;
	}

	SettingsManager::Get().Load();
	WindowWidth = SettingsManager::Get().GetWindowWidth();
	WindowHeight = SettingsManager::Get().GetWindowHeight();

	auto glfwStatus = InitializeGLFW();
	if(glfwStatus != InitStatus::Success) 
	{
		Logger::Err("Failed to initialize GLFW");
		return glfwStatus;
	}

	auto glStatus = InitializeOpenGL();
	if(glStatus != InitStatus::Success) 
	{
		Logger::Err("Failed to initialize OpenGL");
		return glStatus;
	}
	
	LogGraphicsInfo();

	assert(window && "GLFW window is null before GUI initialization");
	GUI.Init(window);

	Logger::Log("Application initialized");
	bIsInitialized = true;
	return InitStatus::Success;
}

InitStatus NGApplication::InitializeGLFW()
{
	if(!glfwInit())
	{
		Logger::Err("Failed to initialize GLFW");
		return InitStatus::GLFW_InitFailed;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	assert(WindowWidth > 0 && WindowHeight > 0 && "Invalid window size");
	window = glfwCreateWindow(WindowWidth, WindowHeight, "Noise Generator", nullptr, nullptr);
	if(!window) 
	{
		Logger::Err("Failed to create GLFW window");
		glfwTerminate();
		return InitStatus::WindowCreationFailed;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	Logger::Log("GLFW initialized");
	int glfwMajor, glfwMinor, glfwRev;
	glfwGetVersion(&glfwMajor, &glfwMinor, &glfwRev);

	Logger::Log("GLFW Version      : " + std::to_string(glfwMajor) + "." +
		std::to_string(glfwMinor) + "." + std::to_string(glfwRev));

	return InitStatus::Success;
}

InitStatus NGApplication::InitializeOpenGL() 
{
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		Logger::Err("Failed to initialize GLAD");
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
	Logger::Log("OpenGL Version    : " + std::string((const char*)glGetString(GL_VERSION)));
	Logger::Log("GLSL Version      : " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
	Logger::Log("GL Vendor         : " + std::string((const char*)glGetString(GL_VENDOR)));
	Logger::Log("GL Renderer       : " + std::string((const char*)glGetString(GL_RENDERER)));
}

std::string NGApplication::GetInitStatus(InitStatus status)
{
	switch(status)
	{
	case InitStatus::Success: return "Success";
	case InitStatus::GLFW_InitFailed: return "GLFW initialization failed";
	case InitStatus::WindowCreationFailed: return "Window creation failed";
	case InitStatus::OpenGL_InitFailed: return "OpenGL initialization failed";
	case InitStatus::GLAD_InitFailed: return "GLAD loading failed";
	default: return "Unknown error";
	}
}

void NGApplication::RunApplication()
{
	while(!glfwWindowShouldClose(window)) 
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
	if(window) 
	{
		Logger::Log("Destroying window...");
		glfwDestroyWindow(window);
		window = nullptr;
	}
	glfwTerminate();
	Logger::Log("Application shutdown");
}

bool NGApplication::IsInitialized() const
{
	return bIsInitialized;
}
