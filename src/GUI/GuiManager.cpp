
#include "GuiManager.h"
#include "Logger/LoggerUI.h"
#include "Logger/Logger.h"


#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Noise/NoiseTypes.h"
#include "Noise/NoiseGenerator.h"
#include "Image/ImageExporter.h"
#include <random>
#include <windows.h>


namespace NoiseGenerator
{
	constexpr int spaceOffset = 24;

	template<typename WidgetFunc>
	void LabeledWidgetWithLock(const char* lockID, bool* lockState, WidgetFunc widget)
	{
		if(*lockState) ImGui::BeginDisabled(true);
		widget(); 
		if(*lockState) ImGui::EndDisabled();

		ImGui::SameLine();
		float space = ImGui::GetContentRegionAvail().x;
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space - spaceOffset);

		const char* icon = *lockState ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN;

		ImGui::PushID(lockID);
		if(ImGui::Button(icon, ImVec2(24, 22)))
		{
			*lockState = !*lockState;
			Logger::Log(std::string("Lock toggled: ") + lockID + " = " + (*lockState ? "true" : "false"));
		}
		ImGui::PopID();
	}

	template<typename T>
	void LogWidget(const char* label, T* value, std::function<bool()> widget)
	{
		if(widget())
		{
			Logger::Log(std::string(label) + " = " + std::to_string(*value));
		}
	}

}

void GuiManager::Init(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	
	InitStyleConfig(io);
	InitFontConfig(io);
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	Logger::Log("ImGui initialized");

	noisePreview.Init();
}

void GuiManager::InitStyleConfig(const ImGuiIO& io)
{
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	io.Fonts->AddFontDefault();

	style.ItemSpacing = ImVec2(12.0f, 10.0f);

	style.FrameRounding = 2.0f;
	style.WindowRounding = 8.0f;
	style.ChildRounding = 6.0f;
	style.PopupRounding = 6.0f;
	style.ScrollbarRounding = 6.0f;
	style.GrabRounding = 2.0f;
}

