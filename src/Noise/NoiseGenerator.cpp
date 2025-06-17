#include "NoiseGenerator.h"
#include "Noise/NoiseMath.h"
#include "Logger/Logger.h"
#include "Logger/LoggerMacro.h"
#include "Utils/RandomGenerator.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

#define PI      3.14159265358979323846264338327950f
#define PI2     6.28318530717958647692528676655901f
#define EPSILON 0.00000001000000000000000000000000f

DEFINE_LOG_CATEGORY(LogNoise);

namespace NG
{
	float* StupidNoise1D(int res, int freq, float* data2, float scale, unsigned int seed)
	{
		float* data1 = (float*)calloc(sizeof(float), freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res);
		if(!data1 || !data2) {
			NGLOG(LogNoise, Error, "Out of memory");
			throw std::runtime_error("Out of memory");
		}

		RandomGenerator rng(seed);
		for(int i = 0; i < freq; i++)
			data1[i] = rng.NextFloat();

		for(int x = 0; x < res; x++) 
		{
			int x3 = (x * freq) / res - 1;
			float tmp[4];
			for(int x2 = 0; x2 < 4; x2++)
			{
				tmp[x2] = data1[CalcIndex1D(x2 + x3, freq)];
			}
			float xf = (float)(x * freq) / res;
			xf -= floorf(xf);
			data2[CalcIndex1D(x, res)] += Interpolate1D(tmp, xf) * scale;
		}

		free(data1);
		return data2;
	}

	float* StupidNoise2D(int res, int freq, float* data2, float scale, unsigned int seed)
	{
		float* data1 = (float*)calloc(sizeof(float), freq * freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res * res);
		if(!data1 || !data2) {
			NGLOG(LogNoise, Error, "Out of memory");
			throw std::runtime_error("Out of memory");
		}

		RandomGenerator rng(seed);
		for(int i = 0; i < freq * freq; i++)
			data1[i] = rng.NextFloat();

		for(int y = 0; y < res; y++) {
			for(int x = 0; x < res; x++) {
				int x3 = (x * freq) / res - 1;
				int y3 = (y * freq) / res - 1;
				float tmp[16];

				for(int y2 = 0; y2 < 4; y2++)
					for(int x2 = 0; x2 < 4; x2++)
						tmp[x2 + y2 * 4] = data1[CalcIndex2D(x2 + x3, y2 + y3, freq)];

				float xf = (float)(x * freq) / res;
				float yf = (float)(y * freq) / res;
				xf -= floorf(xf);
				yf -= floorf(yf);

				data2[CalcIndex2D(x, y, res)] += Interpolate2D(tmp, xf, yf) * scale;
			}
		}

		free(data1);
		return data2;
	}

	float* StupidNoise3D(int res, int freq, float* data2, float scale, unsigned int seed)
	{
		float* data1 = (float*)calloc(sizeof(float), freq * freq * freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res * res * res);
		if(!data1 || !data2) 
		{
			NGLOG(LogNoise, Error, "Out of memory");
			throw std::runtime_error("Out of memory");
		}

		RandomGenerator rng(seed);
		for(int i = 0; i < freq * freq * freq; i++)
			data1[i] = rng.NextFloat();

		for(int z = 0; z < res; z++)
			for(int y = 0; y < res; y++)
				for(int x = 0; x < res; x++) {
					int x3 = (x * freq) / res - 1;
					int y3 = (y * freq) / res - 1;
					int z3 = (z * freq) / res - 1;
					float tmp[64];

					for(int z2 = 0; z2 < 4; z2++)
						for(int y2 = 0; y2 < 4; y2++)
							for(int x2 = 0; x2 < 4; x2++)
								tmp[x2 + y2 * 4 + z2 * 16] =
								data1[CalcIndex3D(x2 + x3, y2 + y3, z2 + z3, freq)];

					float xf = (float)(x * freq) / res;
					float yf = (float)(y * freq) / res;
					float zf = (float)(z * freq) / res;
					xf -= floorf(xf);
					yf -= floorf(yf);
					zf -= floorf(zf);

					data2[CalcIndex3D(x, y, z, res)] += Interpolate3D(tmp, xf, yf, zf) * scale;
				}

		free(data1);
		return data2;
	}

