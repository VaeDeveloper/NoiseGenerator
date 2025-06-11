#include "NoiseGenerator.h"
#include "Math/vector.h"
#include "Noise/NoiseMath.h"
#include "Logger/Logger.h"
#include <cmath>
#include <cstdlib>
#include <cstring>

namespace NoiseGenerator
{
	float* StupidNoise1D(int res, int freq, float* data2, float scale)
	{
		float* data1 = nullptr;
		if(!data1) data1 = (float*)calloc(sizeof(float), freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res);
		if(!data1 || !data2)
		{
			Logger::Err("Out of memory");
			throw std::runtime_error("Out of memory");
		}

		for(int i = 0; i < freq; i++)
		{
			data1[i] = (float)rand() / 32767.0f;
		}

		for(int x = 0; x < res; x++)
		{
			int x3 = (x * freq) / res - 1;
			float tmp[64];

			for(int x2 = 0; x2 < 4; x2++)
			{
				tmp[x2] = data1[CalcIndex1D(x2 + x3, freq)];
			}

			float xf = (float)(x * freq) / (float)(res);
			xf -= floorf(xf);
			data2[CalcIndex1D(x, res)] += Interpolate1D(tmp, xf) * scale;
		}

		free(data1);
		return data2;
	}

	float* StupidNoise2D(int res, int freq, float* data2, float scale)
	{
		float* data1 = nullptr;
		if(!data1) data1 = (float*)calloc(sizeof(float), freq * freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res * res);
		if(!data1 || !data2)
		{
			Logger::Err("Out of memory");
			throw std::runtime_error("Out of memory");
		}

		for(int i = 0; i < freq * freq; i++)
		{
			data1[i] = (float)rand() / 32767.0f;
		}

		for(int y = 0; y < res; y++)
			for(int x = 0; x < res; x++)
			{
				int x3 = (x * freq) / res - 1;
				int y3 = (y * freq) / res - 1;
				float tmp[64];

				for(int y2 = 0; y2 < 4; y2++)
					for(int x2 = 0; x2 < 4; x2++)
					{
						tmp[x2 + y2 * 4] = data1[CalcIndex2D(x2 + x3, y2 + y3, freq)];
					}

				float xf = (float)(x * freq) / (float)(res);
				float yf = (float)(y * freq) / (float)(res);
				xf -= floorf(xf);
				yf -= floorf(yf);
				data2[CalcIndex2D(x, y, res)] += Interpolate2D(tmp, xf, yf) * scale;
			}

		free(data1);
		return data2;
	}

	float* StupidNoise3D(int res, int freq, float* data2, float scale)
	{
		float* data1 = nullptr;
		if(!data1) data1 = (float*)calloc(sizeof(float), freq * freq * freq);
		if(!data2) data2 = (float*)calloc(sizeof(float), res * res * res);
		if(!data1 || !data2)
		{
			Logger::Err("Out of memory");
			throw std::runtime_error("Out of memory");
		}

		for(int i = 0; i < freq * freq * freq; i++)
		{
			data1[i] = (float)rand() / 32767.0f;
		}

		for(int z = 0; z < res; z++)
			for(int y = 0; y < res; y++)
				for(int x = 0; x < res; x++)
				{
					int x3 = (x * freq) / res - 1;
					int y3 = (y * freq) / res - 1;
					int z3 = (z * freq) / res - 1;
					float tmp[64];

					for(int z2 = 0; z2 < 4; z2++)
						for(int y2 = 0; y2 < 4; y2++)
							for(int x2 = 0; x2 < 4; x2++)
							{
								tmp[x2 + y2 * 4 + z2 * 16] = data1[CalcIndex3D(x2 + x3, y2 + y3, z2 + z3, freq)];
							}

					float xf = (float)(x * freq) / (float)(res);
					float yf = (float)(y * freq) / (float)(res);
					float zf = (float)(z * freq) / (float)(res);
					xf -= floorf(xf);
					yf -= floorf(yf);
					zf -= floorf(zf);
					data2[CalcIndex3D(x, y, z, res)] += Interpolate3D(tmp, xf, yf, zf) * scale;
				}

		free(data1);
		return data2;
	}

