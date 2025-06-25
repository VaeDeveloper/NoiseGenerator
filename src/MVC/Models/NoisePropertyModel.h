#pragma once 
#include "NoiseTypes.h"
#include "IModel.h"

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

class NoisePropertyModel : public IModel
{
public:
	virtual std::string GetId() const override;
	virtual void Reset() override;
	virtual std::string SerializeToJson() const { return "{}"; }
	virtual void LoadFromJson(const std::string&) {}

	const NoiseProperties& Get() const;

	NoiseProperties& Access();

	void Randomize(bool respectLocks = true);
	void Mutate(int style);

	LockFlags& GetLockFlags();
	void SetLockAll(bool lock = true);
	bool IsAllLocked() const { return locks.AreAllLocked(); }
	int GetRandomStyle() const;
	int GetResolutionIndex() const;
	void SetResolutionIndex(int index);
	int GetResolutionValue() const;
	static constexpr int GetResolutionCount();
	static constexpr const char* const* GetResolutions()
	{
		return resolutions;
	}

	

	int turbulenceResIndex = 2;
private:
	float RandF(float min, float max) const;

	bool bLocked = false;
	int resolutionIndex = 3;
	int randomStyle = 0;

	LockFlags locks;
	NoiseProperties props;
	static constexpr char* randomStyles[] = 
	{
		"Full Random",
		"Controlled Chaos",
		"Organic",
		"Extreme",
		"Minimal",
	};

	static constexpr const char* resolutions[] =
	{
		"8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
	};
};