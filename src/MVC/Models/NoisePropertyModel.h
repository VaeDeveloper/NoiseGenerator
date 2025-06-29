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


/**
 * Encapsulates procedural noise properties and behavior.
 *
 * Provides support for resolution selection, randomization, mutation styles,
 * and property locking. Used to drive noise-based systems with customizable
 * and constrained parameters.
 */
class NoisePropertyModel : public IModel
{
public:
	virtual std::string GetId() const override;
	virtual void Reset() override;

	/** Returns a const reference to the current noise properties. */
	const NoiseProperties& Get() const;

	/** Returns a mutable reference to the noise properties. */
	NoiseProperties& Access();

	/**
	 * Randomizes the noise properties.
	 * @param respectLocks - If true, locked properties will remain unchanged.
	 */
	void Randomize(bool respectLocks = true);

	/**
	 * Applies a mutation to the current properties based on a style index.
	 * @param style - The mutation style index to apply.
	 */
	void Mutate(int style);

	/** Returns a reference to the lock flags used to control which properties are locked. */
	LockFlags& GetLockFlags();

	/** Sets the locked state for all lockable properties. */
	void SetLockAll(bool lock = true);

	/** Returns true if all lock flags are enabled. */
	bool IsAllLocked() const { return locks.AreAllLocked(); }

	/** Returns the index of the current randomization style. */
	int GetRandomStyle() const;

	/** Gets the current resolution index. */
	int GetResolutionIndex() const;

	/** Sets the current resolution index. */
	void SetResolutionIndex(int index);

	/** Gets the actual resolution value corresponding to the index. */
	int GetResolutionValue() const;

	/** Returns the number of available resolution options. */
	static constexpr int GetResolutionCount()
	{
		return sizeof(resolutions) / sizeof(resolutions[0]);
	}

	/** Returns a pointer to the list of resolution string values. */
	static constexpr const char* const* GetResolutions()
	{
		return resolutions;
	}

	/** Gets the current noise type (e.g., Perlin, Simplex, etc.). */
	NoiseType GetType() const;

	/** Sets the noise type. */
	void SetType(NoiseType type);

	/** Index used for turbulence resolution, default is 2. */
	int turbulenceResIndex = 2;

private:

	/**
	 * Returns a random float within a specified range.
	 * @param min - The minimum bound.
	 * @param max - The maximum bound.
	 */
	float RandF(float min, float max) const;

	/** If true, the entire property model is considered locked. */
	bool bLocked = false;

	/** Index into the resolutions array. */
	int resolutionIndex = 3;

	/** Index representing the current randomization style. */
	int randomStyle = 0;

	/** List of randomization style names. */
	static constexpr const char* randomStyles[] =
	{
		"Full Random",
		"Controlled Chaos",
		"Organic",
		"Extreme",
		"Minimal",
	};

	/** List of resolution options as string representations. */
	static constexpr const char* resolutions[] =
	{
		"8", "16", "32", "64", "128", "256", "512", "1024", "2048", "4096"
	};

	/** Lock flags that determine which properties are locked from modification. */
	LockFlags locks;

	/** The current set of noise generation properties. */
	NoiseProperties props;

	/** The currently selected noise type. */
	NoiseType type = NoiseType::Perlin;
};
