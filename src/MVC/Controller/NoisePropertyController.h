#pragma once 

#include "MVC/Model/NoisePropertyModel.h"
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include "Utils/Delegates.h"

DECLARE_DELEGATE_OneParam(FOnNoiseGenerated, float*); // pointer to noise
DECLARE_DELEGATE(FOnGenerationCompleted);
DECLARE_DELEGATE(FOnGenerationFailed);
DECLARE_DELEGATE_OneParam(FOnProgressUpdated, float);

class NoisePropertyController
{
public:
	NoisePropertyController();

	void Randomize();
	void Mutate(int style);
	void Reset();
	void SetLockAll(bool value);
	bool IsAllLocked() const;

	void StartGeneration();
	void CancelGeneration();
	bool IsGenerating() const;
	float GetProgress() const;

	NoiseProperties& GetProperties();
	LockFlags& GetLockFlags();

	void SetUITaskCallback(std::function<void(std::function<void()>)> cb);


	FOnNoiseGenerated OnNoiseGenerated;
	FOnGenerationCompleted OnGenerationCompleted;
	FOnGenerationFailed OnGenerationFailed;
	FOnProgressUpdated OnProgressUpdated;

private:

	void GenerateNoiseAsync();

	NoisePropertyModel model;

	std::atomic<bool> isGenerating = false;
	std::atomic<float> generationProgress = 0.0f;
	std::atomic<bool> cancelRequested = false;
	std::thread generationThread;

	int randomStyle = 0;
	int resolutionIndex = 3;




};