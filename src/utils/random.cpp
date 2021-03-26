#include "random.h"

#include <random>

std::mt19937 randEngine;

namespace rnd {



	/** Return a random int from 1 to n. */
	int dice(int n) {
		std::uniform_int_distribution<int> d{ 1, n };
		return d(randEngine);
	}

	/** Return a random float between 0 and n. */
	float rand(float n) {
		std::uniform_real_distribution<float> f(0.0f,n);
		return f(randEngine);
	}

	/** Return a random float between r1 and r2. */
	float rand(float r1, float r2)	{
		std::uniform_real_distribution<float> f(r1, r2);
		return f(randEngine);
	}


}