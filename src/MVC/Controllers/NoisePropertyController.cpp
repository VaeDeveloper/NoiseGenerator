#include "NoisePropertyController.h"
#include "MVC/Models/NoisePropertyModel.h"
#include "Generator/NoiseGenerator.h"
#include <random>

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

void NoisePropertyController::SetLockAll()
{
	model.SetLockAll();
}

bool NoisePropertyController::IsAllLocked() const
{
	return model.IsAllLocked();
}

void NoisePropertyController::StartGeneration()
{
	if(isGenerating) return;
		

	NoiseProperties propsCopy = model.Get();
	propsCopy.res = model.GetResolutionValue();
	propsCopy.seed = std::random_device{}();
	isGenerating = true;
	generationProgress = 0.0f;
	cancelRequested = false;
	generationThread = std::thread([this, propsCopy] ()
		{
			float* noise = NG::FBMNoise2D(propsCopy.res, &propsCopy, [this] (float progress)
				{
					this->generationProgress = progress;
					return !this->cancelRequested;
				});

			if(noise != nullptr)
			{
				this->QueueUITask([this, noise, propsCopy] ()
					{
						if(OnNoiseReadyForUI.IsBound())
						{
							OnNoiseReadyForUI.Execute(noise, propsCopy.res, propsCopy.res);
						}
						this->generationProgress = -1.0f;
						this->isGenerating = false;
					});
			}
			else
			{
				this->QueueUITask([this] ()
					{
						if(OnNoiseReadyForUI.IsBound())
						{
							OnNoiseReadyForUI.Execute(nullptr, 0, 0);
						}
						this->generationProgress = -1.0f;
						this->isGenerating = false;
					});
			}
		});

	generationThread.detach();
}

void NoisePropertyController::QueueUITask(std::function<void()> task)
{
	std::lock_guard<std::mutex> lock(uiMutex);
	uiTasks.push(std::move(task));
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

void NoisePropertyController::ProcessUITasks()
{
	std::lock_guard<std::mutex> lock(uiMutex);
	while(!uiTasks.empty())
	{
		uiTasks.front()();
		uiTasks.pop();
	}
}

NoiseProperties& NoisePropertyController::GetProperties()
{
	return model.Access();
}

LockFlags& NoisePropertyController::GetLockFlags()
{
	return model.GetLockFlags();
}

