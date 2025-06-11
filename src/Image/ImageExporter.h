
#pragma once

#include <string>

class ImageExporter 
{
public:
	static bool SavePNG(const std::string& filename, unsigned int textureId, int width, int height);
	static bool SaveTGA(const std::string& filename, unsigned int textureId, int width, int height);
};