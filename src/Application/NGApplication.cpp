#include "NGApplication.h"
#include "Logger/Logger.h"


NGApplication::NGApplication() {}

NGApplication::~NGApplication() 
{
	Shutdown();
}

bool NGApplication::Init() {
	if(!InitGLFW())
	{
		return false;
	}

	if(!InitOpenGL()) 
	{
		return false;
	}

	gui.Init(window);
	Logger::Log("Application initialized");
	return true;
}

bool NGApplication::InitGLFW()
{
	if(!glfwInit()) 
	{
		Logger::Err("Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1280, 720, "Noise Generator", nullptr, nullptr);
	if(!window) 
	{
		Logger::Err("Failed to create GLFW window");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	Logger::Log("GLFW initialized");
	return true;
}

bool NGApplication::InitOpenGL() 
{
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		Logger::Err("Failed to initialize GLAD");
		return false;
	}

	Logger::Log("OpenGL context initialized");
	return true;
}

void NGApplication::RenderScene() 
{
	glViewport(0, 0, 1280, 720);
	glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void NGApplication::Run() 
{
	while(!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();

		gui.BeginFrame();
		gui.DrawUI();

		RenderScene();
		gui.Render();

		glfwSwapBuffers(window);
	}
}

void NGApplication::Shutdown()
{
	gui.Shutdown();
	if(window) 
	{
		glfwDestroyWindow(window);
		window = nullptr;
	}
	glfwTerminate();

	Logger::Log("Application shutdown");
}