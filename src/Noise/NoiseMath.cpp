#include "NoiseMath.h"
#include <cmath>

namespace NoiseGenerator
{
	int CalcIndex1D(int x, int res)
	{
		x %= res; if(x < 0) x += res;

		return x;
	}

	int CalcIndex2D(int x, int y, int res)
	{
		x %= res; if(x < 0) x += res;
		y %= res; if(y < 0) y += res;

		return x + y * res;
	}

	int CalcIndex3D(int x, int y, int z, int res)
	{
		x %= res; if(x < 0) x += res;
		y %= res; if(y < 0) y += res;
		z %= res; if(z < 0) z += res;

		return x + y * res + z * res * res;
	}

	float Interpolate1D(const float* data, float xf)
	{
		const float x2 = xf * xf;
		const float x3 = xf * x2;

		return
			(
				data[0] * (1.0f - 3.0f * xf + 3.0f * x2 - 1.0f * x3) +
				data[1] * (4.0f - 6.0f * x2 + 3.0f * x3) +
				data[2] * (1.0f + 3.0f * xf + 3.0f * x2 - 3.0f * x3) +
				data[3] * (1.0f * x3)
				) / 6.0f;

		// WARNING: result may be out of range
		/*
		float p= (data[3] - data[2]) - (data[0] - data[1]);
		float q= (data[0] - data[1]) - p;
		float r= (data[2] - data[0]);
		float s= (data[1]);

		// d0*(-x^3 + 2*x^2 - x) +
		// d1*( x^3 - 2*x^2 + 1) +
		// d2*(-x^3 +   x^2 + x) +
		// d3*( x^3 -   x^2    )

		return s + xf*(r + xf*(q + xf*p));
		*/
	}

	float Interpolate2D(const float* data, float xf, float yf)
	{
		float c[4];

		for(int i = 0; i < 4; i++)
		{
			c[i] = NoiseGenerator::Interpolate1D(data + i * 4, xf);
		}

		return NoiseGenerator::Interpolate1D(c, yf);
	}

	float Interpolate3D(const float* data, float xf, float yf, float zf)
	{
		float c[4];

		for(int i = 0; i < 4; i++)
		{
			c[i] = NoiseGenerator::Interpolate2D(data + i * 16, xf, yf);
		}

		return NoiseGenerator::Interpolate1D(c, zf);
	}

	float Interpolate1D_Linear(const float* data, float xf)
	{
		return data[1] + xf * (data[2] - data[1]);
	}

	float Interpolate2D_Linear(const float* data, float xf, float yf)
	{
		float c[4];

		for(int i = 0; i < 4; i++)
		{
			c[i] = NoiseGenerator::Interpolate1D_Linear(data + i * 4, xf);
		}

		return NoiseGenerator::Interpolate1D_Linear(c, yf);
	}

	float Interpolate3D_Linear(const float* data, float xf, float yf, float zf)
	{
		float c[4];

		for(int i = 0; i < 4; i++)
		{
			c[i] = NoiseGenerator::Interpolate2D_Linear(data + i * 16, xf, yf);
		}

		return NoiseGenerator::Interpolate1D_Linear(c, zf);
	}

	float Sample2D(const float* data, short width, short height, float x, float y)
	{
		short xi = static_cast<short>(floorf(x * static_cast<float>(width)));
		short yi = static_cast<short>(floorf(y * static_cast<float>(height)));
		float xf = x * width - floorf(x * width);
		float yf = y * height - floorf(y * height);

		float d1 = data[((xi + 0 + width) % width) + ((yi + 0 + height) % height) * width] * (1.0f - xf) +
			data[((xi + 1 + width) % width) + ((yi + 0 + height) % height) * width] * xf;
		float d2 = data[((xi + 0 + width) % width) + ((yi + 1 + height) % height) * width] * (1.0f - xf) +
			data[((xi + 1 + width) % width) + ((yi + 1 + height) % height) * width] * xf;

		return d1 * (1.0f - yf) + d2 * yf;
	}
}