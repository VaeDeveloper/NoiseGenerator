


#ifdef APIENTRY
#undef APIENTRY
#endif
#include <windows.h>

#include "GuiManager.h"
#include "Logger/LoggerUI.h"
#include "Logger/LoggerMacro.h"
#include "GUI/GuiUtils.h"
#include "Utils/StringUtils.h"
#include "IconRegistry.h"
#include "Utils/Constants.h"
#include "Utils/UIUtils.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Noise/NoiseTypes.h"
#include "Noise/NoiseGenerator.h"
#include "Image/ImageExporter.h"
#include <random>
#include <type_traits>
#include <nfd.h>
#include <string>
#include <filesystem>  
#include "Config/SettingsManager.h"

namespace fs = std::filesystem;

DEFINE_LOG_CATEGORY(LogGUI);


std::string EnsureExtension(const std::string& path, const std::string& ext) 
{
	if(!NG::StringUtils::EndsWith(path, ext) && !NG::StringUtils::EndsWith(path, NG::StringUtils::ToUpper(ext))) 
	{
		return path + ext;
	}
	return path;
}


namespace NG
{
	constexpr int spaceOffset = 24;
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

	NGLOG(LogGUI, Info, "ImGui initialized");

	noisePreview.Initialize();
}

void GuiManager::InitStyleConfig(const ImGuiIO& io)
{

	InitThemeStyle();
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

	std::vector<fs::path> fontSearchPaths =
	{
		fs::current_path() / "fonts" / "fa-solid-900.ttf",
		fs::current_path() / ".." / "fonts" / "fa-solid-900.ttf",
		fs::current_path() / ".." / ".." / "fonts" / "fa-solid-900.ttf"
	};

	fs::path fontPath;
	for(const auto& path : fontSearchPaths)
	{
		if(fs::exists(path))
		{
			fontPath = fs::weakly_canonical(path);
			break;
		}
	}

	if(fontPath.empty())
	{
		NGLOG(LogGUI, Error, "Font Awesome not found in known paths!");
		NGLOG(LogGUI, Info, "Working directory: " + fs::current_path().string());
		return;
	}

	float fontSize = SettingsManager::Get().GetFontSize();
	NGLOG(LogGUI, Info, "Font size: " + NG::StringUtils::ToString(fontSize));

	if(!io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), fontSize, &config, icon_ranges))
	{
		NGLOG(LogGUI, Error, "Failed to load Font Awesome font from: " + fontPath.string());
	}
	else
	{
		NGLOG(LogGUI, Info, "Font Awesome loaded from: " + fontPath.string());
	}
}
void GuiManager::InitThemeStyle()
{
	std::string theme = SettingsManager::Get().GetTheme();

	if(theme == "dark")
	{
		ImGui::StyleColorsDark();
	}
	else if(theme == "light")
	{
		ImGui::StyleColorsLight();
	}
	else if(theme == "classic")
	{
		ImGui::StyleColorsClassic();
	}
	else
	{
		ImGui::StyleColorsDark();
	}
}

