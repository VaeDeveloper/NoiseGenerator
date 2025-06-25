#include "NoisePanelController.h"
#include "Logger/LoggerMacro.h"

DEFINE_LOG_CATEGORY(NPControllerLog);

NoisePanelController::NoisePanelController()
{
	model = std::make_unique<NoisePanelModel>();
	if(model)
	{
		model->Initialize();
	}
	NGLOG(NPControllerLog, Info, "NoisePanelController Initialize");
}

void NoisePanelController::Initialize()
{
}

void NoisePanelController::UploadNoise(const float* noiseData, int w, int h) const
{
	if(!model || !model->IsInitialized())
	{
		NGLOG(NPControllerLog, Warning, "Model is not initialized. Upload aborted.");
		return;
	}

	//if(!noiseData || w <= 0 || h <= 0)
	//{
	//	NGLOG(NPControllerLog, Error, "Invalid noise data or dimensions passed to UploadNoise (w = "
	//		+ std::to_string(w) + ", h = " + std::to_string(h) + ")");
	//	return;
	//}

	model->UploadData(noiseData, w, h);
}

