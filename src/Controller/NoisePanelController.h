#pragma once 

#include "Model/NoisePanelModel.h"
#include <memory>


/**
 * NoisePanelController
 *
 * Acts as the Controller in MVC architecture.
 * Manages the NoisePanelModel instance and provides a high-level interface
 * for uploading noise data and accessing the model.
 */
class NoisePanelController
{
public:
	NoisePanelController();

	/** Returns a pointer to the underlying model */
	NoisePanelModel* GetModel();

	/**
	 * Uploads noise data to the model, if it has been initialized.
	 *
	 * @param noiseData		Pointer to grayscale noise values
	 * @param w			Texture width
	 * @param h			Texture height
	 */
	void UploadNoise(const float* noiseData, int w, int h) const;

private:
	/** Unique instance of the model owned by this controller */
	std::unique_ptr<NoisePanelModel> model;
};