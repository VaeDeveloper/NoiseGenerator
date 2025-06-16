#pragma once

#include "NoiseTypes.h"
#include <functional>

namespace NG
{
	float* StupidNoise1D(int res, int freq, float* data2, float scale, unsigned int seed);
	float* StupidNoise3D(int res, int freq, float* data2, float scale, unsigned int seed);
	float* StupidNoise3D(int res, int freq, float* data2, float scale, unsigned int seed);
	float* PerlinNoise2D(int res, const noise_properties* props, std::function<bool(float)> onProgress);
}