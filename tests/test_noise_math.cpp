#include <gtest/gtest.h>
#include "Noise/NoiseMath.h"
#include <algorithm>

// Distance test
TEST(NoiseMathTest, ZeroDistance)
{
	EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 0.0f, 0.0f), 0.0f);
}

TEST(NoiseMathTest, HorizontalDistance)
{
	EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 3.0f, 0.0f), 3.0f);
}

TEST(NoiseMathTest, VerticalDistance)
{
	EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 0.0f, 4.0f), 4.0f);
}

TEST(NoiseMathTest, DiagonalDistance)
{
	EXPECT_FLOAT_EQ(NG::Distance(0.0f, 0.0f, 3.0f, 4.0f), 5.0f); // 3-4-5 треугольник
}

TEST(NoiseMathTest, NegativeCoordinates)
{
	EXPECT_FLOAT_EQ(NG::Distance(-1.0f, -1.0f, 2.0f, 3.0f), 5.0f);
}


// CalcIndex1D_Test
TEST(NoiseMathTest, CalcIndex1D_Basic)
{
	EXPECT_EQ(NG::CalcIndex1D(2, 5), 2);
	EXPECT_EQ(NG::CalcIndex1D(5, 5), 0);
	EXPECT_EQ(NG::CalcIndex1D(-1, 5), 4);
	EXPECT_EQ(NG::CalcIndex1D(-6, 5), 4);
}

TEST(NoiseMathTest, CalcIndex2D_Basic)
{
	EXPECT_EQ(NG::CalcIndex2D(1, 2, 4), 1 + 2 * 4);
	EXPECT_EQ(NG::CalcIndex2D(-1, 2, 4), 3 + 2 * 4);
	EXPECT_EQ(NG::CalcIndex2D(1, -1, 4), 1 + 3 * 4);
}

TEST(NoiseMathTest, CalcIndex3D_Basic)
{
	EXPECT_EQ(NG::CalcIndex3D(1, 2, 3, 4), 1 + 2 * 4 + 3 * 4 * 4);
	EXPECT_EQ(NG::CalcIndex3D(-1, 0, 0, 4), 3);
	EXPECT_EQ(NG::CalcIndex3D(0, -1, 0, 4), 3 * 4);
	EXPECT_EQ(NG::CalcIndex3D(0, 0, -1, 4), 3 * 16);
}

TEST(NoiseMathTest, Interpolate1D_Linear)
{
	float data[4] = { 0.0f, 1.0f, 2.0f, 3.0f };

	EXPECT_FLOAT_EQ(NG::Interpolate1D_Linear(data, 0.0f), 1.0f);
	EXPECT_FLOAT_EQ(NG::Interpolate1D_Linear(data, 0.5f), 1.5f);
	EXPECT_FLOAT_EQ(NG::Interpolate1D_Linear(data, 1.0f), 2.0f);
}

TEST(NoiseMathTest, Interpolate1D_Cubic)
{
	float data[4] = { 0.0f, 1.0f, 2.0f, 3.0f };

	float result = NG::Interpolate1D(data, 0.5f);
	EXPECT_NEAR(result, 1.5f, 0.01f);
}

TEST(NoiseMathTest, Interpolate2D_Linear)
{
	float data[16];
	for(int i = 0; i < 16; ++i) data[i] = static_cast<float>(i);

	float result = NG::Interpolate2D_Linear(data, 0.5f, 0.5f);
	EXPECT_NEAR(result, 7.5f, 0.01f);
}

TEST(NoiseMathTest, Interpolate3D_Linear)
{
	float data[64];
	for(int i = 0; i < 64; ++i) data[i] = static_cast<float>(i);

	float result = NG::Interpolate3D_Linear(data, 0.5f, 0.5f, 0.5f);
	EXPECT_NEAR(result, 31.5f, 0.1f);
}

TEST(NoiseMathTest, Sample2D_Basic)
{
	float data[4 * 4] = 
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		8, 9,10,11,
	   12,13,14,15
	};

	float result = NG::Sample2D(data, 4, 4, 0.0f, 0.0f);
	EXPECT_NEAR(result, 0.0f, 0.01f);

	result = NG::Sample2D(data, 4, 4, 0.5f, 0.5f); 
	EXPECT_NEAR(result, 10.0f, 0.5f); 
}

TEST(NoiseMathTest, Sample2D_EdgeClamp)
{
	float data[4 * 4];
	for(int i = 0; i < 16; ++i) data[i] = static_cast<float>(i);

	float result = NG::Sample2D(data, 4, 4, 1.0f, 1.0f); 
	EXPECT_LE(result, 15.0f);
}

TEST(NoiseMathTest, Interpolate1D_Cubic_Bounds)
{
	float data[4] = { 0.0f, 1.0f, 2.0f, 3.0f };

	EXPECT_NEAR(NG::Interpolate1D(data, 0.0f), 1.0f, 0.01f);
	EXPECT_NEAR(NG::Interpolate1D(data, 1.0f), 2.0f, 0.01f);
}

TEST(NoiseMathTest, Sample2D_NullData)
{
	EXPECT_FLOAT_EQ(NG::Sample2D(nullptr, 4, 4, 0.5f, 0.5f), 0.0f);
}

TEST(NoiseMathTest, Sample2D_InvalidSize)
{
	float dummy[1] = { 0.0f };
	EXPECT_FLOAT_EQ(NG::Sample2D(dummy, 0, 4, 0.5f, 0.5f), 0.0f);
	EXPECT_FLOAT_EQ(NG::Sample2D(dummy, 4, 0, 0.5f, 0.5f), 0.0f);
}

TEST(NoiseMathTest, Interpolation_OutOfRange)
{
	float data[4] = { 0.0f, 1.0f, 2.0f, 3.0f };

	// xf < 0 and xf > 1 are not clamped; function assumes xf in [0,1]
	EXPECT_NO_FATAL_FAILURE(NG::Interpolate1D(data, -0.5f));
	EXPECT_NO_FATAL_FAILURE(NG::Interpolate1D(data, 1.5f));
}

TEST(NoiseMathTest, Sample2D_NearEdge)
{
	float data[4 * 4];
	for(int i = 0; i < 16; ++i) data[i] = static_cast<float>(i);

	EXPECT_NO_FATAL_FAILURE(NG::Sample2D(data, 4, 4, -0.1f, 0.5f)); 
	EXPECT_NO_FATAL_FAILURE(NG::Sample2D(data, 4, 4, 0.5f, -0.1f)); 
}

TEST(NoiseMathTest, Interpolate2D_Cubic)
{
	float data[16];
	for(int i = 0; i < 16; ++i) data[i] = static_cast<float>(i);

	float result = NG::Interpolate2D(data, 0.5f, 0.5f);
	EXPECT_NEAR(result, 7.5f, 1.0f); 
}

TEST(NoiseMathTest, Interpolate3D_Cubic)
{
	float data[64];
	for(int i = 0; i < 64; ++i) data[i] = static_cast<float>(i);

	float result = NG::Interpolate3D(data, 0.5f, 0.5f, 0.5f);
	EXPECT_NEAR(result, 31.5f, 2.0f);
}

TEST(PerfTest, Interpolate3D_Speed)
{
	float data[64];
	for(int i = 0; i < 64; ++i) data[i] = static_cast<float>(i);

	for(int i = 0; i < 1000000; ++i)
	{
		float res = NG::Interpolate3D(data, 0.3f, 0.4f, 0.5f);
		(void)res;
	}
}