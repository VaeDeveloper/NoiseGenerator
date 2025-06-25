#pragma once 

#include "NoisePanelModel.h"
#include "IController.h"
#include "BoilerplateMacro.h"
#include <memory>
#include <functional>


/**
 * NoisePanelController
 *
 * Acts as the Controller in MVC architecture.
 * Manages the NoisePanelModel instance and provides a high-level interface
 * for uploading noise data and accessing the model.
 */
class NoisePanelController : public IController, public IControllerTyped<NoisePanelModel>
{
public:
	IMPL_TYPED_MODEL(NoisePanelModel, model);

	NoisePanelController();

	virtual void Initialize() override;

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
	std::shared_ptr<NoisePanelModel> model;
};