#include <imgui.h>
#include <imgui_internal.h>
#include "NoisePreviewPanelUI.h"

#include "GUI/GuiUtils.h"


#include "Logger/Logger.h"
#include "Logger/LoggerMacro.h"

DEFINE_LOG_CATEGORY(LogNoisePanel);



//static GLuint LoadComputeShader(const char* path)
//{
//	std::ifstream file(path);
//	if(!file)
//	{
//		std::cerr << "Shader file not found or failed to open: " << path << std::endl;
//		throw std::runtime_error("Failed to open shader file");
//	}
//	std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//	const char* src = source.c_str();
//
//	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
//	glShaderSource(shader, 1, &src, nullptr);
//	glCompileShader(shader);
//
//	GLint success;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
//	if(!success)
//	{
//		char log[512];
//		glGetShaderInfoLog(shader, 512, NULL, log);
//		std::cerr << "Shader compile failed:\n" << log << std::endl;
//		throw std::runtime_error(std::string("Shader compile error:\n") + log);
//	}
//
//	GLuint program = glCreateProgram();
//	glAttachShader(program, shader);
//	glLinkProgram(program);
//	glDeleteShader(shader);
//
//	return program;
//}
//
//
//void RunPerlinCompute(GLuint textureId, int width, int height, float seed, float roughness, int freq)
//{
//	static GLuint program = LoadComputeShader("D:/GitProject/NoiseGenerator/shaders/perlin.comp");
//	glUseProgram(program);
//
//	glUniform1f(glGetUniformLocation(program, "seed"), seed);
//	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
//	glUniform1i(glGetUniformLocation(program, "freq"), freq);
//
//	glBindImageTexture(0, textureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
//	glDispatchCompute((GLuint)ceil(width / 16.0f), (GLuint)ceil(height / 16.0f), 1);
//
//	GLenum err = glGetError();
//	if(err != GL_NO_ERROR)
//	{
//		NGLOG(LogNoisePanel, Error, "GL Error after dispatch: 0x");
//		std::cerr << "GL Error after dispatch: 0x" << std::hex << err << std::endl;
//	}
//
//	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
//}

void NoisePreviewPanelUI::Initialize()
{
	SetPreviewWidth(1024.0f);
	SetPreviewHeight(1024.0f);
	controller = std::make_unique<NoisePanelController>();
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