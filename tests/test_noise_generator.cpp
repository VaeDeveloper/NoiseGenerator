#include <gtest/gtest.h>
#include <functional>
#include "Noise/NoiseGenerator.h" 

using namespace NG;

TEST(StupidNoiseTest, StupidNoise2D_BasicOutput)
{
	const int res = 16;
	const int freq = 4;
	float* result = StupidNoise2D(res, freq, nullptr, 1.0f, 12345);
	ASSERT_NE(result, nullptr);

	for(int i = 0; i < res * res; ++i)
	{
		EXPECT_GE(result[i], 0.0f);
		EXPECT_LE(result[i], 1.0f);
	}

	free(result);
}

TEST(FBMNoiseTest, BasicGeneration)
{
	const int res = 32;
	NoiseProperties props{};
	props.seed = 42;
	props.roughness = 0.5f;
	props.low_freq_skip = 0;
	props.high_freq_skip = 0;
	props.turbulence = 0.0f;
	props.marbling = 0.0f;

	float* result = FBMNoise2D(res, &props, nullptr);
	ASSERT_NE(result, nullptr);

	for(int i = 0; i < res * res; ++i)
	{
		EXPECT_GE(result[i], 0.0f);
		EXPECT_LE(result[i], 1.0f);
	}

	free(result);
}

TEST(FBMNoiseTest, CancelByProgress)
{
	const int res = 32;
	NoiseProperties props{};
	props.seed = 42;
	props.roughness = 0.5f;
	props.low_freq_skip = 0;
	props.high_freq_skip = 0;
	props.turbulence = 0.0f;
	props.marbling = 0.0f;

	bool called = false;
	auto cancelAtStart = [&] (float progress) -> bool {
		called = true;
		return false;
		};

	float* result = FBMNoise2D(res, &props, cancelAtStart);
	EXPECT_EQ(result, nullptr);
	EXPECT_TRUE(called);
}

TEST(WorleyNoiseTest, GeneratesSuccessfully)
{
	const int res = 32;
	NoiseProperties props{};
	props.seed = 7;
	props.low_freq_skip = 0;
	props.high_freq_skip = 0;
	props.turbulence = 0.0f;
	props.marbling = 0.0f;

	float* data = WorleyNoise2D(res, &props, nullptr);
	ASSERT_NE(data, nullptr);

	for(int i = 0; i < res * res; ++i)
	{
		EXPECT_GE(data[i], 0.0f);
		EXPECT_LE(data[i], 1.0f);
	}

	free(data);
}

TEST(StupidNoiseTest, ZeroFrequencyThrows)
{
	EXPECT_THROW(
		{
		StupidNoise2D(16, 0, nullptr, 1.0f, 123);
		}, std::invalid_argument);
}