#pragma once

#include "glm/glm.hpp"

#include "hex.h"

/** A class describing the element of a hex array. */
class CHexElement {
public:
	CHexElement() : content(0) { };
	

	int content;
	glm::vec3 position; 


};