	float* PerlinNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress)
	{
		float* data = nullptr;
		float scale = 1.0f;
		int freq = 2;
		int octaves = 0;
		while((1 << octaves) < res) octaves++;

		for(int level = 0; level < octaves; level++) {
			if(level >= in_props->low_freq_skip && level <= octaves - in_props->high_freq_skip) {
				unsigned int levelSeed = in_props->seed + level * 31;
				data = StupidNoise2D(res, freq, data, scale, levelSeed);
			}

			if(onProgress && !onProgress((float)level / octaves * 0.4f)) {
				if(data) free(data);
				return nullptr;
			}

			freq *= 2;
			scale *= in_props->roughness;
		}

		if(!data) {
			data = (float*)calloc(sizeof(float), res * res);
			if(!data) 
			{
				NGLOG(LogNoise, Error, "Out of memory");
				throw std::runtime_error("Out of memory");
			}
			return data;
		}

		// === Turbulence Pass ===
		if(in_props->turbulence != 0.0f) {
			NoiseProperties prop = *in_props;
			prop.turbulence = 0.0f;
			prop.roughness = in_props->turbulence_roughness;
			prop.low_freq_skip = in_props->turbulence_low_freq_skip;
			prop.high_freq_skip = in_props->turbulence_high_freq_skip;
			prop.marbling = in_props->turbulence_marbling;

			int turbulence_res = 8 << in_props->turbulence_res;
			float turbulence_exp = powf(2.0f, in_props->turbulence_expshift);

			prop.seed = in_props->seed + 100;
			float* dx = PerlinNoise2D(turbulence_res, &prop, [] (float) { return true; });
			prop.seed = in_props->seed + 200;
			float* dy = PerlinNoise2D(turbulence_res, &prop, [] (float) { return true; });

			if(!dx || !dy) 
			{
				if(data) free(data);
				if(dx) free(dx);
				if(dy) free(dy);
				NGLOG(LogNoise, Error, "Turbulence sub-pass canceled or failed");
				return nullptr;
			}

			float* temp = new float[res * res];
			memcpy(temp, data, res * res * sizeof(float));

			for(int j = 0; j < res; j++) 
			{
				for(int i = 0; i < res; i++) 
				{
					float x = Sample2D(dx, turbulence_res, turbulence_res, (float)i / res, (float)j / res) * 2.0f - 1.0f;
					float y = Sample2D(dy, turbulence_res, turbulence_res, (float)i / res, (float)j / res) * 2.0f - 1.0f;

					if(turbulence_exp != 1.0f) {
						x = powf(fabsf(x), turbulence_exp) * (x >= 0.0f ? 1.0f : -1.0f);
						y = powf(fabsf(y), turbulence_exp) * (y >= 0.0f ? 1.0f : -1.0f);
					}

					x += in_props->turbulence_offset_x;
					y += in_props->turbulence_offset_y;

					x = x * in_props->turbulence / 64.0f + (float)i / res;
					y = y * in_props->turbulence / 64.0f + (float)j / res;

					data[i + j * res] = Sample2D(temp, res, res, x, y);
				}

				if(onProgress && !onProgress(0.4f + (float)j / res * 0.5f)) 
				{
					delete[] temp;
					free(data);
					free(dx);
					free(dy);
					return nullptr;
				}
			}

			delete[] temp;
			free(dx);
			free(dy);
		}

		// === Normalize ===
		float min_v = data[0], max_v = data[0];
		for(int i = 1; i < res * res; i++) 
		{
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}
		for(int i = 0; i < res * res; i++) 
		{
			data[i] = (data[i] - min_v) / (max_v - min_v);
		}

		// === Marbling ===
		if(in_props->marbling != 0.0f)
		{
			for(int i = 0; i < res * res; i++) 
			{
				data[i] = sinf(PI2 * data[i] * in_props->marbling) * 0.5f + 0.5f;
			}
		}

		if(onProgress && !onProgress(1.0f)) 
		{
			free(data);
			return nullptr;
		}

		return data;
	}
}