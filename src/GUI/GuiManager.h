#pragma once
#include "Noise/NoisePreviewPanel.h"
#include <GLFW/glfw3.h>


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
};