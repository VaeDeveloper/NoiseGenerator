#pragma once

#include "Noise/NoisePreviewPanel.h"
#include <GLFW/glfw3.h>
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
	bool dockBuilt = false;
	NoisePreviewPanel noisePreview;

	bool showNoiseGenerator = true;
	bool showOutputLog = true;

	std::thread generationThread;
	std::atomic<bool> isGenerating = false;
	std::atomic<float> generationProgress = -1.0f;

	std::queue<std::function<void()>> uiTasks;
	std::mutex uiMutex;

	void QueueUITask(std::function<void()> task);
};