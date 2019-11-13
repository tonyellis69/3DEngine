#pragma once

#include <glm/glm.hpp>

/** A class describing a hex position. */
class CHex {
public:
	
	glm::i32vec2 getAxial();

	//cube coordinates:
	int x;
	int y;
	int z;
};