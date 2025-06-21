#pragma once

#include <imgui_internal.h>
#include <imgui.h>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>
#include <mutex>


#include "MVC/View/NoisePreviewPanelUI.h"
#include "MVC/View/MenuBarUI.h"

#include <GLFW/glfw3.h>


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
	void QueueUITask(std::function<void()> task);
	void InitStyleConfig(const ImGuiIO& io);
	void InitFontConfig(const ImGuiIO& io);
	void InitThemeStyle();

	void DrawMenuBar();
	void DrawOutputLog();


	void SetAllLock(bool state);
	void MutateNoiseStyle(int style);
	void RandomizeNoise();
	void OpenURL(const char* url);
	void DrawResolutionComboWithLock();

private:
	bool bFullscreen = false;
	bool bDockBuilt = false;
	bool bShowOutputLog = true;
	MenuBarUI menuBar;
	NoisePreviewPanelUI noisePreview;

	std::thread generationThread;
	std::atomic<bool> isGenerating = false;

	std::atomic<bool> cancelRequested = false;
	std::atomic<float> generationProgress = -1.0f;

	std::mutex uiMutex;


	std::queue<std::function<void()>> uiTasks;


	//Lock properties
	bool lockResolution = false;
	bool lockRoughness = false;
	bool lockMarbling = false;
	bool lockSeed = false;
	bool lockLowFreq = false;
	bool lockHighFreq = false;
	bool lockTurbulence = false;
	bool lockTurbRes = false;
	bool lockTurbRoughness = false;
	bool lockTurbLow = false;
	bool lockTurbHigh = false;
	bool lockTurbMarbling = false;
	bool lockExpShift = false;
	bool lockOffsetX = false;
	bool lockOffsetY = false;
	bool lockPreviewSize = false;
	bool allLocked = false;

	static constexpr char* resolutions[] =
	{
		"8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
	};

	//Random properties
	int randomStyle = 0;

	static constexpr char* randomStyles[] = {
	"Full Random",
	"Controlled Chaos",
	"Organic",
	"Extreme",
	"Minimal",
	};

	// Base properties
	int seed = 42;
	int high_freq_skip = 0;
	int low_freq_skip = 0;
	int resolutionIndex = 3;
	float roughness = 0.5f;
	float marbling = 0.0f;

	// Turbulence
	int turbulence_res = 2;
	int turbulence_low_freq_skip = 0;
	int turbulence_high_freq_skip = 0;
	float turbulence = 0.0f;
	float turbulence_roughness = 0.5f;
	float turbulence_marbling = 0.0f;
	float turbulence_expshift = 0.0f;
	float turbulence_offset_x = 0.0f;
	float turbulence_offset_y = 0.0f;

};