	float* PerlinNoise1D(int res, const noise_properties* in_noise_properties)
	{
		float* data = nullptr;
		float  scale = 1.0f;
		int    freq = 2;
		int    octaves = 0; do { octaves++; } while((1 << octaves) < (res));

		for(int level = 0; level < octaves; level++)
		{
			if(level >= in_noise_properties->low_freq_skip &&
				level <= octaves - in_noise_properties->high_freq_skip)
			{
				data = StupidNoise1D(res, freq, data, scale);
			}
			else if(level <= octaves - in_noise_properties->high_freq_skip)
			{
				for(int i = 0; i < freq; i++) rand();
			}

			freq *= 2;
			scale *= in_noise_properties->roughness;
		}

		if(data)
		{
			float min_v = data[0];
			float max_v = data[0];
			int i;

			for(i = 1; i < res; i++)
			{
				if(min_v > data[i]) min_v = data[i];
				if(max_v < data[i]) max_v = data[i];
			}

			for(i = 0; i < res; i++)
			{
				data[i] = (data[i] - min_v) / (max_v - min_v);
			}

			if(in_noise_properties->marbling != 0.0f)
			{
				for(i = 0; i < res; i++)
				{
					data[i] = sinf(PI2 * data[i] * in_noise_properties->marbling) * 0.5f + 0.5f;
				}
			}
		}
		else
		{
			data = (float*)calloc(sizeof(float), res);
			if(!data)
			{
				Logger::Err("Out of memory");
				throw std::runtime_error("Out of memory");
			}
		}

		return data;
	}

	float* PerlinNoise2D(int res, const noise_properties* in_noise_properties)
	{
		float* data = nullptr;
		float  scale = 1.0f;
		int    freq = 2;
		int    octaves = 0; do { octaves++; } while((1 << octaves) < (res));

		for(int level = 0; level < octaves; level++)
		{
			if(level >= in_noise_properties->low_freq_skip &&
				level <= octaves - in_noise_properties->high_freq_skip)
			{
				data = StupidNoise2D(res, freq, data, scale);
			}
			else if(level <= octaves - in_noise_properties->high_freq_skip)
			{
				for(int i = 0; i < freq * freq; i++) rand();
			}

			freq *= 2;
			scale *= in_noise_properties->roughness;
		}

		if(data)
		{
			if(in_noise_properties->turbulence != 0.0f)
			{
				noise_properties prop = *in_noise_properties;

				int turbulence_res = 8 << in_noise_properties->turbulence_res;
				float turbulence_exp = powf(2.0f, in_noise_properties->turbulence_expshift);

				prop.turbulence = 0.0f;
				prop.roughness = in_noise_properties->turbulence_roughness;
				prop.low_freq_skip = in_noise_properties->turbulence_low_freq_skip;
				prop.high_freq_skip = in_noise_properties->turbulence_high_freq_skip;
				prop.marbling = in_noise_properties->turbulence_marbling;

				srand(in_noise_properties->seed + 1);
				float* dx = PerlinNoise2D(turbulence_res, &prop);
				srand(in_noise_properties->seed + 2);
				float* dy = PerlinNoise2D(turbulence_res, &prop);
				float* temp = new float[res * res];
				memcpy(temp, data, res * res * sizeof(float));

				for(int j = 0; j < res; j++)
					for(int i = 0; i < res; i++)
					{
						float x = Sample2D(dx, turbulence_res, turbulence_res, (float)i / (float)res, (float)j / (float)res) * 2.0f - 1.0f;
						float y = Sample2D(dy, turbulence_res, turbulence_res, (float)i / (float)res, (float)j / (float)res) * 2.0f - 1.0f;

						if(turbulence_exp != 1.0f)
						{
							x = powf(x > 0.0f ? x : -x, turbulence_exp) * (x > 0.0f ? 1.0f : -1.0f);
							y = powf(y > 0.0f ? y : -y, turbulence_exp) * (y > 0.0f ? 1.0f : -1.0f);
						}

						x += in_noise_properties->turbulence_offset_x;
						y += in_noise_properties->turbulence_offset_y;

						x = x * in_noise_properties->turbulence / 64.0f + (float)i / (float)res;
						y = y * in_noise_properties->turbulence / 64.0f + (float)j / (float)res;

						data[i + j * res] = Sample2D(temp, res, res, x, y);
					}

				delete[] temp;
				free(dx);
				free(dy);
			}

			float min_v = data[0];
			float max_v = data[0];
			int i;

			for(i = 1; i < res * res; i++)
			{
				if(min_v > data[i]) min_v = data[i];
				if(max_v < data[i]) max_v = data[i];
			}

			for(i = 0; i < res * res; i++)
			{
				data[i] = (data[i] - min_v) / (max_v - min_v);
			}

			if(in_noise_properties->marbling != 0.0f)
			{
				for(i = 0; i < res * res; i++)
				{
					data[i] = sinf(PI2 * data[i] * in_noise_properties->marbling) * 0.5f + 0.5f;
				}
			}
		}
		else
		{
			data = (float*)calloc(sizeof(float), res * res);
			if(!data)
			{
				Logger::Err("Out of memory");
				throw std::runtime_error("Out of memory");
			}
		}

		return data;
	}

