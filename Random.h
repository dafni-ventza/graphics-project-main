#pragma once

#include <random>

class Random {
	//using this for generating any random numbers desired
	//Random::Float() * 100.0f; //generatate a random number from 0-100;
public:
	static void Init() {
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float() {
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};
