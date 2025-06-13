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
	int GetWidth() const { return texWidth; }
	int GetHeight() const { return texHeight; }

	void SetPreviewWidth(float Width) { previewWidht = Width; }
	void SetPreviewHeight(float Height) { previewHeight = Height; }
private:
	GLuint textureId = 0;
	int texWidth = 0, texHeight = 0;
	bool textureInitialized = false;

	float previewWidht, previewHeight;
};