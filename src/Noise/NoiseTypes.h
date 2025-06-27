
#pragma once

#include <string>
#include <nlohmann/json.hpp>

enum class NoiseType
{
	//Classic
	Value,
	Perlin,
	Simplex,
	FBM,
	Worley,

	// Variant
	Ridged,
	Billow,
	DomainWarp,

	// Cellular / Voronoi
	Cellular,
	Voronoi,

	// Gabor Noise
	Gabor,

	// White Noise (random pixel values)
	White,

	// Gradient-free
	OpenSimplex,
	SuperSimplex,

	/** TODO in future */
	//// Experimental
	//IQNoise,       // Inigo Quilez style FBM
	//SwissTurbulence,
	//JordanNoise,   // Jordan Pegg-style warping
};

enum class WarpType 
{
	None,
	Basic,
	Recursive,
	IQStyle
};

enum class DistanceMetric 
{
	Euclidean,
	Manhattan,
	Natural
};

enum class CellularReturnType {
	F1,
	F2MinusF1,
	EdgeDistance
};

enum class SpectralType 
{
	None,
	White
};

struct NoiseProperties
{
	int base_frequency = 1;
	int seed = 42;
	int res = 0;
	float roughness = 0.5;
	int low_freq_skip = 0;
	int high_freq_skip = 0;
	float marbling = 0;

	float turbulence = 0.0f;
	int turbulence_res = 2;
	float turbulence_roughness = 0.5f;
	int turbulence_low_freq_skip = 0;
	int turbulence_high_freq_skip = 0;
	float turbulence_marbling = 0.0f;
	float turbulence_expshift = 0.0f;
	float turbulence_offset_x = 0.0f;
	float turbulence_offset_y = 0.0f;

	// === Warp (for DomainWarp / Jordan / IQNoise) ===
	float warp_strength = 0.0f;
	int warp_octaves = 1;
	WarpType warp_type = WarpType::None;
	NoiseType warp_source = NoiseType::Value;
	NoiseType warp_target = NoiseType::Value;

	// === Ridged / Billow ===
	float gain = 0.5f;
	float bias = 0.4f;
	bool use_abs_value = false;     // For Billow
	bool invert_ridges = true;     // For Ridged

	// === Cellular / Voronoi / Worley ===
	DistanceMetric cellular_distance = DistanceMetric::Euclidean;
	CellularReturnType cellular_return = CellularReturnType::F1;
	float cell_jitter = 1.0f;
	int cell_count = 32;

	// === Gabor noise ===
	int gabor_impulse_count = 16;
	float gabor_angle_variance = 1.0f;
	float gabor_sigma = 0.05f;          // Spread of gaussian kernel
	float gabor_frequency = 6.0f;       // Frequency of impulses
	float gabor_impulse_spread = 1.0f;  // Distance spread factor

	// === Spectral coloration ===
	SpectralType spectral_type = SpectralType::None;
};

struct NoisePreset
{
	std::string name;
	NoiseProperties properties;
	NoiseType type;
	int resolutionIndex;
};

