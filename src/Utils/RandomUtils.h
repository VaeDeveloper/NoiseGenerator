#pragma once
#include <random>

class RandomUtil 
{
public:
    RandomUtil(unsigned int seed)
        : rng(seed), dist(0.0f, 1.0f) {}

    float NextFloat() 
    {
        return dist(rng);
    }

    void Reseed(unsigned int seed) 
    {
        rng.seed(seed);
    }

private:
    std::mt19937 rng;
    std::uniform_real_distribution<float> dist;
};