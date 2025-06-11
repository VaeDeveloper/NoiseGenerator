#include "ImageExporter.h"
#include <vector>
#include <glad/glad.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ThirdParty/stb_image_write.h>
#include "Logger/Logger.h"

bool ImageExporter::SavePNG(const std::string& filename, unsigned int textureId, int width, int height) 
{
	std::vector<unsigned char> pixels(width * height);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

	// Expand red channel to RGB for PNG (grayscale -> RGB)
	std::vector<unsigned char> rgbPixels(width * height * 3);
	for(int i = 0; i < width * height; ++i) {
		rgbPixels[i * 3 + 0] = pixels[i];
		rgbPixels[i * 3 + 1] = pixels[i];
		rgbPixels[i * 3 + 2] = pixels[i];
	}

	if(stbi_write_png(filename.c_str(), width, height, 3, rgbPixels.data(), width * 3)) 
	{
		Logger::Log("Saved PNG: " + filename);
		return true;
	}
	else 
	{
		Logger::Err("Failed to save PNG: " + filename);
		return false;
	}
}

bool ImageExporter::SaveTGA(const std::string& filename, unsigned int textureId, int width, int height) 
{
	std::vector<unsigned char> pixels(width * height);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

	// Expand red channel to grayscale TGA (1-channel)
	std::vector<unsigned char> grayPixels(width * height * 3);
	for(int i = 0; i < width * height; ++i) 
	{
		grayPixels[i * 3 + 0] = pixels[i];
		grayPixels[i * 3 + 1] = pixels[i];
		grayPixels[i * 3 + 2] = pixels[i];
	}

	if(stbi_write_tga(filename.c_str(), width, height, 3, grayPixels.data())) 
	{
		Logger::Log("Saved TGA: " + filename);
		return true;
	}
	else 
	{
		Logger::Err("Failed to save TGA: " + filename);
		return false;
	}
}