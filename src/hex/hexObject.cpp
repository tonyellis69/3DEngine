#include "hexObject.h"

#include <glm/gtc/matrix_transform.hpp>	


CHexObject::CHexObject() {
	zHeight = 0;
	moving = false;
}

/** Set position using hex cube coordinates. */
void CHexObject::setPosition(int x, int y, int z) {
	hexPosition.x = x;
	hexPosition.y = y;
	hexPosition.z = z;
	destination = hexPosition;

	glm::i32vec2 axial = hexPosition.getAxial();
	glm::vec3 translation(0);
	translation.x = sqrt(3) * axial.x + sqrt(3) / 2.0f * axial.y;
	translation.y = -3.0f / 2.0f * axial.y;

	//translation = cubeToWorldSpace(destination);

	translation.z = zHeight;

	worldMatrix =  glm::translate(glm::mat4(1), translation);
}

void CHexObject::move(THexDir direction) {

	glm::i32vec3 newPos = glm::i32vec3(hexPosition.x, hexPosition.y, hexPosition.z)
		+ moveVectorCube[direction];
	setPosition(newPos.x, newPos.y, newPos.z);
}

/**	Order this object to move to the neighbouring hex. */
void CHexObject::moveOrder() {
	if (travelPath.empty()) 
		return;
	CHex& nextHex = travelPath[0];

	moving = true;
	destination = travelPath[0];
	worldSpaceDestination = cubeToWorldSpace(destination);
	CHex moveVectorHex = destination - hexPosition;
	moveVector = glm::normalize(worldSpaceDestination - cubeToWorldSpace(hexPosition));
}

/** Set the height at which this object is drawn. */
void CHexObject::setZheight(float height) {
	zHeight = height;
	worldMatrix = glm::translate(worldMatrix, glm::vec3(0, 0, height));
}

/** Load a sequence of hexes to travel down. */
void CHexObject::setTravelPath(THexList& path) {
	travelPath = path;
}

THexList& CHexObject::getTravelPath() {
	return travelPath;
}

/** Carry out whatever changes have arisen since last update. Return true if this is ongoing. */
bool CHexObject::update(float dT) {
	if (moving) {
		float speed = 0.01f;
		glm::vec3 velocity = moveVector * speed;
		worldMatrix = glm::translate(worldMatrix, velocity);
		glm::vec3 currentPos = glm::vec3(worldMatrix[3]);
		if (glm::distance(currentPos, worldSpaceDestination) < 0.1f) {
			moving = false;
			setPosition(destination.x,destination.y,destination.z);
			travelPath.erase(travelPath.begin());
			return false;
		}
		return true;
	}
	return false;
}


