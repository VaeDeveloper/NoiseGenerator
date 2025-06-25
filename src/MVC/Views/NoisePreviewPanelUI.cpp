#include <imgui.h>
#include <imgui_internal.h>
#include "NoisePreviewPanelUI.h"

#include "GuiUtils.h"
#include "Logger.h"
#include "LoggerMacro.h"

DEFINE_LOG_CATEGORY(LogNoisePanel);

void NoisePreviewPanelUI::Initialize()
{
	try
	{
		controller = std::make_shared<NoisePanelController>();
		NGLOG(LogNoisePanel, Info, "Noise Panel Controller Initialized");
	}
	catch(const std::bad_alloc& e)
	{
		NGLOG(LogNoisePanel, Error, std::string("Failed to allocate NoisePanelController: ") + e.what());
		controller = nullptr;
	}
	catch(const std::exception& e)
	{
		NGLOG(LogNoisePanel, Error, std::string("Unexpected error during controller creation: ") + e.what());
		controller = nullptr;
	}

	SetPreviewWidth(1024.0f);
	SetPreviewHeight(1024.0f);
}

void NoisePreviewPanelUI::UpdateTexture(const float* data, int width, int height)
{
	controller->UploadNoise(data, width, height);
}

void NoisePreviewPanelUI::Draw()
{
	ImGui::Begin("Noise Preview", nullptr, ImGuiWindowFlags_NoTitleBar);
	SHOW_HIDDEN_TAB_BAR(ImGui::GetWindowDockID());

	// Preview control
	static int previewSize = 1; // default: 512
	static const char* previewResolution[] = { "256", "512", "1024" };
	static const int previewSizes[] = { 256, 512, 1024 };
	
	ImGui::Text("Preview size:");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(300); 
	ImGui::Combo("##preview_size", &previewSize, previewResolution, IM_ARRAYSIZE(previewResolution));

	int previewWidth = previewSizes[previewSize];
	int previewHeight = previewSizes[previewSize];
	SetPreviewWidth(previewWidth);
	SetPreviewHeight(previewHeight);
	ImGui::BeginGroup(); 
	ImGui::Image((ImTextureID)(intptr_t)controller->GetModel()->GetTextureId(), ImVec2(static_cast<int>(previewWidth), static_cast<int>(previewHeight)));
	ImGui::EndGroup();

	ImGui::SameLine();

	if(showInfoPanel)
	{
		ImGui::BeginGroup(); // Info block
		ImGui::Text("  Parameter Info: ");
		ImGui::Spacing();
		ImGui::TextWrapped(" - Seed: Defines randomness. Same seed = same result.");
		ImGui::TextWrapped(" - Roughness: Controls fractal sharpness and detail.");
		ImGui::TextWrapped(" - Marbling: Adds sine-based pattern distortion.");
		ImGui::TextWrapped(" - Turbulence: Warps base noise with another noise layer.");
		ImGui::TextWrapped(" - Exp Shift: Modifies turbulence power exponentially.");
		ImGui::TextWrapped(" - Freq Skip: Filters specific frequency bands (low/high).");
		ImGui::TextWrapped(" - Offsets: Moves turbulence sampling in X/Y directions.");
		ImGui::Spacing();
		ImGui::Text("  Tips & Shortcuts:");
		ImGui::Spacing();
		ImGui::TextWrapped(" - F11: Toggle fullscreen mode.");
		ImGui::TextWrapped(" - Alt+F4: Exit application.");
		ImGui::TextWrapped(" - Export menu: Save previews in PNG/TGA/BMP/JPG formats.");
		ImGui::TextWrapped(" - Lock buttons: Prevent randomization of specific parameters.");
		ImGui::EndGroup();
	}

	ImGui::End(); // Noise Preview
}