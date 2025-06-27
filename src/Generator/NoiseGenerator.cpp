#include "NoiseGenerator.h"
#include "NoiseMath.h"
#include "Logger.h"
#include "LoggerMacro.h"
#include "RandomGenerator.h"
#include "ThirdParty/OpenSimplex2S.hpp"
#include <cmath>
#include <cstdlib>
#include <cstring>

#define PI      3.14159265358979323846264338327950f
#define PI2     6.28318530717958647692528676655901f
#define EPSILON 0.00000001000000000000000000000000f

DEFINE_LOG_CATEGORY(LogNoise);

namespace NG
{
	NoiseFunc GetNoiseFunction(NoiseType type)
	{
		switch(type)
		{
		case NoiseType::Value: return ValueNoise2D;
		case NoiseType::Perlin: return PerlinNoise2D;
		case NoiseType::Simplex: return SimplexFBMNoise2D;
		case NoiseType::FBM: return FBMNoise2D;
		case NoiseType::Ridged: return RidgedNoise2D;
		case NoiseType::Cellular: return CellularNoise2D;
		case NoiseType::Voronoi: return VoronoiNoise2D;
		case NoiseType::Worley: return WorleyNoise2D;
		case NoiseType::Billow: return BillowNoise2D;
		case NoiseType::White: return WhiteNoise2D;
		case NoiseType::OpenSimplex: return OpenSimplexNoise2D;
		default: return nullptr;
		}
	}

