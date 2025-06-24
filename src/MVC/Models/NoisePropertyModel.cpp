#include "NoisePropertyModel.h"
#include "Logger/LoggerMacro.h"
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

void NoisePropertyModel::Reset()
{
	props = {};
	resolutionIndex_ = 3;
	SetLockAll();

	NGLOG(NoisePropertyLog, Info, "NoisePropertyModel reset to default");
}

void NoisePropertyModel::Randomize(bool respectLocks)
{
	int res = 8 << resolutionIndex_;
	if(!locks.seed) props.seed = rand();
	if(!locks.roughness) props.roughness = ImLerp(0.01f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.marbling) props.marbling = ImLerp(0.0f, 10.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.lowFreq) props.low_freq_skip = rand() % 5;
	if(!locks.highFreq) props.high_freq_skip = rand() % 5;
	if(!locks.turbulence) props.turbulence = ImLerp(0.0f, 64.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.turbRes) props.turbulence_res = rand() % IM_ARRAYSIZE(GetResolutions());
	if(!locks.turbRoughness) props.turbulence_roughness = ImLerp(0.01f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.turbLow) props.turbulence_low_freq_skip = rand() % 5;
	if(!locks.turbHigh) props.turbulence_high_freq_skip = rand() % 5;
	if(!locks.turbMarbling) props.turbulence_marbling = ImLerp(0.0f, 10.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.expShift) props.turbulence_expshift = ImLerp(-4.0f, 4.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.offsetX) props.turbulence_offset_x = ImLerp(-1.0f, 1.0f, static_cast<float>(rand()) / RAND_MAX);
	if(!locks.offsetY) props.turbulence_offset_y = ImLerp(-1.0f, 1.0f, static_cast<float>(rand()) / RAND_MAX);

	NGLOG(NoisePropertyLog, Info, "Randomized noise settings");
}

void NoisePropertyModel::Mutate(int style)
{
	switch(style)
	{
	case 0: // Full Random
		if(!locks.seed) props.seed = rand();
		if(!locks.roughness) props.roughness = RandF(0.01f, 1.0f);
		if(!locks.marbling) props.marbling = RandF(0.0f, 10.0f);
		if(!locks.lowFreq) props.low_freq_skip = rand() % 5;
		if(!locks.highFreq) props.high_freq_skip = rand() % 5;
		if(!locks.turbulence) props.turbulence = RandF(0.0f, 64.0f);
		if(!locks.turbRes) props.turbulence_res = rand() % IM_ARRAYSIZE(GetResolutions());
		if(!locks.turbRoughness) props.turbulence_roughness = RandF(0.01f, 1.0f);
		if(!locks.turbLow) props.turbulence_low_freq_skip = rand() % 5;
		if(!locks.turbHigh) props.turbulence_high_freq_skip = rand() % 5;
		if(!locks.turbMarbling) props.turbulence_marbling = RandF(0.0f, 10.0f);
		if(!locks.expShift) props.turbulence_expshift = RandF(-4.0f, 4.0f);
		if(!locks.offsetX) props.turbulence_offset_x = RandF(-1.0f, 1.0f);
		if(!locks.offsetY) props.turbulence_offset_y = RandF(-1.0f, 1.0f);
		break;

	case 1: // Controlled Chaos 
		if(!locks.roughness) props.roughness *= RandF(0.9f, 1.1f);
		if(!locks.marbling) props.marbling += RandF(-0.5f, 0.5f);
		if(!locks.lowFreq) props.low_freq_skip = (props.low_freq_skip + rand() % 3) % 5;
		if(!locks.highFreq) props.high_freq_skip = (props.high_freq_skip + rand() % 3) % 5;
		if(!locks.turbulence) props.turbulence *= RandF(0.8f, 1.2f);
		if(!locks.turbRoughness) props.turbulence_roughness *= RandF(0.8f, 1.2f);
		if(!locks.turbMarbling) props.turbulence_marbling += RandF(-0.5f, 0.5f);
		break;

	case 2: // Organic 
		if(!locks.roughness) props.roughness = RandF(0.3f, 0.6f);
		if(!locks.marbling) props.marbling = RandF(0.1f, 2.0f);
		if(!locks.turbulence) props.turbulence = RandF(0.0f, 4.0f);
		if(!locks.turbMarbling) props.turbulence_marbling = RandF(0.0f, 2.0f);
		break;

	case 3: // Extreme 
		if(!locks.roughness) props.roughness = RandF(0.01f, 1.0f);
		if(!locks.marbling) props.marbling = RandF(5.0f, 10.0f);
		if(!locks.turbulence) props.turbulence = RandF(32.0f, 64.0f);
		if(!locks.turbMarbling) props.turbulence_marbling = RandF(5.0f, 10.0f);
		if(!locks.expShift) props.turbulence_expshift = RandF(-4.0f, 4.0f);
		break;

	case 4: // Minimal 
		if(!locks.roughness) props.roughness = RandF(0.01f, 0.05f);
		if(!locks.marbling) props.marbling = RandF(0.0f, 1.0f);
		if(!locks.turbulence) props.turbulence = 0.0f;
		if(!locks.turbMarbling) props.turbulence_marbling = 0.0f;
		if(!locks.turbRoughness) props.turbulence_roughness = 0.01f;
		break;
	}
}

int NoisePropertyModel::GetRandomStyle() const
{
	return randomStyle;
}



float NoisePropertyModel::RandF(float min, float max) const
{
	return ImLerp(min, max, static_cast<float>(rand()) / RAND_MAX);
}