void GuiManager::DrawMenuBar()
{
	if(ImGui::BeginMenuBar())
	{
		if(ImGui::BeginMenu(WITH_ICON("File", "File")))
		{
			if(ImGui::BeginMenu(WITH_ICON("Save", "Save As")))
			{

				if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as PNG")))
				{
					if(noisePreview.IsInitialized())
					{
						nfdchar_t* outPath = nullptr;
						nfdresult_t result = NFD_SaveDialog("png", nullptr, &outPath);
						if(result == NFD_OKAY)
						{
							std::string pathStr = EnsureExtension(outPath, ".png");
							bool ok = ImageExporter::SavePNG(
								pathStr.c_str(),
								noisePreview.GetTextureId(),
								static_cast<int>(noisePreview.GetWidth()),
								static_cast<int>(noisePreview.GetHeight())
							);

							if(ok)
							{
								NGLOG(LogGUI, Info, "Saved PNG: " + pathStr);
							}
							else
							{
								NGLOG(LogGUI, Error, "Failed to save PNG: " + pathStr);
							}

							free(outPath);
						}
						else if(result == NFD_CANCEL)
						{
							NGLOG(LogGUI, Info, "Save dialog canceled.");
						}
						else
						{
							NGLOG(LogGUI, Error, "NFD Error: " + std::string(NFD_GetError()));
						}
					}
				}

				if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as TGA"))) 
				{
					if(noisePreview.IsInitialized()) 
					{
						nfdchar_t* outPath = nullptr;
						nfdresult_t result = NFD_SaveDialog("tga", nullptr, &outPath);
						if(result == NFD_OKAY) 
						{
							std::string pathStr = EnsureExtension(outPath, ".tga");
							bool ok = ImageExporter::SaveTGA(pathStr.c_str(),
								noisePreview.GetTextureId(),
								static_cast<int>(noisePreview.GetWidth()),
								static_cast<int>(noisePreview.GetHeight()));

							if(ok)
							{
								NGLOG(LogGUI, Info, "Saved TGA: " + pathStr);
							}
							else
							{
								NGLOG(LogGUI, Error, "Failed to save TGA: " + pathStr);
							}
							free(outPath);
						}
						else if(result == NFD_CANCEL)
						{
							NGLOG(LogGUI, Info, "Save dialog canceled.");
						}
						else
						{
							NGLOG(LogGUI, Error, "NFD Error: " + std::string(NFD_GetError()));
						}
					}
				}

				if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as BMP")))
				{
					if(noisePreview.IsInitialized())
					{
						nfdchar_t* outPath = nullptr;
						nfdresult_t result = NFD_SaveDialog("bmp", nullptr, &outPath);
						if(result == NFD_OKAY)
						{
							std::string pathStr = EnsureExtension(outPath, ".bmp");

							bool ok = ImageExporter::SaveBMP(
								pathStr.c_str(),
								noisePreview.GetTextureId(),
								static_cast<int>(noisePreview.GetWidth()),
								static_cast<int>(noisePreview.GetHeight()));

							if(ok)
							{
								NGLOG(LogGUI, Info, "Saved BMP: " + pathStr);
							}
							else
							{
								NGLOG(LogGUI, Error, "Failed to save BMP: " + pathStr);
							}

							free(outPath);
						}
						else if(result == NFD_CANCEL)
						{
							NGLOG(LogGUI, Info, "Save dialog canceled.");
						}
						else
						{
							NGLOG(LogGUI, Error, std::string("NFD Error: ") + NFD_GetError());
						}
					}
				}

				if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as JPG")))
				{
					if(noisePreview.IsInitialized())
					{
						nfdchar_t* outPath = nullptr;
						nfdresult_t result = NFD_SaveDialog("jpg", nullptr, &outPath);
						if(result == NFD_OKAY)
						{
							std::string pathStr = EnsureExtension(outPath, ".jpg");

							bool ok = ImageExporter::SaveJPG(
								pathStr.c_str(),
								noisePreview.GetTextureId(),
								static_cast<int>(noisePreview.GetWidth()),
								static_cast<int>(noisePreview.GetHeight()),
								90 // JPEG quality
							);

							if(ok)
							{
								NGLOG(LogGUI, Info, "Saved JPG: " + pathStr);
							}
							else
							{
								NGLOG(LogGUI, Error, "Failed to save JPG: " + pathStr);
							}

							free(outPath);
						}
						else if(result == NFD_CANCEL)
						{
							NGLOG(LogGUI, Info, "Save dialog canceled.");
						}
						else
						{
							NGLOG(LogGUI, Error, std::string("NFD Error: ") + NFD_GetError());
						}
					}
				}

				ImGui::EndMenu();
			}

			ImGui::Separator();

			if(ImGui::MenuItem(WITH_ICON("DoorOpen", "Exit"), "Alt+F4"))
			{
				exit(0);
			}

			ImGui::EndMenu();
		}

		if(ImGui::BeginMenu(WITH_ICON("Eye", "View")))
		{
			ImGui::MenuItem(WITH_ICON("Terminal", "Output Log"), nullptr, &bShowOutputLog);
			if(ImGui::MenuItem(WITH_ICON("Expand", "Fullscreen"), "F11", &bFullscreen))
			{
				GLFWwindow* window = glfwGetCurrentContext();
				if(bFullscreen)
				{
					const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
					glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
				}
				else
				{
					int width = SettingsManager::Get().GetWindowWidth();
					int height = SettingsManager::Get().GetWindowHeight();;
					glfwSetWindowMonitor(window, nullptr, 100, 100, width, height, 0);
				}
			}

			bool showInfo = noisePreview.GetShowInfoPanel();
			if(ImGui::MenuItem(WITH_ICON("InfoCircle", "Info Panel"), nullptr, &showInfo))
			{
				noisePreview.SetShowInfoPanel(showInfo);
			}

			ImGui::EndMenu();
		}

#ifdef _DEBUG 
		if(ImGui::BeginMenu(WITH_ICON("QuestionCircle", "Help")))
		{
			if(ImGui::MenuItem(WITH_ICON("InfoCircle", "About")))
			{
				OpenURL(NG::GitURL);
			}

			ImGui::EndMenu();
		}
#endif // DEBUG

		ImGui::EndMenuBar();
	}
}