void GuiManager::InitFontConfig(const ImGuiIO& io)
{
	ImFontConfig config;
	config.MergeMode = true;
	config.PixelSnapH = true;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	if(!io.Fonts->AddFontFromFileTTF("../fonts/fa-solid-900.ttf", 12.0f, &config, icon_ranges))
	{
		Logger::Err("Failed to load Font Awesome font!");
	}
	else
	{
		Logger::Log("Font Awesome loaded");
	}
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
	using namespace NoiseGenerator;
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
		if(ImGui::BeginMenu(ICON_FA_FILE " File"))
		{
			if(ImGui::BeginMenu(ICON_FA_SAVE " Save As"))
			{
				if(ImGui::MenuItem(ICON_FA_FILE_IMAGE " Export as PNG"))
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

				if(ImGui::MenuItem(ICON_FA_FILE_IMAGE " Export as TGA"))
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

				if(ImGui::MenuItem(ICON_FA_FILE_IMAGE " Export as BMP"))
				{
					if(noisePreview.IsInitialized())
					{
						if(ImageExporter::SaveBMP("export.bmp", noisePreview.GetTextureId(), noisePreview.GetWidth(), noisePreview.GetHeight()))
						{
							Logger::Log("Saved BMP: export.bmp");
						}
						else
						{
							Logger::Err("Failed to save BMP");
						}
					}
				}

				if(ImGui::MenuItem(ICON_FA_FILE_IMAGE " Export as JPG"))
				{
					if(noisePreview.IsInitialized())
					{
						if(ImageExporter::SaveJPG("export.jpg", noisePreview.GetTextureId(), noisePreview.GetWidth(), noisePreview.GetHeight(), 90))
						{
							Logger::Log("Saved JPG: export.jpg");
						}
						else
						{
							Logger::Err("Failed to save JPG");
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if(ImGui::MenuItem(ICON_FA_DOOR_OPEN " Exit", "Alt+F4"))
			{
				exit(0);
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu(ICON_FA_EYE " View"))
		{
			ImGui::MenuItem(ICON_FA_TERMINAL " Output Log", nullptr, &showOutputLog);
			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu(ICON_FA_QUESTION_CIRCLE " Help"))
		{
			if(ImGui::MenuItem(ICON_FA_INFO_CIRCLE " About"))
			{
#ifdef _WIN32
				ShellExecuteA(NULL, "open", "https://github.com/VaeDeveloper/NoiseGenerator", NULL, NULL, SW_SHOWNORMAL);

#else
				system("xdg-open https://github.com/VaeDeveloper/NoiseGenerator"); // Linux
				// macOS: system("open https://github.com/VaeDeveloper/NoiseGenerator");
#endif
				Logger::Log("Open Github Page");
			}
			
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::DockSpace(dockspace_id);
	ImGui::End();

	static bool lockResolution = false;
	static bool lockRoughness = false;
	static bool lockMarbling = false;
	static bool lockSeed = false;
	static bool lockLowFreq = false;
	static bool lockHighFreq = false;
	static bool lockTurbulence = false;
	static bool lockTurbRes = false;
	static bool lockTurbRoughness = false;
	static bool lockTurbLow = false;
	static bool lockTurbHigh = false;
	static bool lockTurbMarbling = false;
	static bool lockExpShift = false;
	static bool lockOffsetX = false;
	static bool lockOffsetY = false;
	static bool lockPreviewSize = false;

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
		static int previewSize = 1; // default: 512
		static const char* previewResolution[] = { "256", "512", "1024" };
		static const int previewSizes[] = { 256, 512, 1024 };
		
		// Layout
		ImGui::Text("Noise Settings");
		if(ImGui::Combo("Resolution", &resolutionIndex, resolutions, IM_ARRAYSIZE(resolutions)))
		{
			Logger::Log(std::string("Resolution = ") + resolutions[resolutionIndex]);
		}

		LabeledWidgetWithLock("##lockRough", &lockRoughness, [&] () {
			LogWidget("Roughness", &roughness, [&] () {
				return ImGui::SliderFloat("Roughness", &roughness, 0.01f, 1.0f);
				});
			});

		LabeledWidgetWithLock("##lockMarb", &lockMarbling, [&] () {
			LogWidget("Marbling", &marbling, [&] () {
				return ImGui::SliderFloat("Marbling", &marbling, 0.0f, 10.0f);
				});
			});

		LabeledWidgetWithLock("##lockSeed", &lockSeed, [&] () {
			LogWidget("Seed", &seed, [&] () {
				return ImGui::InputInt("Seed", &seed);
				});
			});

		LabeledWidgetWithLock("##lockLF", &lockLowFreq, [&] () {
			LogWidget("Low Freq Skip", &low_freq_skip, [&] () {
				return ImGui::SliderInt("Low Freq Skip", &low_freq_skip, 0, 12);
				});
			});

		LabeledWidgetWithLock("##lockHF", &lockHighFreq, [&] () {
			LogWidget("High Freq Skip", &high_freq_skip, [&] () {
				return ImGui::SliderInt("High Freq Skip", &high_freq_skip, 0, 12);
				});
			});

		ImGui::SeparatorText("Turbulence");

		LabeledWidgetWithLock("##lockTurb", &lockTurbulence, [&] () {
			LogWidget("Turbulence", &turbulence, [&] () {
				return ImGui::SliderFloat("Turbulence", &turbulence, 0.0f, 64.0f);
				});
			});

		LabeledWidgetWithLock("##lockTurbRes", &lockTurbRes, [&] () {
			LogWidget("Turbulence Res", &turbulence_res, [&] () {
				return ImGui::Combo("Turbulence Res", &turbulence_res, resolutions, IM_ARRAYSIZE(resolutions));
				});
			});

		LabeledWidgetWithLock("##lockTurbRough", &lockTurbRoughness, [&] () {
			LogWidget("Turbulence Roughness", &turbulence_roughness, [&] () {
				return ImGui::SliderFloat("Turbulence Roughness", &turbulence_roughness, 0.01f, 1.0f);
				});
			});

		LabeledWidgetWithLock("##lockTurbLF", &lockTurbLow, [&] () {
			LogWidget("Turb Low Freq Skip", &turbulence_low_freq_skip, [&] () {
				return ImGui::SliderInt("Turb Low Freq Skip", &turbulence_low_freq_skip, 0, 12);
				});
			});

		LabeledWidgetWithLock("##lockTurbHF", &lockTurbHigh, [&] () {
			LogWidget("Turb High Freq Skip", &turbulence_high_freq_skip, [&] () {
				return ImGui::SliderInt("Turb High Freq Skip", &turbulence_high_freq_skip, 0, 12);
				});
			});

		LabeledWidgetWithLock("##lockTurbMarb", &lockTurbMarbling, [&] () {
			LogWidget("Turbulence Marbling", &turbulence_marbling, [&] () {
				return ImGui::SliderFloat("Turbulence Marbling", &turbulence_marbling, 0.0f, 10.0f);
				});
			});

		LabeledWidgetWithLock("##lockExp", &lockExpShift, [&] () {
			LogWidget("Exp Shift", &turbulence_expshift, [&] () {
				return ImGui::SliderFloat("Exp Shift", &turbulence_expshift, -4.0f, 4.0f);
				});
			});

		LabeledWidgetWithLock("##lockX", &lockOffsetX, [&] () {
			LogWidget("Turb Offset X", &turbulence_offset_x, [&] () {
				return ImGui::SliderFloat("Turb Offset X", &turbulence_offset_x, -1.0f, 1.0f);
				});
			});

		LabeledWidgetWithLock("##lockY", &lockOffsetY, [&] () {
			LogWidget("Turb Offset Y", &turbulence_offset_y, [&] () {
				return ImGui::SliderFloat("Turb Offset Y", &turbulence_offset_y, -1.0f, 1.0f);
				});
			});

		ImGui::SeparatorText("Preview Output");
		if(ImGui::Combo("Preview Size", &previewSize, previewResolution, IM_ARRAYSIZE(previewResolution)))
		{
			// Logger::Log(std::string("Preview Size") + previewSizes[previewSize]);
		}

		int previewWidth = previewSizes[previewSize];
		int previewHeight = previewSizes[previewSize];
		noisePreview.SetPreviewWidth(previewWidth);
		noisePreview.SetPreviewHeight(previewHeight);

		if(!isGenerating)
		{
			if(ImGui::Button(" " ICON_FA_RANDOM " Randomize"))
			{
				if(!lockSeed) seed = rand();
				if(!lockRoughness) roughness = ImLerp(0.01f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockMarbling) marbling = ImLerp(0.0f, 10.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockLowFreq) low_freq_skip = rand() % 5;
				if(!lockHighFreq) high_freq_skip = rand() % 5;
				if(!lockTurbulence) turbulence = ImLerp(0.0f, 64.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockTurbRes) turbulence_res = rand() % IM_ARRAYSIZE(resolutions);
				if(!lockTurbRoughness) turbulence_roughness = ImLerp(0.01f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockTurbLow) turbulence_low_freq_skip = rand() % 5;
				if(!lockTurbHigh) turbulence_high_freq_skip = rand() % 5;
				if(!lockTurbMarbling) turbulence_marbling = ImLerp(0.0f, 10.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockExpShift) turbulence_expshift = ImLerp(-4.0f, 4.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockOffsetX) turbulence_offset_x = ImLerp(-1.0f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
				if(!lockOffsetY) turbulence_offset_y = ImLerp(-1.0f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
				Logger::Log("Randomize noise settings");
			}
			ImGui::SameLine();
			static int randomStyle = 0;
			const char* randomStyles[] = {
				"Full Random",
				"Controlled Chaos",
				"Organic",
				"Extreme",
				"Minimal",
			};

			if(ImGui::Button(ICON_FA_FLASK  " Mutate"))
			{

			}

			ImGui::SameLine();
			ImGui::Combo("Random Style", &randomStyle, randomStyles, IM_ARRAYSIZE(randomStyles));


			if(ImGui::Button(ICON_FA_PLAY " Generate 2D Noise") && !isGenerating)
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

			if(ImGui::Button(ICON_FA_TRASH " Clear"))
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