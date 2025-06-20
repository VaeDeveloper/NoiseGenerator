#pragma once 

#include <glad/glad.h>
#include <vector>


/**
 * NoisePanelModel
 *
 * Responsible for managing the OpenGL texture used to display noise preview data.
 * Handles allocation, upload, and state tracking of the preview texture.
 */
class NoisePanelModel
{
public:
	NoisePanelModel(){}
	~NoisePanelModel();

	/** Initializes the texture if not already created */
	void Initialize();

	/**
	 * Uploads raw grayscale float data as RGB texture to GPU.
	 * Expands grayscale input into RGB channels.
	 *
	 * @param data   Pointer to grayscale float data (size: width * height)
	 * @param width  Texture width
	 * @param height Texture height
	 */
	void UploadData(const float* data, int width, int height);

	/** Returns the OpenGL texture ID */
	GLuint GetTextureId() const;

	/** Returns true if the texture has been initialized texture*/
	bool IsInitialized() const;

	/** Returns the current texture width */
	int GetWidth() const;

	/** Returns the current texture height */
	int GetHeight() const;


private:
	/** OpenGL texture ID */
	GLuint gTextureId = 0;

	/** Whether the texture has been initialized */
	bool bTextureInitialized = false;

	/** Texture width in pixels */
	int width = 0;

	/** Texture height in pixels */
	int height = 0;
};
