#include "NoisePanelModel.h"
#include "Logger/LoggerMacro.h"

DEFINE_LOG_CATEGORY(NoisePanelLog);

NoisePanelModel::~NoisePanelModel()
{
	if(bTextureInitialized)
	{
		glDeleteTextures(1, &gTextureId);
	}
}

void NoisePanelModel::Initialize()
{
	if(bTextureInitialized)
	{
		NGLOG(NoisePanelLog, Warning, "Texture initialized");
		return;
	}

	glGenTextures(1, &gTextureId);
	bTextureInitialized = true;
}

void NoisePanelModel::UploadData(const float* data, int width, int height)
{
	if(!data || width <= 0 || height <= 0)
	{
		NGLOG(NoisePanelLog, Warning, "Invalid parameters passed to UploadData (width = "
			+ std::to_string(width) + ", height = " + std::to_string(height) + ")");
		return;
	}

	this->width = width;
	this->height = height;
	int tpixels = width * height;
	// Using raw `new` here instead of std::vector for maximum performance,
	// avoiding zero-initialization and extra safety overhead.
	// Manual memory management is safe in this scope due to predictable allocation size.
	float* rgb_data = new float[tpixels * 3];
	if(!rgb_data)
	{
		NGLOG(NoisePanelLog, Error, "Failed to allocate memory for rgbData");
		return;

	}

	for(int i = 0; i < tpixels; ++i)
	{
		rgb_data[i * 3 + 0] = data[i]; // R
		rgb_data[i * 3 + 1] = data[i]; // G
		rgb_data[i * 3 + 2] = data[i]; // B
	}

	glBindTexture(GL_TEXTURE_2D, gTextureId);
	//glTexSubImage2D( 
	//	GL_TEXTURE_2D,
	//	0, // mip level
	//	0, 0, width, height,
	//	GL_RGB,
	//	GL_FLOAT,
	//	rgb_data
	//);	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, rgb_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Clean up allocated RGB buffer
	delete[] rgb_data;
}

GLuint NoisePanelModel::GetTextureId() const
{
	return gTextureId;
}

bool NoisePanelModel::IsInitialized() const
{
	return bTextureInitialized;
}

int NoisePanelModel::GetWidth() const
{
	return width;
}

int NoisePanelModel::GetHeight() const
{
	return width;
}