	float CellDist(float x1, float y1, float x2, float y2, DistanceMetric metric)
	{
		switch(metric) {
		case DistanceMetric::Manhattan:
			return fabsf(x1 - x2) + fabsf(y1 - y2);
		case DistanceMetric::Natural:
			return 0.5f * (fabsf(x1 - x2) + fabsf(y1 - y2)) + 0.5f * sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		default: // Euclidean
			return sqrtf((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
		}
	}

	float Lerp(float a, float b, float t) 
	{
		return a + t * (b - a);
	}
	float Fade(float t)
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}
	float Gradient2D(int ix, int iy, float x, float y, unsigned int seed)
	{
		
		uint32_t hash = static_cast<uint32_t>(ix * 374761393 + iy * 668265263 + seed * 982451653);
		hash = (hash ^ (hash >> 13)) * 1274126177;
		hash = hash ^ (hash >> 16);

		
		float angle = (hash % 360) * 3.14159265f / 180.0f;
		float gx = std::cos(angle);
		float gy = std::sin(angle);

		
		float dx = x - static_cast<float>(ix);
		float dy = y - static_cast<float>(iy);

		return gx * dx + gy * dy;;
	}

	float Simplex2D(float x, float y, RandomGenerator& rng)
	{
		static thread_local std::vector<int> perm;
		if(perm.empty()) {
			perm.resize(512);
			std::vector<int> p(256);
			for(int i = 0; i < 256; ++i) p[i] = i;
			std::shuffle(p.begin(), p.end(), std::mt19937(rng.NextInt(0, 1 << 30)));
			for(int i = 0; i < 512; ++i) perm[i] = p[i % 256];
		}

		const float F2 = 0.366025403f;  // (√3 - 1) / 2
		const float G2 = 0.211324865f;  // (3 - √3) / 6

		float s = (x + y) * F2;
		int i = static_cast<int>(std::floor(x + s));
		int j = static_cast<int>(std::floor(y + s));

		float t = (i + j) * G2;
		float X0 = i - t;
		float Y0 = j - t;
		float x0 = x - X0;
		float y0 = y - Y0;

		int i1, j1;
		if(x0 > y0) { i1 = 1; j1 = 0; } // lower triangle
		else { i1 = 0; j1 = 1; }         // upper triangle

		float x1 = x0 - i1 + G2;
		float y1 = y0 - j1 + G2;
		float x2 = x0 - 1.0f + 2.0f * G2;
		float y2 = y0 - 1.0f + 2.0f * G2;

		int ii = i & 255;
		int jj = j & 255;

		auto grad = [] (int hash, float x, float y) {
			int h = hash & 7;
			float u = h < 4 ? x : y;
			float v = h < 4 ? y : x;
			return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
			};

		float n0 = 0.0f, n1 = 0.0f, n2 = 0.0f;

		auto t0 = 0.5f - x0 * x0 - y0 * y0;
		if(t0 >= 0) {
			t0 *= t0;
			n0 = t0 * t0 * grad(perm[ii + perm[jj]], x0, y0);
		}
		auto t1 = 0.5f - x1 * x1 - y1 * y1;
		if(t1 >= 0) {
			t1 *= t1;
			n1 = t1 * t1 * grad(perm[ii + i1 + perm[jj + j1]], x1, y1);
		}
		auto t2 = 0.5f - x2 * x2 - y2 * y2;
		if(t2 >= 0) {
			t2 *= t2;
			n2 = t2 * t2 * grad(perm[ii + 1 + perm[jj + 1]], x2, y2);
		}

		return 70.0f * (n0 + n1 + n2);
	}

	float* StupidNoise1D(int res, int freq, float* data2, float scale, unsigned int seed)
	{
		float* data1 = (float*)calloc(sizeof(float), freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res);
		if(!data1 || !data2) 
		{
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
		if(res <= 0 || freq <= 0)
		{
			NGLOG(LogNoise, Error, "Invalid resolution or frequency in StupidNoise2D");
			throw std::invalid_argument("Resolution and frequency must be > 0");
		} 

		if(res <= 0 || freq <= 0)
		{
			NGLOG(LogNoise, Error, "Invalid resolution or frequency in StupidNoise2D");
			throw std::invalid_argument("Resolution and frequency must be > 0");
		}

		float* data1 = (float*)calloc(sizeof(float), freq * freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res * res);
		if(!data1 || !data2) 
		{
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

	float* FBMNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress)
	{
		if(!in_props) return nullptr;

		float* data = nullptr;
		float scale = 1.0f;
		int freq = in_props->base_frequency;
		int octaves = 0;
		while((1 << octaves) < res) octaves++;

		for(int level = 0; level < octaves; level++) {
			if(level >= in_props->low_freq_skip && level <= octaves - in_props->high_freq_skip) 
			{
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
		if(in_props->turbulence != 0.0f) 
		{
			NoiseProperties prop = *in_props;
			prop.turbulence = 0.0f;
			prop.roughness = in_props->turbulence_roughness;
			prop.low_freq_skip = in_props->turbulence_low_freq_skip;
			prop.high_freq_skip = in_props->turbulence_high_freq_skip;
			prop.marbling = in_props->turbulence_marbling;

			int turbulence_res = 8 << in_props->turbulence_res;
			float turbulence_exp = powf(2.0f, in_props->turbulence_expshift);

			prop.seed = in_props->seed + 100;
			float* dx = FBMNoise2D(turbulence_res, &prop, [] (float) { return true; });
			prop.seed = in_props->seed + 200;
			float* dy = FBMNoise2D(turbulence_res, &prop, [] (float) { return true; });

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

	float* WorleyNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		if(!props) return nullptr;

		unsigned int seed = static_cast<unsigned int>(props->seed);
		int pointCount = std::max(1, 32 << std::max(0, (int)(props->low_freq_skip - props->high_freq_skip)));

		srand(seed);
		std::vector<std::pair<float, float>> points;
		points.reserve(pointCount);

		for(int i = 0; i < pointCount; ++i) {
			points.emplace_back(static_cast<float>(rand()) / RAND_MAX,
				static_cast<float>(rand()) / RAND_MAX);
		}

		float* data = (float*)calloc(sizeof(float), res * res);
		if(!data) {
			NGLOG(LogNoise, Error, "Out of memory in WorleyNoise2D");
			return nullptr;
		}

		for(int y = 0; y < res; ++y) {
			for(int x = 0; x < res; ++x) {
				float fx = static_cast<float>(x) / res;
				float fy = static_cast<float>(y) / res;

				float minDist = 999999.0f;
				for(const auto& [px, py] : points) {
					float d = Distance(fx, fy, px, py);
					if(d < minDist) minDist = d;
				}

				data[x + y * res] = minDist;
			}

			if(onProgress && !onProgress((float)y / res * 0.4f)) {
				free(data);
				return nullptr;
			}
		}

		// === Turbulence Pass ===
		if(props->turbulence != 0.0f) 
		{
			NoiseProperties prop = *props;
			prop.turbulence = 0.0f;
			prop.roughness = props->turbulence_roughness;
			prop.low_freq_skip = props->turbulence_low_freq_skip;
			prop.high_freq_skip = props->turbulence_high_freq_skip;
			prop.marbling = props->turbulence_marbling;

			int turbulence_res = 8 << props->turbulence_res;
			float turbulence_exp = powf(2.0f, props->turbulence_expshift);

			prop.seed = props->seed + 100;
			float* dx = WorleyNoise2D(turbulence_res, &prop, [] (float) { return true; });
			prop.seed = props->seed + 200;
			float* dy = WorleyNoise2D(turbulence_res, &prop, [] (float) { return true; });

			if(!dx || !dy) 
			{
				if(data) free(data);
				if(dx) free(dx);
				if(dy) free(dy);
				NGLOG(LogNoise, Error, "Turbulence sub-pass failed");
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

					x += props->turbulence_offset_x;
					y += props->turbulence_offset_y;

					x = x * props->turbulence / 64.0f + (float)i / res;
					y = y * props->turbulence / 64.0f + (float)j / res;

					data[i + j * res] = Sample2D(temp, res, res, x, y);
				}

				if(onProgress && !onProgress(0.4f + (float)j / res * 0.4f)) 
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

		// Normalize
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

		// Optional marbling pass
		if(props->marbling != 0.0f) 
		{
			for(int i = 0; i < res * res; i++) {
				data[i] = sinf(PI2 * data[i] * props->marbling) * 0.5f + 0.5f;
			}
		}

		if(onProgress && !onProgress(1.0f)) 
		{
			free(data);
			return nullptr;
		}

		return data;
	}

	float* SimplexFBMNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		if(!props) return nullptr;

		const int octaves = std::max(1, (int)(std::log2(res)));
		float amplitude = 1.0f;
		float frequency = (float)props->base_frequency;
		float totalAmplitude = 0.0f;

		float* data = (float*)calloc(res * res, sizeof(float));
		if(!data) return nullptr;

		for(int octave = 0; octave < octaves; ++octave)
		{
			if(octave < props->low_freq_skip || octave > octaves - props->high_freq_skip)
			{
				frequency *= 2.0f;
				amplitude *= props->roughness;
				continue;
			}

			RandomGenerator rng(props->seed + octave * 1337);

			for(int y = 0; y < res; ++y)
			{
				for(int x = 0; x < res; ++x)
				{
					float nx = (float)x / res * frequency;
					float ny = (float)y / res * frequency;

					float value = Simplex2D(nx, ny, rng);  // 👈 Тебе нужно реализовать эту функцию отдельно
					data[x + y * res] += value * amplitude;
				}
			}

			totalAmplitude += amplitude;
			amplitude *= props->roughness;
			frequency *= 2.0f;

			if(onProgress && !onProgress((float)octave / octaves * 0.5f))
			{
				free(data);
				return nullptr;
			}
		}

		// === Normalize
		for(int i = 0; i < res * res; ++i)
		{
			data[i] = (data[i] / totalAmplitude) * 0.5f + 0.5f;
		}

		// === Marbling
		if(props->marbling != 0.0f)
		{
			for(int i = 0; i < res * res; ++i)
			{
				data[i] = std::sinf(data[i] * PI2 * props->marbling) * 0.5f + 0.5f;
			}
		}

		if(onProgress && !onProgress(1.0f))
		{
			free(data);
			return nullptr;
		}

		return data;
	}

	float* ValueNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress)
	{
		if(!in_props) return nullptr;

		float* data = nullptr;
		float scale = 1.0f;
		int freq = 2;
		int octaves = 0;
		while((1 << octaves) < res) octaves++;

		for(int level = 0; level < octaves; level++) {
			if(level >= in_props->low_freq_skip && level <= octaves - in_props->high_freq_skip) {
				unsigned int levelSeed = in_props->seed + level * 17;
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
			if(!data) throw std::runtime_error("Out of memory");
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
			float* dx = ValueNoise2D(turbulence_res, &prop, [] (float) { return true; });
			prop.seed = in_props->seed + 200;
			float* dy = ValueNoise2D(turbulence_res, &prop, [] (float) { return true; });

			if(!dx || !dy) {
				free(data);
				if(dx) free(dx);
				if(dy) free(dy);
				return nullptr;
			}

			float* temp = new float[res * res];
			memcpy(temp, data, res * res * sizeof(float));

			for(int j = 0; j < res; j++) {
				for(int i = 0; i < res; i++) {
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

				if(onProgress && !onProgress(0.4f + (float)j / res * 0.5f)) {
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
		for(int i = 1; i < res * res; i++) {
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}
		for(int i = 0; i < res * res; i++) {
			data[i] = (data[i] - min_v) / (max_v - min_v);
		}

		// === Marbling ===
		if(in_props->marbling != 0.0f) {
			for(int i = 0; i < res * res; i++) {
				data[i] = sinf(PI2 * data[i] * in_props->marbling) * 0.5f + 0.5f;
			}
		}

		if(onProgress && !onProgress(1.0f)) {
			free(data);
			return nullptr;
		}

		return data;
	}

	float* PerlinNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress) 
	{
		if(!props) return nullptr;

		float* data = (float*)calloc(sizeof(float), res * res);
		if(!data) return nullptr;

		int freq = 2;
		float scale = 1.0f;
		int octaves = 0;
		while((1 << octaves) < res) ++octaves;

		for(int o = 0; o < octaves; ++o) {
			if(o >= props->low_freq_skip && o <= octaves - props->high_freq_skip) {
				for(int y = 0; y < res; ++y) {
					for(int x = 0; x < res; ++x) {
						float fx = (float)x / res * freq;
						float fy = (float)y / res * freq;

						int ix = (int)floorf(fx);
						int iy = (int)floorf(fy);

						float dx = fx - ix;
						float dy = fy - iy;

						float u = NG::Fade(dx);
						float v = NG::Fade(dy);

						float g00 = NG::Gradient2D(ix, iy, fx, fy, props->seed + o);
						float g10 = NG::Gradient2D(ix + 1, iy, fx, fy, props->seed + o);
						float g01 = NG::Gradient2D(ix, iy + 1, fx, fy, props->seed + o);
						float g11 = NG::Gradient2D(ix + 1, iy + 1, fx, fy, props->seed + o);

						float gx0 = Lerp(g00, g10, u);
						float gx1 = Lerp(g01, g11, u);
						float val = Lerp(gx0, gx1, v);

						data[x + y * res] += val * scale;
					}

					if(onProgress && !onProgress((float)o / octaves + (float)y / res / octaves)) {
						free(data);
						return nullptr;
					}
				}
			}

			freq *= 2;
			scale *= props->roughness;
		}

		// Normalize
		float min_v = data[0], max_v = data[0];
		for(int i = 1; i < res * res; ++i) {
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}
		float range = max_v - min_v;
		for(int i = 0; i < res * res; ++i)
			data[i] = (data[i] - min_v) / range;

		return data;
	}

	float* RidgedNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress)
	{
		if(!in_props) return nullptr;

		float* data = (float*)calloc(sizeof(float), res * res);
		if(!data) {
			NGLOG(LogNoise, Error, "Out of memory");
			return nullptr;
		}

		int octaves = 0;
		while((1 << octaves) < res) octaves++;

		float scale = 1.0f;
		int freq = in_props->base_frequency;
		float weight = 1.0f;

		for(int level = 0; level < octaves; ++level) {
			if(level < in_props->low_freq_skip || level >(octaves - in_props->high_freq_skip)) {
				freq *= 2;
				scale *= in_props->roughness;
				continue;
			}

			unsigned int levelSeed = in_props->seed + level * 37;
			float* octave = StupidNoise2D(res, freq, nullptr, 1.0f, levelSeed);

			if(!octave) {
				free(data);
				return nullptr;
			}

			for(int i = 0; i < res * res; i++) {
				float base = octave[i] * 2.0f - 1.0f; // [-1, 1] noise range
				float val = 1.0f - fabsf(base);      // ridge shape

				val *= val; // emphasize ridges
				val *= weight;

				if(in_props->invert_ridges)
					val = 1.0f - val;

				data[i] += val * scale;

				weight = std::clamp(val * in_props->gain + in_props->bias, 0.0f, 1.0f);
			}

			free(octave);
			freq *= 2;
			scale *= in_props->roughness;

			if(onProgress && !onProgress((float)level / octaves * 0.9f)) {
				free(data);
				return nullptr;
			}
		}

		// === Normalize ===
		float min_v = data[0], max_v = data[0];
		for(int i = 1; i < res * res; i++) {
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}
		for(int i = 0; i < res * res; i++) {
			data[i] = (data[i] - min_v) / (max_v - min_v);
		}

		// === Marbling ===
		if(in_props->marbling != 0.0f) {
			for(int i = 0; i < res * res; i++) {
				data[i] = sinf(PI2 * data[i] * in_props->marbling) * 0.5f + 0.5f;
			}
		}

		if(onProgress && !onProgress(1.0f)) {
			free(data);
			return nullptr;
		}

		return data;
	}

	float* CellularNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		if(!props) return nullptr;

		int grid = props->cell_count;
		float jitter = props->cell_jitter;
		RandomGenerator rng(props->seed);

		// Generate feature points per cell
		std::vector<std::pair<float, float>> featurePoints;
		featurePoints.reserve(grid * grid);
		for(int gy = 0; gy < grid; ++gy) {
			for(int gx = 0; gx < grid; ++gx) {
				float fx = (gx + rng.NextFloat(0.5f - jitter * 0.5f, 0.5f + jitter * 0.5f)) / grid;
				float fy = (gy + rng.NextFloat(0.5f - jitter * 0.5f, 0.5f + jitter * 0.5f)) / grid;
				featurePoints.emplace_back(fx, fy);
			}
		}

		float* data = (float*)calloc(sizeof(float), res * res);
		if(!data) return nullptr;

		for(int y = 0; y < res; ++y) {
			for(int x = 0; x < res; ++x) {
				float px = (float)x / res;
				float py = (float)y / res;

				float min1 = 999.0f, min2 = 999.0f;
				for(auto& [fx, fy] : featurePoints) {
					float dist = CellDist(px, py, fx, fy, props->cellular_distance);
					if(dist < min1) {
						min2 = min1;
						min1 = dist;
					}
					else if(dist < min2) {
						min2 = dist;
					}
				}

				float value = 0.0f;
				switch(props->cellular_return) {
				case CellularReturnType::F2MinusF1: value = min2 - min1; break;
				case CellularReturnType::EdgeDistance: value = min2; break;
				default: value = min1; break;
				}

				data[y * res + x] = value;
			}

			if(onProgress && !onProgress((float)y / res)) {
				free(data);
				return nullptr;
			}
		}

		// Normalize
		float min_v = data[0], max_v = data[0];
		for(int i = 1; i < res * res; i++) {
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}
		for(int i = 0; i < res * res; i++) {
			data[i] = (data[i] - min_v) / (max_v - min_v);
		}

		// Marbling
		if(props->marbling != 0.0f) {
			for(int i = 0; i < res * res; i++) {
				data[i] = sinf(PI2 * data[i] * props->marbling) * 0.5f + 0.5f;
			}
		}

		return data;
	}

	float* VoronoiNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> cb)
	{
		NoiseProperties mod = *props;
		mod.cellular_return = CellularReturnType::EdgeDistance;
		return CellularNoise2D(res, &mod, cb);
	}

	float* DomainWarpNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		if(!props) return nullptr;

		auto warpFunc = GetNoiseFunction(props->warp_source);
		auto baseFunc = GetNoiseFunction(props->warp_target);

		if(!warpFunc || !baseFunc) return nullptr;

		const int size = res * res;
		float* result = new float[size];
		float* base = baseFunc(res, props, nullptr);
		if(!base)
		{
			delete[] result;
			return nullptr;
		}

		std::vector<float> xMap(size);
		std::vector<float> yMap(size);

		for(int y = 0; y < res; ++y)
		{
			for(int x = 0; x < res; ++x)
			{
				int i = y * res + x;
				xMap[i] = static_cast<float>(x);
				yMap[i] = static_cast<float>(y);
			}
		}

		if(props->warp_type != WarpType::None)
		{
			const float strength = props->warp_strength;
			for(int octave = 0; octave < props->warp_octaves; ++octave)
			{
				float scale = strength / (1 << octave); 

				float* warpX = warpFunc(res, props, nullptr);
				float* warpY = warpFunc(res, props, nullptr);
				if(!warpX || !warpY)
				{
					delete[] base;
					delete[] result;
					delete[] warpX;
					delete[] warpY;
					return nullptr;
				}

				for(int i = 0; i < size; ++i)
				{
					float dx = (warpX[i] * 2.0f - 1.0f) * scale;
					float dy = (warpY[i] * 2.0f - 1.0f) * scale;

					switch(props->warp_type)
					{
					case WarpType::Basic:
						xMap[i] += dx;
						yMap[i] += dy;
						break;

					case WarpType::Recursive:
						xMap[i] += dx;
						yMap[i] += dy;
						break;

					case WarpType::IQStyle:
						xMap[i] += std::sin(warpX[i] * 3.14159f) * scale;
						yMap[i] += std::cos(warpY[i] * 3.14159f) * scale;
						break;

					default:
						break;
					}
				}

				delete[] warpX;
				delete[] warpY;
			}
		}


		for(int y = 0; y < res; ++y)
		{
			if(onProgress && !onProgress(float(y) / res))
			{
				delete[] base;
				delete[] result;
				return nullptr;
			}

			for(int x = 0; x < res; ++x)
			{
				int i = y * res + x;
				int sampleX = std::clamp(static_cast<int>(xMap[i]), 0, res - 1);
				int sampleY = std::clamp(static_cast<int>(yMap[i]), 0, res - 1);
				result[i] = base[sampleY * res + sampleX];
			}
		}

		delete[] base;
		return result;
	}

	float* BillowNoise2D(int res, const NoiseProperties* in_props, std::function<bool(float)> onProgress)
	{
		if(!in_props) return nullptr;

		float* data = (float*)calloc(res * res, sizeof(float));
		if(!data) {
			NGLOG(LogNoise, Error, "Out of memory");
			return nullptr;
		}

		int octaves = 0;
		while((1 << octaves) < res) octaves++;

		float scale = 1.0f;
		int freq = in_props->base_frequency;
		float weight = 1.0f;

		for(int level = 0; level < octaves; ++level)
		{
			if(level < in_props->low_freq_skip || level >(octaves - in_props->high_freq_skip)) {
				freq *= 2;
				scale *= in_props->roughness;
				continue;
			}

			unsigned int levelSeed = in_props->seed + level * 31;
			float* octave = StupidNoise2D(res, freq, nullptr, 1.0f, levelSeed);
			if(!octave) {
				free(data);
				return nullptr;
			}

			for(int i = 0; i < res * res; ++i)
			{
				float base = octave[i] * 2.0f - 1.0f;     // [-1, 1]
				float val;
				if(in_props->invert_ridges)
					val = 1.0f - fabsf(base); // Ridged
				else
					val = fabsf(base);        // Billow

				val *= val;                              // softer falloff
				val *= weight;

				data[i] += val * scale;

				weight = std::clamp(val * in_props->gain + in_props->bias, 0.0f, 1.0f);
			}

			free(octave);
			freq *= 2;
			scale *= in_props->roughness;

			if(onProgress && !onProgress((float)level / octaves * 0.9f)) {
				free(data);
				return nullptr;
			}
		}

		// Normalize
		float min_v = data[0], max_v = data[0];
		for(int i = 1; i < res * res; ++i) {
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}
		for(int i = 0; i < res * res; ++i) {
			data[i] = (data[i] - min_v) / (max_v - min_v);
		}

		// === Marbling ===
		if(in_props->marbling != 0.0f) {
			for(int i = 0; i < res * res; i++) {
				data[i] = sinf(PI2 * data[i] * in_props->marbling) * 0.5f + 0.5f;
			}
		}
		return data;
	}

	float* WhiteNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		float* data = (float*)malloc(sizeof(float) * res * res);
		if(!data) return nullptr;

		RandomGenerator rng(props->seed);
		for(int i = 0; i < res * res; ++i)
		{
			data[i] = rng.NextFloat(); // [0,1]
			if(onProgress && i % 4096 == 0 && !onProgress(float(i) / (res * res)))
			{
				free(data);
				return nullptr;
			}
		}
		return data;
	}

	float* OpenSimplexNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		if(!props) return nullptr;

		float* data = (float*)calloc(sizeof(float), res * res);
		if(!data) return nullptr;

		OpenSimplex2S noiseGen(props->seed);

		const int octaves = props->base_frequency;
		const float persistence = props->gain;
		const float lacunarity = props->cell_count;

		float maxVal = 0.0f;
		float minVal = 99999.0f;
		float maxAmp = 0.0f;

		for(int y = 0; y < res; ++y)
		{
			for(int x = 0; x < res; ++x)
			{
				float nx = (float)x / (float)res;
				float ny = (float)y / (float)res;

				float freq = 1.0f;
				float amp = 1.0f;
				float total = 0.0f;

				for(int i = 0; i < octaves; ++i)
				{
					float n = (float)noiseGen.noise2(nx * freq, ny * freq); // [-1,1]
					total += n * amp;
					maxAmp += amp;

					freq *= lacunarity;
					amp *= persistence;
				}

				data[y * res + x] = total;

				if(total < minVal) minVal = total;
				if(total > maxVal) maxVal = total;
			}

			if(onProgress && !onProgress((float)y / res * 0.95f)) {
				free(data);
				return nullptr;
			}
		}

		// Normalize to [0,1]
		float range = maxVal - minVal;
		for(int i = 0; i < res * res; ++i)
		{
			data[i] = (data[i] - minVal) / range;
		}

		return data;
	}

	float* GaborNoise2D(int res, const NoiseProperties* props, std::function<bool(float)> onProgress)
	{
		if(!props) return nullptr;

		const int impulse_count = props->gabor_impulse_count;
		const float sigma = props->gabor_sigma;
		const float frequency = props->base_frequency;
		const float spread = props->gabor_impulse_spread;
		const float angle_variance = props->gabor_angle_variance;

		std::mt19937 rng(props->seed);
		std::uniform_real_distribution<float> uniform(0.0f, 1.0f);

		float* data = (float*)calloc(res * res, sizeof(float));
		if(!data) return nullptr;

		struct Impulse {
			float x, y;
			float angle;
		};

		std::vector<Impulse> impulses;
		impulses.reserve(impulse_count);
		for(int i = 0; i < impulse_count; ++i) {
			impulses.push_back({
				uniform(rng) * res,
				uniform(rng) * res,
				angle_variance * 3.14159f * (2.0f * uniform(rng) - 1.0f)
				});
		}

		const float invRes = 1.0f / res;
		const float sigma2 = sigma * sigma * res * res;

		for(int y = 0; y < res; ++y) {
			for(int x = 0; x < res; ++x) {
				float sum = 0.0f;

				for(const auto& imp : impulses) {
					float dx = x - imp.x;
					float dy = y - imp.y;

					float dist2 = dx * dx + dy * dy;
					if(dist2 > spread * spread * res * res)
						continue;

					float theta = imp.angle;
					float dot = dx * std::cos(theta) + dy * std::sin(theta);

					float weight = std::exp(-dist2 / (2.0f * sigma2));
					float wave = std::cos(2.0f * 3.14159f * frequency * dot * invRes);
					sum += weight * wave;
				}

				data[y * res + x] = sum;
			}

			if(onProgress && !onProgress(float(y) / res * 0.9f)) {
				free(data);
				return nullptr;
			}
		}

		// Нормализация
		float min_v = data[0], max_v = data[0];
		for(int i = 1; i < res * res; ++i) {
			if(data[i] < min_v) min_v = data[i];
			if(data[i] > max_v) max_v = data[i];
		}

		float range = max_v - min_v;
		if(range > 0.0f) {
			for(int i = 0; i < res * res; ++i) {
				data[i] = (data[i] - min_v) / range;
			}
		}

		return data;
	}

}