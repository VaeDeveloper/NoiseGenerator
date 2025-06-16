#pragma once
#include <glad/glad.h>

class NoisePreviewPanel
{
public:
	void Init();
	void UpdateTexture(const float* data, int width, int height);
	void Draw();

	bool IsInitialized() const { return textureInitialized; }
	GLuint GetTextureId() const { return textureId; }
	float GetWidth() const { return texWidth; }
	float GetHeight() const { return texHeight; }
	void SetShowInfoPanel(bool value) { showInfoPanel = value; }
	bool GetShowInfoPanel() const { return showInfoPanel; }

	void SetPreviewWidth(float Width) { previewWidht = Width; }
	void SetPreviewHeight(float Height) { previewHeight = Height; }
private:
	bool textureInitialized = false;
	bool showInfoPanel = true;
	float texWidth = 0;
	float texHeight = 0;
	float previewWidht = 0;
	float previewHeight = 0;
	GLuint textureId = 0;

};