void GuiManager::DrawOutputLog()
{
	if(bShowOutputLog)
	{
		ImGui::Begin("Output Log", nullptr, ImGuiWindowFlags_NoTitleBar);
		SHOW_HIDDEN_TAB_BAR(ImGui::GetWindowDockID());
		DrawLoggerWindow();
		ImGui::End();
	}
}

void GuiManager::SetAllLock(bool state)
{
	lockRoughness = state;
	lockMarbling = state;
	lockSeed = state;
	lockLowFreq = state;
	lockHighFreq = state;
	lockTurbulence = state;
	lockTurbRes = state;
	lockTurbRoughness = state;
	lockTurbLow = state;
	lockTurbHigh = state;
	lockTurbMarbling = state;
	lockExpShift = state;
	lockOffsetX = state;
	lockOffsetY = state;
}

void GuiManager::MutateNoiseStyle(int style)
{
	auto randf = [] (float min, float max) {
		return ImLerp(min, max, static_cast<float>(rand()) / RAND_MAX);
		};

	switch(style)
	{
	case 0: // Full Random
		if(!lockSeed) seed = rand();
		if(!lockRoughness) roughness = randf(0.01f, 1.0f);
		if(!lockMarbling) marbling = randf(0.0f, 10.0f);
		if(!lockLowFreq) low_freq_skip = rand() % 5;
		if(!lockHighFreq) high_freq_skip = rand() % 5;
		if(!lockTurbulence) turbulence = randf(0.0f, 64.0f);
		if(!lockTurbRes) turbulence_res = rand() % IM_ARRAYSIZE(resolutions);
		if(!lockTurbRoughness) turbulence_roughness = randf(0.01f, 1.0f);
		if(!lockTurbLow) turbulence_low_freq_skip = rand() % 5;
		if(!lockTurbHigh) turbulence_high_freq_skip = rand() % 5;
		if(!lockTurbMarbling) turbulence_marbling = randf(0.0f, 10.0f);
		if(!lockExpShift) turbulence_expshift = randf(-4.0f, 4.0f);
		if(!lockOffsetX) turbulence_offset_x = randf(-1.0f, 1.0f);
		if(!lockOffsetY) turbulence_offset_y = randf(-1.0f, 1.0f);
		break;

	case 1: // Controlled Chaos 
		if(!lockRoughness) roughness *= randf(0.9f, 1.1f);
		if(!lockMarbling) marbling += randf(-0.5f, 0.5f);
		if(!lockLowFreq) low_freq_skip = (low_freq_skip + rand() % 3) % 5;
		if(!lockHighFreq) high_freq_skip = (high_freq_skip + rand() % 3) % 5;
		if(!lockTurbulence) turbulence *= randf(0.8f, 1.2f);
		if(!lockTurbRoughness) turbulence_roughness *= randf(0.8f, 1.2f);
		if(!lockTurbMarbling) turbulence_marbling += randf(-0.5f, 0.5f);
		break;

	case 2: // Organic 
		if(!lockRoughness) roughness = randf(0.3f, 0.6f);
		if(!lockMarbling) marbling = randf(0.1f, 2.0f);
		if(!lockTurbulence) turbulence = randf(0.0f, 4.0f);
		if(!lockTurbMarbling) turbulence_marbling = randf(0.0f, 2.0f);
		break;

	case 3: // Extreme 
		if(!lockRoughness) roughness = randf(0.01f, 1.0f);
		if(!lockMarbling) marbling = randf(5.0f, 10.0f);
		if(!lockTurbulence) turbulence = randf(32.0f, 64.0f);
		if(!lockTurbMarbling) turbulence_marbling = randf(5.0f, 10.0f);
		if(!lockExpShift) turbulence_expshift = randf(-4.0f, 4.0f);
		break;

	case 4: // Minimal 
		if(!lockRoughness) roughness = randf(0.01f, 0.05f);
		if(!lockMarbling) marbling = randf(0.0f, 1.0f);
		if(!lockTurbulence) turbulence = 0.0f;
		if(!lockTurbMarbling) turbulence_marbling = 0.0f;
		if(!lockTurbRoughness) turbulence_roughness = 0.01f;
		break;
	}

}

