#include "NoisePropertyController.h"
#include "MVC/Model/NoisePropertyModel.h"
#include "Noise/NoiseGenerator.h"

NoisePropertyController::NoisePropertyController()
{
}

void NoisePropertyController::Randomize()
{
	model.Randomize();
}

void NoisePropertyController::Mutate(int style)
{
	model.Mutate(style);
}

void NoisePropertyController::Reset()
{
	model.Reset();
}

void NoisePropertyController::SetLockAll(bool value)
{
	model.SetLockAll(value);
}

bool NoisePropertyController::IsAllLocked() const
{
	return model.IsAllLocked();
}

void NoisePropertyController::StartGeneration()
{
	if(isGenerating) return;
		

	const auto& props = model.Get();
	int res = 8 << model.GetResolutionIndex();

	NoiseProperties propsCopy = props;
	isGenerating = true;
	generationProgress = 0.0f;
	cancelRequested = false;

	generationThread = std::thread([this, res, propsCopy] ()
		{
			float* noise = NG::FBMNoise2D(res, &propsCopy, [this] (float progress) 
				{
					this->generationProgress = progress;

					// Уведомляем UI
					if(OnProgressUpdated.IsBound())
					{
						OnProgressUpdated.Execute(progress);
					}

					return !this->cancelRequested;
				});

			if(noise)
			{
				if(OnNoiseGenerated.IsBound())
				{
					OnNoiseGenerated.Execute(noise); // UI должен освободить
				}

				if(OnGenerationCompleted.IsBound())
				{
					OnGenerationCompleted.Execute();
				}
			}
			else
			{
				if(OnGenerationFailed.IsBound())
				{
					OnGenerationFailed.Execute();
				}
			}

			this->generationProgress = -1.0f;
			this->isGenerating = false;
		});

	generationThread.detach();
}

void NoisePropertyController::CancelGeneration()
{
	cancelRequested = true;
}

bool NoisePropertyController::IsGenerating() const
{
	return isGenerating;
}

float NoisePropertyController::GetProgress() const
{
	return generationProgress;
}

NoiseProperties& NoisePropertyController::GetProperties()
{
	return model.Access();
}

LockFlags& NoisePropertyController::GetLockFlags()
{
	return model.locks;
}



void NoisePropertyController::SetUITaskCallback(std::function<void(std::function<void()>)> cb)
{
	StartGeneration();
}


void NoisePropertyController::GenerateNoiseAsync()
{
}
