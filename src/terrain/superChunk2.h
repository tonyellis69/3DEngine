#pragma once

#include "glm/glm.hpp"

/** A container for zero or more chunks occupying a cubic volume of space. */
class CSuperChunk2 {
public:
	CSuperChunk2() {};

	glm::vec4 colour; //TO DO probably temp
	glm::i32vec3 origIndex; //TO DO probably temp

};