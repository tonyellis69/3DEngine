#include "hexObject.h"

#include <cmath>

#include "..\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	


CHexObject::CHexObject() {
	zHeight = 0;
	moving = false;
	isRobot = false;
	worldPos = glm::vec3(0);
	setZheight(0.05f);

	facing = hexEast;
	rotation = 0.0f;
	rotationalVelocity = 0.0f;
	turning = false;
	buildWorldMatrix();
}

void CHexObject::setCallbackObj(IhexObjectCallback* obj) {
	callbackObj = obj;
}

/** Set position using individual hex cube coordinates. */
void CHexObject::setPosition(int x, int y, int z) {
	hexPosition.x = x;
	hexPosition.y = y;
	hexPosition.z = z;
	destination = hexPosition;

	glm::i32vec2 axial = hexPosition.getAxial();
	
	worldPos.x = sqrt(3) * axial.x + sqrt(3) / 2.0f * axial.y;
	worldPos.y = -3.0f / 2.0f * axial.y;
	worldPos.z = zHeight;

	buildWorldMatrix();
}

/** Set position using offset coordinates. */
void CHexObject::setPosition(int x, int y) {
	CHex pos = offsetToCube(x, y);
	setPosition(pos.x, pos.y, pos.z);
}

/** Set position using hex cube coordinates. */
void CHexObject::setPosition(CHex& hex) {
	setPosition(hex.x, hex.y, hex.z);
}



/** Set the rotation and facing direction of this object. */
void CHexObject::setDirection(THexDir direction) {
	facing = direction;
	rotation = dirToAngle(direction);
	buildWorldMatrix();
}



/**	Order this object to move to the next hex on its current travel path. */
void CHexObject::newMove() {
	if (travelPath.empty()) {
		return;
	}
	CHex& nextHex = travelPath[0];
	moving = true;
	destination = travelPath[0];
	worldSpaceDestination = cubeToWorldSpace(destination);
	CHex moveVectorHex = destination - hexPosition;
	moveVector = glm::normalize(worldSpaceDestination - cubeToWorldSpace(hexPosition));



	destinationDirection =  neighbourDirection(hexPosition, destination);
	float rotationDir = shortestRotation(facing, destinationDirection);
	if (rotationDir == 0.0f)
		return;

	turning = true;
	rotationalVelocity = (rotationDir > 0) - (rotationDir < 0) ;
	destinationAngle = dirToAngle(destinationDirection);
}

/** Set the height at which this object is drawn. */
void CHexObject::setZheight(float height) {
	zHeight = height;
	worldPos.z = height;
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

		if (turning) {
			rotation += rotationalVelocity * 0.01f;
			rotation = glm::mod<float>(rotation, 2 * M_PI);
			if (abs(rotation - destinationAngle) < 0.1f) {
				turning = false;
				setDirection(destinationDirection);
				return true; //because moving itself not yet resolved
			}
			buildWorldMatrix();
			return true;
		}


		float speed = 0.0035f;
		glm::vec3 velocity = moveVector * speed;
		worldPos += velocity;
		
		if (glm::distance(worldPos, worldSpaceDestination) < 0.1f) {
			moving = false;
			setPosition(destination.x,destination.y,destination.z);
			if (!travelPath.empty() && destination == travelPath[0])
				travelPath.erase(travelPath.begin());
			return false;
		}
		buildWorldMatrix();
		return true;
	}
	return false;
}


void CHexObject::findTravelPath(CHex& target) {
	//ordinarily, just find the path from where we are now.
	//but if we've moving, makes sense that the new path will start
	//where we end up
	if (moving)
		travelPath = callbackObj->getPathCB(destination, target);
	else
		travelPath = callbackObj->getPathCB(hexPosition,target);

}

/** Construct this object's world matrix from its known position and rotation.*/
void CHexObject::buildWorldMatrix() {
	worldMatrix = glm::translate(glm::mat4(1), worldPos);
	worldMatrix = glm::rotate(worldMatrix, rotation, glm::vec3(0, 0, -1));
}




