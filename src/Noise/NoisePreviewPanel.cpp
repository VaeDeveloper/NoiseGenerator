#include "NoisePreviewPanel.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <vector>

void NoisePreviewPanel::Init()
{
	glGenTextures(1, &textureId);
	textureInitialized = true;
	SetPreviewWidth(1024.0f);
	SetPreviewHeight(1024.0f);
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

	ImGui::BeginGroup(); // Start preview block
	ImGui::Image((ImTextureID)(intptr_t)textureId, ImVec2(previewWidht, previewHeight));
	ImGui::EndGroup();

	ImGui::SameLine();

	ImGui::BeginGroup(); // Info block
	ImGui::Text("  Parameter Info: ");
	ImGui::Spacing();
	ImGui::TextWrapped(" - Seed - Defines randomness. Same seed = same result.");
	ImGui::TextWrapped(" - Roughness - Controls fractal sharpness and detail.");
	ImGui::TextWrapped(" - Marbling - Adds sine-based pattern distortion.");
	ImGui::TextWrapped(" - Turbulence - Warps base noise with another noise layer.");
	ImGui::TextWrapped(" - Exp Shift - Modifies turbulence power exponentially.");
	ImGui::TextWrapped(" - Freq Skip - Filters specific frequency bands (low/high).");
	ImGui::TextWrapped(" - Offsets - Moves turbulence sampling in X/Y directions.");
	ImGui::EndGroup();

	ImGui::End(); // Noise Preview
}