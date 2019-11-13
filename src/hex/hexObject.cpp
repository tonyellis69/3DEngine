#include "hexObject.h"

#include <glm/gtc/matrix_transform.hpp>	

/** Set position using hex cube coordinates. */
void CHexObject::setPosition(int x, int y, int z) {
	hexPosition.x = x;
	hexPosition.y = y;
	hexPosition.z = z;

	glm::i32vec2 axial = hexPosition.getAxial();
	glm::vec3 translation(0);
	translation.x = sqrt(3) * axial.x + sqrt(3) / 2.0f * axial.y;
	translation.y = -3.0f / 2.0f * axial.y;

	worldMatrix =  glm::translate(glm::mat4(1), translation);
}

void CHexObject::move(THexDir direction) {
	static glm::i32vec3 moveVector[]{ {1,0,-1}, {1,-1,0}, {0,-1,1}, {-1,0,1}, {-1,1,0}, {0,1,-1} };

	glm::i32vec3 newPos = glm::i32vec3(hexPosition.x, hexPosition.y, hexPosition.z)
		+ moveVector[direction];
	setPosition(newPos.x, newPos.y, newPos.z);
}
