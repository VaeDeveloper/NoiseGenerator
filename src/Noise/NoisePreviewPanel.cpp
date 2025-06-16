#include "NoisePreviewPanel.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <vector>

#include "GUI/GuiUtils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include "Logger/Logger.h"

static GLuint LoadComputeShader(const char* path)
{
	std::ifstream file(path);
	if(!file)
	{
		std::cerr << "Shader file not found or failed to open: " << path << std::endl;
		throw std::runtime_error("Failed to open shader file");
	}
	std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	const char* src = source.c_str();

	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char log[512];
		glGetShaderInfoLog(shader, 512, NULL, log);
		std::cerr << "Shader compile failed:\n" << log << std::endl;
		throw std::runtime_error(std::string("Shader compile error:\n") + log);
	}

	GLuint program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);
	glDeleteShader(shader);

	return program;
}

enum class NoiseBackend
{
	CPU,
	GPU
};

NoiseBackend g_Backend = NoiseBackend::CPU;

void RunPerlinCompute(GLuint textureId, int width, int height, float seed, float roughness, int freq)
{
	static GLuint program = LoadComputeShader("D:/GitProject/NoiseGenerator/shaders/perlin.comp");
	glUseProgram(program);

	glUniform1f(glGetUniformLocation(program, "seed"), seed);
	glUniform1f(glGetUniformLocation(program, "roughness"), roughness);
	glUniform1i(glGetUniformLocation(program, "freq"), freq);

	glBindImageTexture(0, textureId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute((GLuint)ceil(width / 16.0f), (GLuint)ceil(height / 16.0f), 1);

	GLenum err = glGetError();
	if(err != GL_NO_ERROR)
	{
		Logger::Err("GL Error after dispatch: 0x");
		std::cerr << "GL Error after dispatch: 0x" << std::hex << err << std::endl;
	}

	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void NoisePreviewPanel::Init()
{
	glGenTextures(1, &textureId);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 1024, 1024);
	//glBindTexture(GL_TEXTURE_2D, textureId);
	//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 1024, 1024);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glBindTexture(GL_TEXTURE_2D, 0);

	textureInitialized = true;
	SetPreviewWidth(1024.0f);
	SetPreviewHeight(1024.0f);
}

void NoisePreviewPanel::UpdateTexture(const float* data, int width, int height)
{
	texWidth = width;
	texHeight = height;

	//if(g_Backend == NoiseBackend::GPU)
	//{
	//	int currentSeed = 42;
	//	float currentRoughness = 1.0f;
	//	int currentFreq = 4;
	//
	//
	//	RunPerlinCompute(textureId, width, height, (float)currentSeed, currentRoughness, currentFreq);
	//	return;
	//}

	std::vector<float> rgbData(width * height * 3);
	for(int i = 0; i < width * height; ++i)
	{
		rgbData[i * 3 + 0] = data[i]; // R
		rgbData[i * 3 + 1] = data[i]; // G
		rgbData[i * 3 + 2] = data[i]; // B
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
	//glTexSubImage2D( 
	//	GL_TEXTURE_2D,
	//	0, // mip level
	//	0, 0, width, height,
	//	GL_RGB,
	//	GL_FLOAT,
	//	rgbData.data()
	//);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, rgbData.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void NoisePreviewPanel::Draw()
{
	if(!textureInitialized) return;

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
	ImGui::BeginGroup(); // Start preview block

	ImGui::Image((ImTextureID)(intptr_t)textureId, ImVec2(previewWidht, previewHeight));
	ImGui::EndGroup();

	ImGui::SameLine();

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

	ImGui::End(); // Noise Preview
}