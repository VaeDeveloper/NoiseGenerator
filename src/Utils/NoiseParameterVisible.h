#pragma once

#include "NoiseTypes.h"


/**
 * Enum representing each editable parameter in NoiseProperties.
 * Used for visibility filtering and UI binding.
 */
enum class NoiseParameter
{
	// === Base ===
	BaseFrequency,
	Seed,
	Resolution,
	Roughness,
	LowFreqSkip,
	HighFreqSkip,
	Marbling,

	// === Turbulence ===
	Turbulence,
	TurbulenceRes,
	TurbulenceRoughness,
	TurbulenceLowFreqSkip,
	TurbulenceHighFreqSkip,
	TurbulenceMarbling,
	TurbulenceExpShift,
	TurbulenceOffsetX,
	TurbulenceOffsetY,

	// === Warp ===
	WarpStrength,
	WarpOctaves,
	WarpType,
	WarpSource,
	WarpTarget,

	// === Ridged / Billow ===
	Gain,
	Bias,
	UseAbsValue,
	InvertRidges,

	// === Cellular / Voronoi / Worley ===
	CellularDistance,
	CellularReturn,
	CellJitter,
	CellCount,

	// === Gabor ===
	GaborImpulseCount,
	GaborAngleVariance,
	GaborSigma,
	GaborFrequency,
	GaborImpulseSpread,

	// === Spectral ===
	SpectralType
};

/** Returns whether the given parameter should be visible for the specified noise type. */
bool IsPropertyVisible(NoiseType type, NoiseParameter param);

