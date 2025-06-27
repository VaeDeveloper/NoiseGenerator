#pragma once

#include "NoiseTypes.h"
#include <functional>

class RandomGenerator;

namespace NG
{
	using NoiseFunc = std::function<float* (int res, const NoiseProperties*, std::function<bool(float)>)>;

	NoiseFunc GetNoiseFunction(NoiseType type);

	float CellDist(float x1, float y1, float x2, float y2, DistanceMetric metric);

	float Lerp(float a, float b, float t);
	float Fade(float t);
	float Gradient2D(int ix, int iy, float x, float y, unsigned int seed);
	float Simplex2D(float x, float y, RandomGenerator& rng);

	float* StupidNoise1D(int res, int freq, float* data2, float scale, unsigned int seed);
	float* StupidNoise2D(int res, int freq, float* data2, float scale, unsigned int seed);
	float* StupidNoise3D(int res, int freq, float* data2, float scale, unsigned int seed);


	float* ValueNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress);
	float* FBMNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
	float* PerlinNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
	float* WorleyNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress = nullptr);
	float* SimplexFBMNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
	float* RidgedNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress);
	float* CellularNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
	float* VoronoiNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> cb);
	float* DomainWarpNoise2D(int resolution, const NoiseProperties* props, std::function<bool(float)> progressCallback);
	float* BillowNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress);
	float* WhiteNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
	float* OpenSimplexNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
	float* GaborNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress);
}