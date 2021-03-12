#include "random.h"

#include <random>

std::mt19937 randEngine;

namespace rnd {



	/** Return a random int from 1 to n. */
	int dice(int n) {
		std::uniform_int_distribution<int> d{ 1, n };
		return d(randEngine);
	}


}