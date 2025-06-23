#pragma once 
#include "Noise/NoiseTypes.h"


struct LockFlags
{
	bool seed = false;
	bool roughness = false;
	bool marbling = false;
	bool lowFreq = false;
	bool highFreq = false;
	bool turbulence = false;
	bool turbRes = false;
	bool turbRoughness = false;
	bool turbLow = false;
	bool turbHigh = false;
	bool turbMarbling = false;
	bool expShift = false;
	bool offsetX = false;
	bool offsetY = false;

	void SetAll(bool value)
	{
		seed = roughness = marbling = lowFreq = highFreq =
			turbulence = turbRes = turbRoughness = turbLow = turbHigh =
			turbMarbling = expShift = offsetX = offsetY = value;
	}

	bool AreAllLocked() const
	{
		return seed && roughness && marbling && lowFreq && highFreq &&
			turbulence && turbRes && turbRoughness && turbLow && turbHigh &&
			turbMarbling && expShift && offsetX && offsetY;
	}


};

class NoisePropertyModel
{
public:
	const NoiseProperties& Get() const;
	NoiseProperties& Access();

	void Reset();
	void Randomize(bool respectLocks = true);
	void Mutate(int style);

	NoiseProperties props;
	LockFlags locks;

	void SetLockAll(bool value) { locks.SetAll(value); }
	bool IsAllLocked() const { return locks.AreAllLocked(); }
	int GetRandomStyle() const;
	int GetResolutionIndex() const;


private:
	float RandF(float min, float max) const;


	int resolutionIndex = 3;
	static constexpr char* resolutions[] =
	{
		"8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
	};

	//Random properties
	int randomStyle = 0;

	static constexpr char* randomStyles[] = {
	"Full Random",
	"Controlled Chaos",
	"Organic",
	"Extreme",
	"Minimal",
	};
};