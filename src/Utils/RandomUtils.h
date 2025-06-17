#pragma once
#include <random>

/**
 * RandomGenerator provides a simple wrapper around std::mt19937
 * to generate deterministic or non-deterministic random numbers.
 * Designed for use in procedural systems (e.g., noise generation).
 */
class RandomGenerator
{
public:
	/** Constructs the generator with an optional seed (random_device by default) */
	explicit RandomGenerator(unsigned int seed = std::random_device{}())
		: rng(seed) {
	}

	/** Reseeds the generator with a new seed */
	void Reseed(unsigned int seed)
	{
		rng.seed(seed);
	}

	/** Returns a random float in the range [min, max) */
	float NextFloat(float min = 0.0f, float max = 1.0f)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(rng);
	}

	/** Returns a random integer in the range [min, max] (inclusive) */
	int NextInt(int min, int max = 1)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}

	/** Returns a random boolean value (true/false with 50% probability) */
	bool NextBool()
	{
		std::bernoulli_distribution dist(0.5);
		return dist(rng);
	}

private:
	/** Mersenne Twister engine used for random number generation */
	std::mt19937 rng;
};