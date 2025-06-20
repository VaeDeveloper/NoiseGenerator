#pragma once
#include "Controller/NoisePanelController.h"



enum class NoiseBackend
{
	CPU,
	GPU
};

//extern NoiseBackend g_Backend;

class NoisePreviewPanelUI
{
public:
	void Initialize();
	void UpdateTexture(const float* data, int width, int height);
	void Draw();
	void SetShowInfoPanel(bool value) { showInfoPanel = value; }
	bool GetShowInfoPanel() const { return showInfoPanel; }

	void SetPreviewWidth(float Width) { previewWidth = Width; }
	void SetPreviewHeight(float Height) { previewHeight = Height; }

	GLuint GetTextureId() const {
		return controller ? controller->GetModel()->GetTextureId() : 0;
	}
	
	bool IsInitialized() const {
		return controller && controller->GetModel()->IsInitialized();
	}

	NoisePanelController* GetController() const {
		return controller.get();
	}
private:

	bool showInfoPanel = true;
	float previewWidth = 0;
	float previewHeight = 0;

	std::unique_ptr<NoisePanelController> controller;
};