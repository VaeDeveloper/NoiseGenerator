#include "ImageExporter.h"

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
	std::vector<unsigned char> rgb;
	if(!ReadTextureAsRGB(textureId, width, height, rgb))
	{
		NGLOG(LogExport, Warning, "Failed to read texture (ID = " 
			+ std::to_string(textureId) + ") as RGB for export.");
		return false;
	}

	if(stbi_write_png(filename.c_str(), width, height, 3, rgb.data(), width * 3))
	{
		NGLOG(LogExport, Info, "Saved PNG: " + filename);
		return true;
	}
	NGLOG(LogExport, Error, "Failed to save PNG: " + filename);
	return false;
}

bool ImageExporter::SaveTGA(const std::string& filename, unsigned int textureId, int width, int height) 
{
	std::vector<unsigned char> rgb;
	if(!ReadTextureAsRGB(textureId, width, height, rgb))
	{
		NGLOG(LogExport, Warning, "Failed to read texture (ID = "
			+ std::to_string(textureId) + ") as TGA for export.");
		return false;
	}

	if(stbi_write_tga(filename.c_str(), width, height, 3, rgb.data()))
	{
		NGLOG(LogExport, Info, "Saved TGA: " + filename);
		return true;
	}
	NGLOG(LogExport, Error, "Failed to save TGA: " + filename);
	return false;
}

bool ImageExporter::SaveBMP(const std::string& filename, unsigned int textureId, int width, int height)
{
	std::vector<unsigned char> rgb;
	if(!ReadTextureAsRGB(textureId, width, height, rgb))
	{
		NGLOG(LogExport, Warning, "Failed to read texture (ID = "
			+ std::to_string(textureId) + ") as BMP for export.");
		return false;
	}

	if(stbi_write_bmp(filename.c_str(), width, height, 3, rgb.data()))
	{
		NGLOG(LogExport, Info, "Saved BMP: " + filename);
		return true;
	}
	NGLOG(LogExport, Error, "Failed to save BMP: " + filename);
	return false;
}

bool ImageExporter::SaveJPG(const std::string& filename, unsigned int textureId, int width, int height, int quality)
{
	std::vector<unsigned char> rgb;
	if(!ReadTextureAsRGB(textureId, width, height, rgb))
	{
		NGLOG(LogExport, Warning, "Failed to read texture (ID = "
			+ std::to_string(textureId) + ") as BMP for export.");
		return false;
	}

	if(stbi_write_jpg(filename.c_str(), width, height, 3, rgb.data(), quality))
	{
		NGLOG(LogExport, Info, "Saved BMP: " + filename);
		return true;
	}
	NGLOG(LogExport, Error, "Failed to save BMP: " + filename);
	return false;
}

bool ImageExporter::ReadTextureAsRGB(unsigned int textureId, int width, int height, std::vector<unsigned char>& outRGB)
{
	if(textureId == 0 || width <= 0 || height <= 0)
	{
		NGLOG(LogExport, Error, "Invalid texture parameters for export.");
		return false;
	}

	std::vector<unsigned char> pixels(width * height);
	outRGB.resize(width * height * 3);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

	for(int i = 0; i < width * height; ++i)
	{
		outRGB[i * 3 + 0] = pixels[i];
		outRGB[i * 3 + 1] = pixels[i];
		outRGB[i * 3 + 2] = pixels[i];
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}
