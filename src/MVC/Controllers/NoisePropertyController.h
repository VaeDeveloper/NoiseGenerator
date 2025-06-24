#pragma once 

#include "MVC/Models/NoisePropertyModel.h"
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include "Utils/Delegates.h"

DECLARE_DELEGATE_ThreeParams(FOnNoiseGenerated, float*, int, int);


class NoisePropertyController
{
public:
	NoisePropertyController();

	void Randomize();
	void Mutate(int style);
	void Reset();
	void SetLockAll();
	bool IsAllLocked() const;

	void StartGeneration();
	void CancelGeneration();
	bool IsGenerating() const;
	float GetProgress() const;

	void ProcessUITasks();

	NoiseProperties& GetProperties();
	LockFlags& GetLockFlags();

	FOnNoiseGenerated OnNoiseReadyForUI;

	NoisePropertyModel& GetModel() {
		return model;
	}

	const NoisePropertyModel& GetModel() const {
		return model;
	}
	
	std::queue<std::function<void()>> uiTasks;
	std::mutex uiMutex;
private:
	void QueueUITask(std::function<void()> task);

	
	NoisePropertyModel model;

	std::atomic<bool> isGenerating = false;
	std::atomic<float> generationProgress = 0.0f;
	std::atomic<bool> cancelRequested = false;

	std::thread generationThread;

	
	
	int randomStyle = 0;
	int resolutionIndex = 2;




};