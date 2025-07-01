#pragma once 

#include "NoisePropertyModel.h"
#include "BoilerplateMacro.h"
#include "RandomGenerator.h"
#include "IController.h"
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include <unordered_map>
#include "Delegates.h"

DECLARE_DELEGATE_ThreeParams(FOnNoiseGenerated, float*, int, int);

class NoisePropertyController : public IController, IControllerTyped<NoisePropertyModel>
{
	IMPL_TYPED_MODEL(NoisePropertyModel, model);

public:
	NoisePropertyController();
	~NoisePropertyController();
	virtual void Initialize() override;

	void Randomize();
	void Mutate(int style);
	void Reset();
	void SetLockAll();
	bool IsAllLocked() const;

	void StartGeneration();
	void CancelGeneration();
	void Clear();
	bool IsGenerating() const;
	float GetProgress() const;

	void ProcessUITasks();

	NoiseProperties& GetProperties();
	LockFlags& GetLockFlags();

	FOnNoiseGenerated OnNoiseReadyForUI;

	
	void SaveCurrentPreset(const std::string& name);
	void LoadPreset(const std::string& name);

	std::vector<std::string> NoisePropertyController::GetPresetNames() const;

	static void GenerationThreadEntry(NoisePropertyController* instance, NoiseProperties propsCopy);

	std::queue<std::function<void()>> uiTasks;
	std::mutex uiMutex;
private:
	void QueueUITask(std::function<void()> task);
	void GenerateAsync(NoiseProperties propsCopy);

	std::unordered_map<std::string, NoisePreset> savedPresets;

	std::atomic<bool> isGenerating = false;
	std::atomic<float> generationProgress = 0.0f;
	std::atomic<bool> cancelRequested = false;

	std::thread generationThread;
	
	std::shared_ptr<NoisePropertyModel> model;
	RandomGenerator generator;
};