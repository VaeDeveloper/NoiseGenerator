#pragma once

#include <imgui_internal.h>
#include <imgui.h>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>
#include <mutex>


#include "NoisePropertyUI.h"
#include "NoisePreviewPanelUI.h"

// todo 
#include "MenuBarUI.h"

class GuiManager
{
public:
	void Initialize(GLFWwindow* window);
	void Shutdown();
	void BeginFrame();
	void Render();
	void DrawUI();
	void SetNoiseData(float* data, int width, int height);

private:
	void InitStyleConfig(const ImGuiIO& io);
	void InitFontConfig(const ImGuiIO& io);
	void InitThemeStyle();

	void DrawMenuBar();
	void DrawDockTab();
	void DrawPreviewPanel();
	void DrawPropertyPanel();
	void DrawOutputLog();
	void InfoPanelVisibleHandle(bool visible);


private:
	bool bDockBuilt = false;
	bool bShowOutputLog = true;


	MenuBarUI menuBar;
	NoisePropertyUI propertyUI;
	NoisePreviewPanelUI noisePreview;

};
