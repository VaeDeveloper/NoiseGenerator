#include "ImageExporter.h"
#include <vector>

#ifdef APIENTRY
#undef APIENTRY
#endif
#include <glad/glad.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ThirdParty/stb_image_write.h>

#include "Logger/LoggerMacro.h"

DEFINE_LOG_CATEGORY(LogExport);

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
		NGLOG(LogExport, Info, "Saved PNG: " + filename);
		return true;
	}
	else
	{
		NGLOG(LogExport, Error, "Failed to save PNG: " + filename);
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
		NGLOG(LogExport, Info, "Saved TGA: " + filename);
		return true;
	}
	else
	{
		NGLOG(LogExport, Error, "Failed to save TGA: " + filename);
		return false;
	}
}

bool ImageExporter::SaveBMP(const std::string& filename, unsigned int textureId, int width, int height)
{
	std::vector<unsigned char> pixels(width * height);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

	std::vector<unsigned char> rgb(width * height * 3);
	for(int i = 0; i < width * height; ++i) {
		rgb[i * 3 + 0] = pixels[i];
		rgb[i * 3 + 1] = pixels[i];
		rgb[i * 3 + 2] = pixels[i];
	}

	if(stbi_write_bmp(filename.c_str(), width, height, 3, rgb.data()))
	{
		NGLOG(LogExport, Info, "Saved BMP: " + filename);
		return true;
	}
	else
	{
		NGLOG(LogExport, Error, "Failed to save BMP: " + filename);
		return false;
	}
}

bool ImageExporter::SaveJPG(const std::string& filename, unsigned int textureId, int width, int height, int quality)
{
	std::vector<unsigned char> pixels(width * height);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

	std::vector<unsigned char> rgb(width * height * 3);
	for(int i = 0; i < width * height; ++i) {
		rgb[i * 3 + 0] = pixels[i];
		rgb[i * 3 + 1] = pixels[i];
		rgb[i * 3 + 2] = pixels[i];
	}

	if(stbi_write_jpg(filename.c_str(), width, height, 3, rgb.data(), quality))
	{
		NGLOG(LogExport, Info, "Saved JPG: " + filename);
		return true;
	}
	else
	{
		NGLOG(LogExport, Error, "Failed to save JPG: " + filename);
		return false;
	}
}