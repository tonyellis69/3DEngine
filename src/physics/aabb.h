#pragma one

#include "glm\glm.hpp"

/** An axis-aligned bounding box. */
class CAABB {
public:
	void setSize(float width, float height);
	void setPos(glm::vec3& pos);

	float halfWidth;
	float halfHeight;
	glm::vec3 centrePos;
	glm::vec3 corner[8]; ///<AB CD on top (back to front), EF GH below,

};