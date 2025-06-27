#pragma once 
#include "NoiseTypes.h"
#include "IModel.h"

/**
 * Struct containing flags to lock various procedural noise generation parameters.
 *
 * Used to prevent specific parameters from being modified during operations such as
 * regeneration or parameter randomization.
 */
struct LockFlags
{
	/** Locks the freq value */
	bool bFrequency = false;

	/** Locks the seed value. */
	bool bSeed = false;

	/** Locks the roughness value. */
	bool bRoughness = false;

	/** Locks the marbling effect. */
	bool bMarbling = false;

	/** Locks the low-frequency component. */
	bool bLowFreq = false;

	/** Locks the high-frequency component. */
	bool bHighFreq = false;

	/** Locks turbulence effect. */
	bool bTurbulence = false;

	/** Locks the turbulence resolution. */
	bool bTurbRes = false;

	/** Locks the roughness of turbulence. */
	bool bTurbRoughness = false;

	/** Locks the low-frequency contribution of turbulence. */
	bool bTurbLow = false;

	/** Locks the high-frequency contribution of turbulence. */
	bool bTurbHigh = false;

	/** Locks turbulence marbling. */
	bool bTurbMarbling = false;

	/** Locks exponential shift parameter. */
	bool bExpShift = false;

	/** Locks horizontal offset. */
	bool bOffsetX = false;

	/** Locks vertical offset. */
	bool bOffsetY = false;

	// Warp

	/** Locks warp strength. */
	bool bWarpStrength = false;

	/** Locks warp octaves. */
	bool bWarpOctaves = false;

	/** Locks the warp type. */
	bool bWarpType = false;

	/** Locks the warp source input. */
	bool bWarpSource = false;

	/** Locks the warp target input. */
	bool bWarpTarget = false;

	// Ridged / Billow

	/** Locks gain value (used in ridged/billow noise). */
	bool bGain = false;

	/** Locks bias value (used in ridged/billow noise). */
	bool bBias = false;

	/** Locks usage of absolute value in noise shaping. */
	bool bUseAbsValue = false;

	/** Locks the ridge inversion toggle. */
	bool bInvertRidges = false;

	// Cellular / Voronoi

	/** Locks cellular distance function. */
	bool bCellularDistance = false;

	/** Locks cellular return type. */
	bool bCellularReturn = false;

	/** Locks cell jitter amount. */
	bool bCellJitter = false;

	/** Locks the number of cells. */
	bool bCellCount = false;

	// Gabor

	/** Locks the number of impulses for Gabor noise. */
	bool bGaborImpulseCount = false;

	/** Locks angular variance of Gabor impulses. */
	bool bGaborAngleVariance = false;

	/** Locks sigma of Gabor noise */
	bool bGaborSigma = false;

	/** Locks impulse spread of gabor noise */
	bool bGaborImpulseSpread = false;

	/**
	 * Sets all lock flags to the specified value.
	 *
	 * @param value Whether to enable or disable all locks.
	 */
	void SetAll(bool value)
	{
		bSeed = bRoughness = bMarbling = bLowFreq = bHighFreq =
			bTurbulence = bTurbRes = bTurbRoughness = bTurbLow = bTurbHigh =
			bTurbMarbling = bExpShift = bOffsetX = bOffsetY =
			bWarpStrength = bWarpOctaves = bWarpType = bWarpSource = bWarpTarget =
			bGain = bBias = bUseAbsValue = bInvertRidges =
			bCellularDistance = bCellularReturn = bCellJitter = bCellCount =
			bGaborImpulseCount = bGaborAngleVariance = bGaborSigma = bGaborImpulseSpread = 
			value;
	}

	/**
	 * Checks if all lock flags are set to true.
	 *
	 * @return True if all parameters are locked, false otherwise.
	 */
	bool AreAllLocked() const
	{
		return bSeed && bRoughness && bMarbling && bLowFreq && bHighFreq &&
			bTurbulence && bTurbRes && bTurbRoughness && bTurbLow && bTurbHigh &&
			bTurbMarbling && bExpShift && bOffsetX && bOffsetY &&
			bWarpStrength && bWarpOctaves && bWarpType && bWarpSource &&
			bGain && bBias && bUseAbsValue && bInvertRidges &&
			bCellularDistance && bCellularReturn && bCellJitter && bCellCount &&
			bGaborSigma && bGaborImpulseSpread && bGaborImpulseCount && bGaborAngleVariance && 
			bWarpTarget;
	}
};


class NoisePropertyModel : public IModel
{
public:
	virtual std::string GetId() const override;
	virtual void Reset() override;

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

	NoiseType GetType() const;
	void SetType(NoiseType type);

	int turbulenceResIndex = 2;
private:
	float RandF(float min, float max) const;

	bool bLocked = false;
	int resolutionIndex = 3;
	int randomStyle = 0;

	LockFlags locks;
	NoiseProperties props;
	NoiseType type = NoiseType::Perlin;

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