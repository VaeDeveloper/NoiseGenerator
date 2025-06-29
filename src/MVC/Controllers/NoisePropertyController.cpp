#include "NoisePropertyController.h"
#include "NoisePropertyModel.h"
#include "NoiseGenerator.h"
#include <random>
#include "NoiseTypes.h"
#include "FileUtils.h"
#include "iostream"
#include <fstream>
#include <filesystem>
#include "LoggerMacro.h"
#include "Serialization/NoisePresetSerialization.h"


namespace fs = std::filesystem;

DEFINE_LOG_CATEGORY(NGController);

NoisePropertyController::NoisePropertyController()
{
	model = std::make_shared<NoisePropertyModel>();
}

NoisePropertyController::~NoisePropertyController()
{
	cancelRequested = true;

	if(generationThread.joinable())
	{
		generationThread.join();
	}
}

void NoisePropertyController::Initialize()
{

}

void NoisePropertyController::Randomize()
{
	model->Randomize();
}

void NoisePropertyController::Mutate(int style)
{
	model->Mutate(style);
}

void NoisePropertyController::Reset()
{
	model->Reset();
}

void NoisePropertyController::SetLockAll()
{
	model->SetLockAll();
}

bool NoisePropertyController::IsAllLocked() const
{
	return model->IsAllLocked();
}

void NoisePropertyController::StartGeneration()
{
	if(isGenerating)
	{
		return;
	}

	NoiseProperties propsCopy = model->Get();
	propsCopy.res = model->GetResolutionValue();
	propsCopy.seed = generator.NextInt(0, std::numeric_limits<int>::max());
	isGenerating = true;
	generationProgress = 0.0f;
	cancelRequested = false;
	generationThread = std::thread(&NoisePropertyController::GenerationThreadEntry, this, propsCopy);
	generationThread.detach();
}

void NoisePropertyController::QueueUITask(std::function<void()> task)
{
	std::lock_guard<std::mutex> lock(uiMutex);
	uiTasks.push(std::move(task));
}

void NoisePropertyController::GenerateAsync(NoiseProperties propsCopy)
{
	auto noiseType = model->GetType();
	float* noise = nullptr;

	auto progressCallback = [this] (float progress)
		{
			this->generationProgress = progress;
			return !this->cancelRequested;
		};

	switch(noiseType)
	{
	case NoiseType::Value:        noise = NG::ValueNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Perlin:       noise = NG::PerlinNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Simplex:      noise = NG::SimplexFBMNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::FBM:          noise = NG::FBMNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Cellular:     noise = NG::CellularNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Voronoi:      noise = NG::VoronoiNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Ridged:       noise = NG::RidgedNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Worley:       noise = NG::WorleyNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::DomainWarp:   noise = NG::DomainWarpNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Billow:       noise = NG::BillowNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::OpenSimplex:  noise = NG::OpenSimplexNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::White:        noise = NG::WhiteNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	case NoiseType::Gabor:        noise = NG::GaborNoise2D(propsCopy.res, &propsCopy, progressCallback); break;
	default:
		NGLOG(NGController, Error, "Unknown noise type");
		break;
	}

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

void NoisePropertyController::CancelGeneration()
{
	cancelRequested = true;
}

void NoisePropertyController::Clear()
{
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
	return model->Access();
}

LockFlags& NoisePropertyController::GetLockFlags()
{
	return model->GetLockFlags();
}

void NoisePropertyController::SaveCurrentPreset(const std::string& name)
{
	std::string basePath = NG::FileSystem::FindPath("presets");
	if(basePath.empty()) {
		fs::create_directories("presets");
		basePath = "presets";
	}

	std::string path = basePath + "/" + name + ".json";

	std::ofstream out(path);
	if(!out.is_open())
	{
		NGLOG(NGController, Error, "Failed to open file for saving preset: " + path);
		return;
	}

	NoiseProperties props = model->Access();
	NoisePreset preset{ name, props, model->GetType(), model->GetResolutionIndex() };

	nlohmann::json j = preset;
	out << j.dump(4);

	NGLOG(NGController, Info, "Preset saved to: " + path);
}

void NoisePropertyController::LoadPreset(const std::string& name)
{
	std::string basePath = NG::FileSystem::FindPath("presets");
	if(basePath.empty())
	{
		NGLOG(NGController, Error, "Presets directory not found");
		return;
	}


	std::string path = basePath + "/" + name + ".json";
	nlohmann::json j;
	std::ifstream in(path);
	if(!in.is_open())
	{
		NGLOG(NGController, Error, "Failed to open preset file: " + path);
		return;
	}
	try 
	{
		in >> j;
		NoisePreset preset = j.get<NoisePreset>();

		GetModel()->SetType(preset.type);
		GetModel()->Access() = preset.properties;
		GetModel()->SetResolutionIndex(preset.resolutionIndex);

		NGLOG(NGController, Info, "Preset loaded: " + name);
	}
	catch(const std::exception& ex) 
	{
		std::cerr << "Exception: " << ex.what() << "\n";
		std::cerr << "JSON content:\n" << j.dump(4) << "\n";
	}
}

std::vector<std::string> NoisePropertyController::GetPresetNames() const
{
	std::vector<std::string> names;

	std::filesystem::path dir = NG::FileSystem::FindPath("presets");
	if(dir.empty())
		return names;

	for(const auto& entry : std::filesystem::directory_iterator(dir))
	{
		if(entry.path().extension() == ".json")
			names.push_back(entry.path().stem().string());
	}

	return names;
}

void NoisePropertyController::GenerationThreadEntry(NoisePropertyController* instance, NoiseProperties propsCopy)
{
	instance->GenerateAsync(propsCopy);
}
