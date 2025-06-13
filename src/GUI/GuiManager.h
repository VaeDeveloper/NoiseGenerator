#pragma once

#include "Noise/NoisePreviewPanel.h"
#include <GLFW/glfw3.h>
#include <imgui_internal.h>
#include <imgui.h>
#include <thread>
#include <atomic>
#include <functional>
#include <queue>
#include <mutex>

class GuiManager
{
public:
	void Init(GLFWwindow* window);
	void Shutdown();
	void BeginFrame();
	void Render();
	void DrawUI();
	void SetNoiseData(float* data, int width, int height);


private:
	void QueueUITask(std::function<void()> task);
	void InitStyleConfig(const ImGuiIO& io);
	void InitFontConfig(const ImGuiIO& io);

private:
	bool dockBuilt = false;
	bool showOutputLog = true;
	bool showNoiseGenerator = true;
	NoisePreviewPanel noisePreview;

	std::thread generationThread;
	std::atomic<bool> isGenerating = false;
	std::atomic<float> generationProgress = -1.0f;

	std::mutex uiMutex;
	std::queue<std::function<void()>> uiTasks;
};