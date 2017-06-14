#pragma once

#include "glm\glm.hpp"

/** Class for bounding sphere collision checks. */
class CBsphere {
public:
	CBsphere() {};
	void setRadius(float r);
	void setCentre(glm::vec3 c);

	float radius;
	glm::vec3 centre;
};