void GuiManager::RandomizeNoise()
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
	NGLOG(LogGUI, Info, "Randomized noise settings");
}

void GuiManager::OpenURL(const char* url)
{
#ifdef _WIN32
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#elif __APPLE__
	std::string command = "open ";
	command += url;
	system(command.c_str());
#else
	std::string command = "xdg-open ";
	command += url;
	system(command.c_str());
#endif
	NGLOG(LogGUI, Info, std::string("Opened URL: ") + url);
}

void GuiManager::DrawResolutionComboWithLock()
{
	NG::LogWidget("Resolution", &resolutions[resolutionIndex], [&] ()
		{
			ImVec2 oldPadding = ImGui::GetStyle().FramePadding;
			float targetHeight = 22.0f;
			float textHeight = ImGui::GetTextLineHeight();
			ImGui::GetStyle().FramePadding.y = (targetHeight - textHeight) * 0.5f;
			bool changed = ImGui::Combo("Resolution", &resolutionIndex, resolutions, IM_ARRAYSIZE(resolutions));
			ImGui::GetStyle().FramePadding = oldPadding;
			return changed;
		});

	ImGui::SameLine();
	float spaceOffset = 24;
	float space = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space - 80);
	ImGui::PushID("##LockAllBtn");
	if(ImGui::Button(allLocked ? "Unlock All" : "Lock All", ImVec2(80, 22.0f)))
	{
		allLocked = !allLocked;
		SetAllLock(allLocked);
		NGLOG(LogGUI, Info, std::string(allLocked ? "All parameters locked" : "All parameters unlocked"));

	}
	ImGui::PopID();
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
	if(!bDockBuilt)
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
		ImGuiID dock_right = dock_main_id;

		ImGui::DockBuilderDockWindow("Noise Generator", dock_left);
		ImGui::DockBuilderDockWindow("Noise Preview", dock_right);
		ImGui::DockBuilderDockWindow("Output Log", dock_bottom);

		ImGui::DockBuilderFinish(dockspace_id);
		bDockBuilt = true;
	}

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags host_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_MenuBar;


	/* Menu Bar */
	ImGui::Begin("DockSpaceRoot", nullptr, host_flags);
	DrawMenuBar();
	ImGui::DockSpace(dockspace_id);
	ImGui::End();

	/** Generate Action */
	ImGui::Begin("Noise Generator", nullptr, ImGuiWindowFlags_NoTitleBar);
	SHOW_HIDDEN_TAB_BAR(ImGui::GetWindowDockID());
	ImGui::SeparatorText("Generate Action");
	ImGui::BeginDisabled(isGenerating);
	if(ImGui::Button(WITH_ICON("Play", "Generate 2D Noise"), ImVec2(200, 30)) && !isGenerating)
	{
		int res = 8 << resolutionIndex;
		NoiseProperties props = {};
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
		cancelRequested = false;
		generationThread = std::thread([this, res, props] () 
			{
				float* noise = NG::PerlinNoise2D(res, &props, [this] (float progress) 
					{
						this->generationProgress = progress;
						return !this->cancelRequested; 
					});

				if(noise != nullptr) 
				{
					this->QueueUITask([this, noise, res] () 
						{
							this->SetNoiseData(noise, res, res);
							//Logger::Log("Generated 2D noise preview");
							free(noise);
							this->generationProgress = -1.0f;
							this->isGenerating = false;
						});
				}
				else 
				{
					this->QueueUITask([this] () 
						{
							NGLOG(LogGUI, Warning, "Noise generation cancelled.");
							this->SetNoiseData(nullptr, 0, 0);
							this->generationProgress = -1.0f;
							this->isGenerating = false;
						});
				}
			});

		generationThread.detach();
	}
	ImGui::EndDisabled();
	ImGui::SameLine();

	if(ImGui::Button(WITH_ICON("TimesCircle", "Cancel"), ImVec2(120, 30))) {
		cancelRequested = true;
		NGLOG(LogGUI, Warning, "Cancel requested by user");
	}


	ImGui::SameLine();

	ImGui::BeginDisabled(isGenerating);
	if(ImGui::Button(WITH_ICON("Trash", "Clear"), ImVec2(120, 30)))
	{
		this->SetNoiseData(nullptr, 0, 0);
		NGLOG(LogGUI, Warning, "Preview cleared");
	}
	ImGui::EndDisabled();


	// Random 
	ImGui::TextUnformatted(WITH_ICON("Dice", "Randomize Action"));
	ImGui::Separator();
	if(ImGui::Button(WITH_ICON("Random", "Randomize")))
	{
		RandomizeNoise();
	}
	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("Flask", "Mutate")))
	{
		MutateNoiseStyle(randomStyle);
		NGLOG(LogGUI, Info, "Mutated noise settings using style: " + std::string(randomStyles[randomStyle]));
	}

	ImGui::SameLine();
	ImGui::SetNextItemWidth(150);
	ImGui::Combo("Random Style", &randomStyle, randomStyles, IM_ARRAYSIZE(randomStyles));

	// Layout Noise Settings
	ImGui::TextUnformatted(WITH_ICON("SlidersH", "Noise Settings"));
	ImGui::Separator();
	DrawResolutionComboWithLock();
	
	/* clang-format off */
	/* Compact formatting style to improve readability of nested lambdas!!!*/
	NG::LabeledWidgetWithLock("##lockRough", &lockRoughness, [&] () {
		NG::LogWidget("Roughness", &roughness, [&] () {
			return ImGui::SliderFloat("Roughness", &roughness, 0.01f, 1.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockMarb", &lockMarbling, [&] () {
		NG::LogWidget("Marbling", &marbling, [&] () {
			return ImGui::SliderFloat("Marbling", &marbling, 0.0f, 10.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockSeed", &lockSeed, [&] () {
		NG::LogWidget("Seed", &seed, [&] () {
			return ImGui::InputInt("Seed", &seed);
			});
		});

	NG::LabeledWidgetWithLock("##lockLF", &lockLowFreq, [&] () {
		NG::LogWidget("Low Freq Skip", &low_freq_skip, [&] () {
			return ImGui::SliderInt("Low Freq Skip", &low_freq_skip, 0, 12);
			});
		});

	NG::LabeledWidgetWithLock("##lockHF", &lockHighFreq, [&] () {
		NG::LogWidget("High Freq Skip", &high_freq_skip, [&] () {
			return ImGui::SliderInt("High Freq Skip", &high_freq_skip, 0, 12);
			});
		});

	ImGui::TextUnformatted(WITH_ICON("Wind", "Turbulence"));
	ImGui::Separator();
	NG::LabeledWidgetWithLock("##lockTurb", &lockTurbulence, [&] () {
		NG::LogWidget("Turbulence", &turbulence, [&] () {
			return ImGui::SliderFloat("Turbulence", &turbulence, 0.0f, 64.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbRes", &lockTurbRes, [&] () {
		NG::LogWidget("Turbulence Res", &turbulence_res, [&] () {
			return ImGui::Combo("Turbulence Res", &turbulence_res, resolutions, IM_ARRAYSIZE(resolutions));
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbRough", &lockTurbRoughness, [&] () {
		NG::LogWidget("Turbulence Roughness", &turbulence_roughness, [&] () {
			return ImGui::SliderFloat("Turbulence Roughness", &turbulence_roughness, 0.01f, 1.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbLF", &lockTurbLow, [&] () {
		NG::LogWidget("Turb Low Freq Skip", &turbulence_low_freq_skip, [&] () {
			return ImGui::SliderInt("Turb Low Freq Skip", &turbulence_low_freq_skip, 0, 12);
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbHF", &lockTurbHigh, [&] () {
		NG::LogWidget("Turb High Freq Skip", &turbulence_high_freq_skip, [&] () {
			return ImGui::SliderInt("Turb High Freq Skip", &turbulence_high_freq_skip, 0, 12);
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbMarb", &lockTurbMarbling, [&] () {
		NG::LogWidget("Turbulence Marbling", &turbulence_marbling, [&] () {
			return ImGui::SliderFloat("Turbulence Marbling", &turbulence_marbling, 0.0f, 10.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockExp", &lockExpShift, [&] () {
		NG::LogWidget("Exp Shift", &turbulence_expshift, [&] () {
			return ImGui::SliderFloat("Exp Shift", &turbulence_expshift, -4.0f, 4.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockX", &lockOffsetX, [&] () {
		NG::LogWidget("Turb Offset X", &turbulence_offset_x, [&] () {
			return ImGui::SliderFloat("Turb Offset X", &turbulence_offset_x, -1.0f, 1.0f);
			});
		});

	NG::LabeledWidgetWithLock("##lockY", &lockOffsetY, [&] () {
		NG::LogWidget("Turb Offset Y", &turbulence_offset_y, [&] () {
			return ImGui::SliderFloat("Turb Offset Y", &turbulence_offset_y, -1.0f, 1.0f);
			});
		});
	/*--------------------------------------------------------------------------------------------------*/

	if(isGenerating)
	{
		ImGui::ProgressBar(generationProgress, ImVec2(-1.0f, 0.0f), generationProgress >= 1.0f ? "Done" : "Generating...");
	}
	ImGui::End();

	noisePreview.Draw();

	// Log window
	DrawOutputLog();

	{
		std::lock_guard<std::mutex> lock(uiMutex);
		while(!uiTasks.empty())
		{
			uiTasks.front()(); // execute
			uiTasks.pop();
		}
	}

	if(ImGui::IsKeyPressed(ImGuiKey_F11, false))
	{
		bFullscreen = !bFullscreen;
		GLFWwindow* window = glfwGetCurrentContext();
		if(bFullscreen)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			float Width = SettingsManager::Get().GetWindowWidth();
			float Height = SettingsManager::Get().GetWindowHeight();
			glfwSetWindowMonitor(window, nullptr, 100, 100, Width, Height, 0);
		}
	}

}

/*  */
