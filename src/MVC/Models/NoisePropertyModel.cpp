#include "NoisePropertyModel.h"
#include "LoggerMacro.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <random>

DEFINE_LOG_CATEGORY(NoisePropertyLog);

const NoiseProperties& NoisePropertyModel::Get() const
{
	return props;
}

NoiseProperties& NoisePropertyModel::Access() 
{
	return props;
}

std::string NoisePropertyModel::GetId() const
{
	return "noise_property_model";
}

void NoisePropertyModel::Reset()
{
	props = {};
	resolutionIndex = 3;
	SetLockAll();

	NGLOG(NoisePropertyLog, Info, "NoisePropertyModel reset to default");
}

void NoisePropertyModel::Randomize(bool respectLocks)
{
	int res = 8 << resolutionIndex;
	if(!locks.bSeed) props.seed = rand();
	if(!locks.bRoughness) props.roughness = ImLerp(0.01f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bMarbling) props.marbling = ImLerp(0.0f, 10.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bLowFreq) props.low_freq_skip = rand() % 5;
	if(!locks.bHighFreq) props.high_freq_skip = rand() % 5;
	if(!locks.bTurbulence) props.turbulence = ImLerp(0.0f, 64.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bTurbRes) props.turbulence_res = rand() % IM_ARRAYSIZE(resolutions);
	if(!locks.bTurbRoughness) props.turbulence_roughness = ImLerp(0.01f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bTurbLow) props.turbulence_low_freq_skip = rand() % 5;
	if(!locks.bTurbHigh) props.turbulence_high_freq_skip = rand() % 5;
	if(!locks.bTurbMarbling) props.turbulence_marbling = ImLerp(0.0f, 10.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bExpShift) props.turbulence_expshift = ImLerp(-4.0f, 4.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bOffsetX) props.turbulence_offset_x = ImLerp(-1.0f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.bOffsetY) props.turbulence_offset_y = ImLerp(-1.0f, 1.0f, static_cast<float>(rand()) / RAND_MAX);

	NGLOG(NoisePropertyLog, Info, "Randomized noise settings");
}

void NoisePropertyModel::Mutate(int style)
{
	switch(style)
	{
	case 0: // Full Random
		if(!locks.bSeed) props.seed = rand();
		if(!locks.bRoughness) props.roughness = RandF(0.01f, 1.0f);
		if(!locks.bMarbling) props.marbling = RandF(0.0f, 10.0f);
		if(!locks.bLowFreq) props.low_freq_skip = rand() % 5;
		if(!locks.bHighFreq) props.high_freq_skip = rand() % 5;
		if(!locks.bTurbulence) props.turbulence = RandF(0.0f, 64.0f);
		if(!locks.bTurbRes) props.turbulence_res = rand() % IM_ARRAYSIZE(resolutions);
		if(!locks.bTurbRoughness) props.turbulence_roughness = RandF(0.01f, 1.0f);
		if(!locks.bTurbLow) props.turbulence_low_freq_skip = rand() % 5;
		if(!locks.bTurbHigh) props.turbulence_high_freq_skip = rand() % 5;
		if(!locks.bTurbMarbling) props.turbulence_marbling = RandF(0.0f, 10.0f);
		if(!locks.bExpShift) props.turbulence_expshift = RandF(-4.0f, 4.0f);
		if(!locks.bOffsetX) props.turbulence_offset_x = RandF(-1.0f, 1.0f);
		if(!locks.bOffsetY) props.turbulence_offset_y = RandF(-1.0f, 1.0f);
		break;

	case 1: // Controlled Chaos 
		if(!locks.bRoughness) props.roughness *= RandF(0.9f, 1.1f);
		if(!locks.bMarbling) props.marbling += RandF(-0.5f, 0.5f);
		if(!locks.bLowFreq) props.low_freq_skip = (props.low_freq_skip + rand() % 3) % 5;
		if(!locks.bHighFreq) props.high_freq_skip = (props.high_freq_skip + rand() % 3) % 5;
		if(!locks.bTurbulence) props.turbulence *= RandF(0.8f, 1.2f);
		if(!locks.bTurbRoughness) props.turbulence_roughness *= RandF(0.8f, 1.2f);
		if(!locks.bTurbMarbling) props.turbulence_marbling += RandF(-0.5f, 0.5f);
		break;

	case 2: // Organic 
		if(!locks.bRoughness) props.roughness = RandF(0.3f, 0.6f);
		if(!locks.bMarbling) props.marbling = RandF(0.1f, 2.0f);
		if(!locks.bTurbulence) props.turbulence = RandF(0.0f, 4.0f);
		if(!locks.bTurbMarbling) props.turbulence_marbling = RandF(0.0f, 2.0f);
		break;

	case 3: // Extreme 
		if(!locks.bRoughness) props.roughness = RandF(0.01f, 1.0f);
		if(!locks.bMarbling) props.marbling = RandF(5.0f, 10.0f);
		if(!locks.bTurbulence) props.turbulence = RandF(32.0f, 64.0f);
		if(!locks.bTurbMarbling) props.turbulence_marbling = RandF(5.0f, 10.0f);
		if(!locks.bExpShift) props.turbulence_expshift = RandF(-4.0f, 4.0f);
		break;

	case 4: // Minimal 
		if(!locks.bRoughness) props.roughness = RandF(0.01f, 0.05f);
		if(!locks.bMarbling) props.marbling = RandF(0.0f, 1.0f);
		if(!locks.bTurbulence) props.turbulence = 0.0f;
		if(!locks.bTurbMarbling) props.turbulence_marbling = 0.0f;
		if(!locks.bTurbRoughness) props.turbulence_roughness = 0.01f;
		break;
	}
}

LockFlags& NoisePropertyModel::GetLockFlags()
{
	return locks;
}

void NoisePropertyModel::SetLockAll(bool lock)
{
	bLocked = !bLocked;
	locks.SetAll(bLocked);
}

int NoisePropertyModel::GetRandomStyle() const
{
	return randomStyle;
}

int NoisePropertyModel::GetResolutionIndex() const
{
	return resolutionIndex;
}

void NoisePropertyModel::SetResolutionIndex(int index)
{
	if(index >= 0 && index < GetResolutionCount())
	{
		resolutionIndex = index;
	}
	else
	{
		NGLOG(NoisePropertyLog, Warning, "Attempted to set invalid resolution index: %d (allowed range: 0-%d)",
			index, GetResolutionCount() - 1);
	}
}

int NoisePropertyModel::GetResolutionValue() const
{
	return 8 << resolutionIndex;
}



NoiseType NoisePropertyModel::GetType() const
{
	return type;
}

void NoisePropertyModel::SetType(NoiseType type)
{
	this->type = type;
}

float NoisePropertyModel::RandF(float min, float max) const
{
	return ImLerp(min, max, static_cast<float>(rand()) / RAND_MAX);
}
