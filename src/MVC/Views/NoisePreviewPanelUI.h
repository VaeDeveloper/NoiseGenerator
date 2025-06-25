#pragma once
#include "MVC/Controllers/NoisePanelController.h"
#include "IView.h"
#include "BoilerplateMacro.h"




class NoisePreviewPanelUI : public IView, public IViewTyped<NoisePanelController>
{
	IMPL_TYPED_CONTROLLER(NoisePanelController, controller);
public:
	virtual void Initialize() override;
	virtual void Draw() override;


	void UpdateTexture(const float* data, int width, int height);
	void SetShowInfoPanel(bool value) { showInfoPanel = value; }
	bool GetShowInfoPanel() const { return showInfoPanel; }

	void SetPreviewWidth(float Width) { previewWidth = Width; }
	void SetPreviewHeight(float Height) { previewHeight = Height; }

	GLuint GetTextureId() const 
	{
		return controller ? controller->GetModel()->GetTextureId() : 0;
	}
	
	bool IsInitialized() const 
	{
		return controller && controller->GetModel()->IsInitialized();
	}

	void SetInfoPanelVisible(bool visible)
	{
		showInfoPanel = visible;
	}
private:
	bool showInfoPanel = true;
	float previewWidth = 0;
	float previewHeight = 0;

	std::shared_ptr<NoisePanelController> controller;
};