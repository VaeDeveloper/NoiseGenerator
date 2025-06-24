#pragma once 
#include "Noise/NoiseTypes.h"
#include "../IModel.h"

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
	const NoiseProperties& Get() const;

	NoiseProperties& Access();

#pragma region NoisePropertyModel
	virtual std::string GetId() const override
	{
		return "noise_property_model";
	}


	virtual void Reset() override;

	// Serialization from JSON 
	virtual std::string SerializeToJson() const { return "{}"; }
	virtual void LoadFromJson(const std::string&) {}
#pragma endregion


	void Randomize(bool respectLocks = true);
	void Mutate(int style);


	LockFlags& GetLockFlags()
	{
		return locks;
	}

	void SetLockAll(bool lock = true) 
	{
		bLocked = !bLocked;
		locks.SetAll(bLocked); 
	}

	bool IsAllLocked() const { return locks.AreAllLocked(); }

	int GetRandomStyle() const;

	static constexpr const char* resolutions_[] = 
	{
		"8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
	};

	int GetResolutionIndex() const 
	{
		return resolutionIndex_;
	}

	void SetResolutionIndex(int index) {
		if(index >= 0 && index < GetResolutionCount()) {
			resolutionIndex_ = index;
		}
	}

	int GetResolutionValue() const {
		return 8 << resolutionIndex_;
	}

	static constexpr int GetResolutionCount() {
		return sizeof(resolutions_) / sizeof(resolutions_[0]);
	}

	static constexpr const char* const* GetResolutions() {
		return resolutions_;
	}



private:
	float RandF(float min, float max) const;

	bool bLocked = false;

	int resolutionIndex_ = 3;


	//Random properties
	int randomStyle = 0;

	static constexpr char* randomStyles[] = 
	{
		"Full Random",
		"Controlled Chaos",
		"Organic",
		"Extreme",
		"Minimal",
	};

	LockFlags locks;
	NoiseProperties props;

};