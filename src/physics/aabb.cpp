#include "aabb.h"

void CAABB::setSize(float width, float height) {
	halfWidth = width / 2;
	halfHeight = height / 2;
}

void CAABB::setPos(glm::vec3 & pos) {
	centrePos = pos;
	corner[0] = centrePos + glm::vec3(-halfWidth, halfHeight, -halfWidth); //A
	corner[1] = centrePos + glm::vec3(halfWidth, halfHeight, -halfWidth); //B
	corner[2] = centrePos + glm::vec3(-halfWidth, halfHeight, halfWidth); //C
	corner[3] = centrePos + glm::vec3(halfWidth, halfHeight, halfWidth); //D

	corner[4] = centrePos + glm::vec3(-halfWidth, -halfHeight, -halfWidth); //E
	corner[5] = centrePos + glm::vec3(halfWidth, -halfHeight, -halfWidth); //F
	corner[6] = centrePos + glm::vec3(-halfWidth, -halfHeight, halfWidth); //G
	corner[7] = centrePos + glm::vec3(halfWidth, -halfHeight, halfWidth); //H
}
