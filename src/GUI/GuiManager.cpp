
#include "GuiManager.h"
#include "Logger/LoggerUI.h"
#include "Logger/Logger.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Noise/NoiseTypes.h"
#include "Noise/NoiseGenerator.h"

void GuiManager::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	Logger::Log("ImGui initialized");
	Logger::Log("OpenGL #version 330");
	noisePreview.Init();
}

void GuiManager::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void GuiManager::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void GuiManager::Render() 
{
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiManager::SetNoiseData(float* data, int width, int height) {
	noisePreview.UpdateTexture(data, width, height);
}

void GuiManager::DrawUI()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

	if(!dockBuilt)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

		ImGuiID dock_main_id = dockspace_id;

		ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);

		ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, nullptr, &dock_main_id);
		ImGuiID dock_right = dock_main_id;

		ImGui::DockBuilderDockWindow("Noise Generator", dock_left);
		ImGui::DockBuilderDockWindow("Noise Preview", dock_right);
		ImGui::DockBuilderDockWindow("Output Log", dock_bottom);

		ImGui::DockBuilderFinish(dockspace_id);
		dockBuilt = true;
	}

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags host_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;

	ImGui::Begin("DockSpaceRoot", nullptr, host_flags);

	// === Menu Bar ===
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu("File"))
		{
			
			if(ImGui::MenuItem("Save", "Ctrl+S"))
			{
				Logger::Log("Save clicked");

			}

			ImGui::Separator();

			
			if(ImGui::MenuItem("Exit", "Alt+F4"))
			{
				exit(0);
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Noise Generator", nullptr, &showNoiseGenerator);
			ImGui::MenuItem("Output Log", nullptr, &showOutputLog);
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("About");
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(dockspace_id);

	ImGui::End();

	if(showNoiseGenerator)
	{
		ImGui::Begin("Noise Generator", nullptr, ImGuiWindowFlags_NoTitleBar);

		static int resolutionIndex = 3;
		static const char* resolutions[] = { "8", "16", "32", "64", "128", "256", "512" };

		// Base properties
		static int seed = 42;
		static float roughness = 0.5f;
		static float marbling = 0.0f;
		static int low_freq_skip = 0;
		static int high_freq_skip = 0;

		// Turbulence
		static float turbulence = 0.0f;
		static int turbulence_res = 2;
		static float turbulence_roughness = 0.5f;
		static int turbulence_low_freq_skip = 0;
		static int turbulence_high_freq_skip = 0;
		static float turbulence_marbling = 0.0f;
		static float turbulence_expshift = 0.0f;
		static float turbulence_offset_x = 0.0f;
		static float turbulence_offset_y = 0.0f;

		// Preview control
		static int previewWidth = 256;
		static int previewHeight = 256;

		// Layout
		ImGui::Text("Noise Settings");
		ImGui::Combo("Resolution", &resolutionIndex, resolutions, IM_ARRAYSIZE(resolutions));
		ImGui::SliderFloat("Roughness", &roughness, 0.01f, 1.0f);
		ImGui::SliderFloat("Marbling", &marbling, 0.0f, 10.0f);
		ImGui::InputInt("Seed", &seed);
		ImGui::SliderInt("Low Freq Skip", &low_freq_skip, 0, 12);
		ImGui::SliderInt("High Freq Skip", &high_freq_skip, 0, 12);

		ImGui::SeparatorText("Turbulence");
		ImGui::SliderFloat("Turbulence", &turbulence, 0.0f, 64.0f);
		ImGui::Combo("Turbulence Res", &turbulence_res, resolutions, IM_ARRAYSIZE(resolutions));
		ImGui::SliderFloat("Turbulence Roughness", &turbulence_roughness, 0.01f, 1.0f);
		ImGui::SliderInt("Turb Low Freq Skip", &turbulence_low_freq_skip, 0, 12);
		ImGui::SliderInt("Turb High Freq Skip", &turbulence_high_freq_skip, 0, 12);
		ImGui::SliderFloat("Turbulence Marbling", &turbulence_marbling, 0.0f, 10.0f);
		ImGui::SliderFloat("Exp Shift", &turbulence_expshift, -4.0f, 4.0f);
		ImGui::SliderFloat("Turb Offset X", &turbulence_offset_x, -1.0f, 1.0f);
		ImGui::SliderFloat("Turb Offset Y", &turbulence_offset_y, -1.0f, 1.0f);

		ImGui::SeparatorText("Preview Output");
		ImGui::InputInt("Preview Width", &previewWidth);
		ImGui::InputInt("Preview Height", &previewHeight);

		if(ImGui::Button("Generate 2D Noise"))
		{
			int res = 8 << resolutionIndex;
			noise_properties props = {};
			props.seed = seed;
			props.res = resolutionIndex;
			props.roughness = roughness;
			props.marbling = marbling;
			props.low_freq_skip = low_freq_skip;
			props.high_freq_skip = high_freq_skip;

			props.turbulence = turbulence;
			props.turbulence_res = turbulence_res;
			props.turbulence_roughness = turbulence_roughness;
			props.turbulence_low_freq_skip = turbulence_low_freq_skip;
			props.turbulence_high_freq_skip = turbulence_high_freq_skip;
			props.turbulence_marbling = turbulence_marbling;
			props.turbulence_expshift = turbulence_expshift;
			props.turbulence_offset_x = turbulence_offset_x;
			props.turbulence_offset_y = turbulence_offset_y;

			float* noise = NoiseGenerator::PerlinNoise2D(res, &props);
			this->SetNoiseData(noise, res, res);
			Logger::Log("Generated 2D noise preview");
			free(noise);
		}

		ImGui::SameLine();
		if(ImGui::Button("Clear"))
		{
			this->SetNoiseData(nullptr, 0, 0);
			Logger::Warn("Preview cleared");
		}

		ImGui::End();
	}
	noisePreview.Draw();

	// Log window
	if(showOutputLog)
	{
		ImGui::Begin("Output Log", nullptr, ImGuiWindowFlags_NoTitleBar);
		DrawLoggerWindow();
		ImGui::End();
	}
}