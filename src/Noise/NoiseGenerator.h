#pragma once
#include "NoiseTypes.h"

float* StupidNoise1D(int res, int freq, float* data2, float scale);
float* StupidNoise2D(int res, int freq, float* data2, float scale);
float* StupidNoise3D(int res, int freq, float* data2, float scale);

float* PerlinNoise1D(int res, const noise_properties* props);
float* PerlinNoise2D(int res, const noise_properties* props);
float* PerlinNoise3D(int res, const noise_properties* props);

float* SphereNoise(const float* input_noise, int input_res, float* output_noise, int output_res, int face_index);