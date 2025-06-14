#pragma once

namespace NG
{

	float Interpolate1D(const float* data, float xf);
	float Interpolate2D(const float* data, float xf, float yf);
	float Interpolate3D(const float* data, float xf, float yf, float zf);

	float Interpolate1D_Linear(const float* data, float xf);
	float Interpolate2D_Linear(const float* data, float xf, float yf);
	float Interpolate3D_Linear(const float* data, float xf, float yf, float zf);


	int CalcIndex1D(int x, int res);
	int CalcIndex2D(int x, int y, int res);
	int CalcIndex3D(int x, int y, int z, int res);

	float Sample2D(const float* data, short width, short height, float x, float y);
}
