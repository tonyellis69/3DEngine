#pragma once

#include "glm/glm.hpp"


/** A class describing a hex object. */
class CHex {
public:
	CHex() : content(0) {};
	

	int content;
	glm::vec3 position; 
};