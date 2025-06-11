#pragma once
#include <glad/glad.h>

class NoisePreviewPanel
{
public:
	void Init();
	void UpdateTexture(const float* data, int width, int height);
	void Draw();

private:
	GLuint textureId = 0;
	int texWidth = 0, texHeight = 0;
	bool textureInitialized = false;
};