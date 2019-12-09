#include "hexObject.h"

#include "..\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	


CHexObject::CHexObject() {
	zHeight = 0;
	moving = false;
	isRobot = false;
	setZheight(0.05f);
}

void CHexObject::setCallbackObj(IhexObjectCallback* obj) {
	callbackObj = obj;
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
	translation.z = zHeight;

	worldMatrix =  glm::translate(glm::mat4(1), translation);
}

/** Set position using offset coordinates. */
void CHexObject::setPosition(int x, int y) {
	CHex pos = offsetToCube(x, y);
	setPosition(pos.x, pos.y, pos.z);
}

void CHexObject::move(THexDir direction) {
	glm::i32vec3 newPos = glm::i32vec3(hexPosition.x, hexPosition.y, hexPosition.z)
		+ moveVectorCube[direction];
	setPosition(newPos.x, newPos.y, newPos.z);
}

/**	Order this object to move to the next hex on its current travel path. */
void CHexObject::startTravel() {
	if (travelPath.empty()) {
		return;
	}
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
			if (badHex(destination))
				int b = 0;
			setPosition(destination.x,destination.y,destination.z);
			if (destination == travelPath[0])
				travelPath.erase(travelPath.begin());
			return false;
		}
		return true;
	}
	return false;
}


void CHexObject::findTravelPath(CHex& target) {
	//ordinarily, just find the path from where we are now.
	//but if we've moving, makes sense that the new path will start
	//where we end up
	if (moving)
		travelPath = callbackObj->getPathCallback(destination, target);
	else
		travelPath = callbackObj->getPathCallback(hexPosition,target);

}




