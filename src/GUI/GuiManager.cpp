
#include "GuiManager.h"
#include "LoggerUI.h"
#include "LoggerMacro.h"

#include "GuiUtils.h"
#include "StringUtils.h"
#include "IconRegistry.h"
#include "Constants.h"
#include "UIUtils.h"
#include "MenuBarController.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "NoiseTypes.h"
#include "NoiseGenerator.h"
#include "ImageExporter.h"
#include <random>
#include <type_traits>
#include <nfd.h>
#include <string>
#include <filesystem>  
#include "SettingsManager.h"

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


void GuiManager::Initialize(GLFWwindow* window)
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

	menuBar.Initialize();
	propertyUI.Initialize();
	noisePreview.Initialize();

	propertyUI.GetControllerRef().StartGeneration();

	// Delegates bind 
	menuBar.GetControllerRef().OnInfoPanelToggled.Bind(this, &GuiManager::InfoPanelVisibleHandle);
	propertyUI.GetControllerRef().OnNoiseReadyForUI.Bind(this, &GuiManager::SetNoiseData);
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
	float fontSize = SettingsManager::Get().GetFontSize();
	IconRegistry::InitializeIcons(io, fontSize);
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
	menuBar.Draw();

	menuBar.SetTextureData(
		noisePreview.GetTextureId(),
		noisePreview.GetControllerRef().GetModel()->GetWidth(),
		noisePreview.GetControllerRef().GetModel()->GetHeight());
}

void GuiManager::DrawDockTab()
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

}

void GuiManager::DrawPreviewPanel()
{
	noisePreview.Draw();
}

void GuiManager::DrawPropertyPanel()
{
	ImGui::Begin("Noise Generator", nullptr, ImGuiWindowFlags_NoTitleBar);
	SHOW_HIDDEN_TAB_BAR(ImGui::GetWindowDockID());
	propertyUI.Draw();
	ImGui::End();
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

void GuiManager::InfoPanelVisibleHandle(bool visible)
{
	noisePreview.SetInfoPanelVisible(visible);
}


void GuiManager::Shutdown()
{
	// Delegates unbind 
	menuBar.GetControllerRef().OnInfoPanelToggled.Unbind();
	propertyUI.GetControllerRef().OnNoiseReadyForUI.Unbind();

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
	if(data != nullptr) free(data);
}

void GuiManager::DrawUI()
{
	DrawDockTab();
	DrawPropertyPanel();
	DrawPreviewPanel();
	DrawOutputLog();

}
