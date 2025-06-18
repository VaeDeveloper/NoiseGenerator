#pragma once


#include <glad/glad.h>

class NoisePreviewPanel
{
public:
	void Initialize();
	void UpdateTexture(const float* data, int width, int height);
	void Draw();

	bool IsInitialized() const { return bTextureInitialized; }
	GLuint GetTextureId() const { return textureId; }
	float GetWidth() const { return texWidth; }
	float GetHeight() const { return texHeight; }
	void SetShowInfoPanel(bool value) { showInfoPanel = value; }
	bool GetShowInfoPanel() const { return showInfoPanel; }

	void SetPreviewWidth(float Width) { previewWidth = Width; }
	void SetPreviewHeight(float Height) { previewHeight = Height; }
private:
	bool bTextureInitialized = false;
	bool showInfoPanel = true;
	float texWidth = 0;
	float texHeight = 0;
	float previewWidth = 0;
	float previewHeight = 0;
	GLuint textureId = 0;

};