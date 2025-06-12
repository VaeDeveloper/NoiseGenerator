#include "NoisePreviewPanel.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <vector>

void NoisePreviewPanel::Init()
{
	glGenTextures(1, &textureId);
	textureInitialized = true;
}

void NoisePreviewPanel::UpdateTexture(const float* data, int width, int height)
{
	texWidth = width;
	texHeight = height;

	std::vector<float> rgbData(width * height * 3);
	for(int i = 0; i < width * height; ++i)
	{
		rgbData[i * 3 + 0] = data[i]; // R
		rgbData[i * 3 + 1] = data[i]; // G
		rgbData[i * 3 + 2] = data[i]; // B
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, rgbData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void NoisePreviewPanel::Draw()
{
	if(!textureInitialized) return;
	ImGui::Begin("Noise Preview", nullptr, ImGuiWindowFlags_NoTitleBar);
	ImGuiID dock_id = ImGui::GetWindowDockID();
	if(dock_id != 0)
	{
		if(ImGuiDockNode* node = ImGui::DockBuilderGetNode(dock_id))
		{
			if(node->WantHiddenTabBarUpdate)
				node->WantHiddenTabBarToggle = true;
		}
	} 
	ImGui::Text("Preview %dx%d", texWidth, texHeight);
	ImGui::Image((ImTextureID)(intptr_t)textureId, ImVec2(1024, 1024));
	ImGui::End();
}