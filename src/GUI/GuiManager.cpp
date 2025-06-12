
#include "GuiManager.h"
#include "Logger/LoggerUI.h"
#include "Logger/Logger.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Noise/NoiseTypes.h"
#include "Noise/NoiseGenerator.h"
#include "Image/ImageExporter.h"
#include <random>
#include <windows.h>
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
	noisePreview.Init();
}

void GuiManager::QueueUITask(std::function<void()> task)
{
	std::lock_guard<std::mutex> lock(uiMutex);
	uiTasks.push(std::move(task));
}

void GuiManager::Shutdown()
{
	if(generationThread.joinable())
	{
		generationThread.join();
	}

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

void GuiManager::SetNoiseData(float* data, int width, int height) 
{
	noisePreview.UpdateTexture(data, width, height);
}

void GuiManager::DrawUI()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

	if(!dockBuilt)
	{
		ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
		ImGui::DockBuilderAddNode(dockspace_id,
			ImGuiDockNodeFlags_DockSpace |
			ImGuiDockNodeFlags_NoDockingSplit |
			ImGuiDockNodeFlags_NoDockingOverMe |
			ImGuiDockNodeFlags_NoTabBar |
			ImGuiDockNodeFlags_NoDockingInCentralNode);

		ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
		ImGuiID dock_main_id = dockspace_id;
		ImGuiID dock_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
		ImGuiID dock_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.5f, nullptr, &dock_main_id);
		ImGuiID dock_left_bottom = ImGui::DockBuilderSplitNode(dock_left, ImGuiDir_Down, 0.1f, nullptr, &dock_left);
		ImGuiID dock_right = dock_main_id;

		ImGui::DockBuilderDockWindow("Noise Generator", dock_left);
		ImGui::DockBuilderDockWindow("Generation Progress", dock_left_bottom);
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
			if(ImGui::BeginMenu("Save As"))
			{
				if(ImGui::MenuItem("Export as PNG"))
				{
					if(noisePreview.IsInitialized())
					{
						if(ImageExporter::SavePNG("export.png", noisePreview.GetTextureId(), noisePreview.GetWidth(), noisePreview.GetHeight()))
						{
							Logger::Log("Saved noise as PNG: export.png");
						}
						else
						{
							Logger::Err("Failed to save PNG");
						}
					}
				}

				if(ImGui::MenuItem("Export as TGA"))
				{
					if(noisePreview.IsInitialized())
					{
						if(ImageExporter::SaveTGA("export.tga", noisePreview.GetTextureId(), noisePreview.GetWidth(), noisePreview.GetHeight()))
						{
							Logger::Log("Saved noise as TGA: export.tga");
						}
						else
						{
							Logger::Err("Failed to save TGA");
						}
					}
				}

				ImGui::EndMenu();
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
			if(ImGui::MenuItem("About"))
			{
#ifdef _WIN32
				ShellExecuteA(NULL, "open", "https://github.com/VaeDeveloper/NoiseGenerator", NULL, NULL, SW_SHOWNORMAL);
#else
				system("xdg-open https://github.com/VaeDeveloper/NoiseGenerator"); // Linux
				// macOS: system("open https://github.com/VaeDeveloper/NoiseGenerator");
#endif
			}
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(dockspace_id);

	ImGui::End();

	if(showNoiseGenerator)
	{
		ImGui::Begin("Noise Generator", nullptr, ImGuiWindowFlags_NoTitleBar);
		ImGuiID dock_id = ImGui::GetWindowDockID();
		if(dock_id != 0)
		{
			if(ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id))
			{
				if(node->WantHiddenTabBarUpdate)
					node->WantHiddenTabBarToggle = true;
			}
		}

		static int resolutionIndex = 3;
		static const char* resolutions[] = { "8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096" };

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

		if(!isGenerating)
		{
			if(ImGui::Button("Generate 2D Noise") && !isGenerating)
			{
				int res = 8 << resolutionIndex;
				noise_properties props = {};
				props.seed = std::random_device{}();
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

				isGenerating = true;
				generationProgress = 0.0f;
				generationThread = std::thread([this, res, props] () {
					float* noise = NoiseGenerator::PerlinNoise2D(res, &props, [this] (float progress)
						{
							this->generationProgress = progress;
						});

					this->QueueUITask([this, noise, res] ()
						{
							this->SetNoiseData(noise, res, res);
							Logger::Log("Generated 2D noise preview");
							free(noise);
							this->isGenerating = false;
							this->generationProgress = -1.0f;
						});
					});

				generationThread.detach();
			}

			ImGui::SameLine();

			if(ImGui::Button("Clear"))
			{
				this->SetNoiseData(nullptr, 0, 0);
				Logger::Warn("Preview cleared");
			}
		}

		if(isGenerating)
		{
			ImGui::ProgressBar(generationProgress, ImVec2(-1.0f, 0.0f), generationProgress >= 1.0f ? "Done" : "Generating...");
		}

		ImGui::End();
	}



	noisePreview.Draw();

	// Log window
	if(showOutputLog)
	{
		ImGui::Begin("Output Log", nullptr, ImGuiWindowFlags_NoTitleBar);
		ImGuiID dock_id = ImGui::GetWindowDockID();
		if(dock_id != 0 )
		{
			if(ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id))
			{
				if (node->WantHiddenTabBarUpdate)
					node->WantHiddenTabBarToggle = true;
			}
		}
		DrawLoggerWindow();
		ImGui::End();
	}

	{
		std::lock_guard<std::mutex> lock(uiMutex);
		while(!uiTasks.empty())
		{
			uiTasks.front()(); // execute
			uiTasks.pop();
		}
	}
}