	float* PerlinNoise3D(int res, const noise_properties* in_noise_properties)
	{
		float* data = nullptr;
		float  scale = 1.0f;
		int    freq = 1;
		int    octaves = 0;
		do
		{
			octaves++;
		} while((1 << octaves) < (res));

		for(int level = 0; level < octaves; level++)
		{
			if(level >= in_noise_properties->low_freq_skip && level <= octaves - in_noise_properties->high_freq_skip)
			{
				data = StupidNoise3D(res, freq, data, scale);

				fprintf(stdout, "octave %d computed.\n", level + 1);
				fflush(stdout);
			}

			freq *= 2;
			scale *= in_noise_properties->roughness;
		}

		if(data)
		{
			float min_v = data[0];
			float max_v = data[0];
			int i;

			for(i = 1; i < res * res * res; i++)
			{
				if(min_v > data[i]) min_v = data[i];
				if(max_v < data[i]) max_v = data[i];
			}

			for(i = 0; i < res * res * res; i++)
			{
				data[i] = (data[i] - min_v) / (max_v - min_v);
			}

			if(in_noise_properties->marbling != 0.0f)
			{
				for(i = 0; i < res * res * res; i++)
				{
					data[i] = sinf(PI2 * data[i] * in_noise_properties->marbling) * 0.5f + 0.5f;
				}
			}
		}
		else
		{
			data = (float*)calloc(sizeof(float), res * res * res);
			if(!data)
			{
				Logger::Err("Out of memory");
				throw std::runtime_error("Out of memory");
			}
		}

		return data;
	}

	float* SphereNoise(const float* input_noise, int input_res, float* output_noise, int output_res, int face_index)
	{
		if(!output_noise)
		{
			output_noise = (float*)calloc(sizeof(float), output_res * output_res);
			if(!output_noise)
			{
				Logger::Err("Out of memory");
				throw std::runtime_error("Out of memory");
			}
		}

		for(int t = 0; t < output_res; t++)
			for(int s = 0; s < output_res; s++)
			{
				float xf = (float)(2 * s - output_res + 1) / (float)output_res;
				float yf = (float)(2 * t - output_res + 1) / (float)output_res;
				vector3 v1;

				switch(face_index)
				{
				case 0: v1 = vector3(+1.0f, +yf, -xf); break; // px
				case 1: v1 = vector3(+xf, +1.0f, -yf); break; // py
				case 2: v1 = vector3(-1.0f, +yf, +xf); break; // nx
				case 3: v1 = vector3(+xf, -1.0f, +yf); break; // ny
				case 4: v1 = vector3(+xf, +yf, +1.0f); break; // pz
				case 5: v1 = vector3(-xf, +yf, -1.0f); break; // nz
				}

				v1 *= 0.5f / sqrtf(v1 * v1);
				v1 += vector3(0.5f, 0.5f, 0.5f);
				v1 *= (float)input_res;

				int xi = (int)floorf(v1.x) - 1;
				int yi = (int)floorf(v1.y) - 1;
				int zi = (int)floorf(v1.z) - 1;

				v1.x -= floorf(v1.x);
				v1.y -= floorf(v1.y);
				v1.z -= floorf(v1.z);

				float tmp[64];

				for(int z = 0; z < 4; z++)
					for(int y = 0; y < 4; y++)
						for(int x = 0; x < 4; x++)
						{
							tmp[x + y * 4 + z * 16] = input_noise[CalcIndex3D(x + xi, y + yi, z + zi, input_res)];
						}

				// really stupid - sample 4x4x4 voxels just to do trilinear interpolation
				output_noise[s + t * output_res] = Interpolate3D_Linear(tmp, v1.x, v1.y, v1.z);
			}

		return output_noise;
	}
}