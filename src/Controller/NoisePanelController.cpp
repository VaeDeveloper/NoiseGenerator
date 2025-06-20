#include "NoisePanelController.h"
#include "Logger/LoggerMacro.h"

DEFINE_LOG_CATEGORY(NoisePanelControllerLog);

NoisePanelController::NoisePanelController()
{
	model = std::make_unique<NoisePanelModel>();
	if(model)
	{
		model->Initialize();
	}
}

NoisePanelModel* NoisePanelController::GetModel()
{
	if(!model)
	{
		NGLOG(NoisePanelControllerLog, Error, "Attempted to access null model.");
		return nullptr;
	}

	return model.get();
}


void NoisePanelController::UploadNoise(const float* noiseData, int w, int h) const
{
	if(!model || !model->IsInitialized())
	{
		NGLOG(NoisePanelControllerLog, Warning, "Model is not initialized. Upload aborted.");
		return;
	}

	if(!noiseData || w <= 0 || h <= 0)
	{
		NGLOG(NoisePanelControllerLog, Error, "Invalid noise data or dimensions passed to UploadNoise (w = "
			+ std::to_string(w) + ", h = " + std::to_string(h) + ")");
		return;
	}

	model->UploadData(noiseData, w, h);
}
