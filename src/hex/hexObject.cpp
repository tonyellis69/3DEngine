#include "hexObject.h"

#include <cmath>
#include <algorithm>

#include "..\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	


CHexObject::CHexObject() {
	zHeight = 0;
	moving = false;
	isRobot = false;
	worldPos = glm::vec3(0);
	setZheight(0.05f);
	action = actNone;
	facing = hexEast;
	rotation = 0.0f;
	rotationalVelocity = 0.0f;
	turning = false;
	buildWorldMatrix();
}

void CHexObject::setCallbackObj(IhexObjectCallback* obj) {
	hexWorld = obj;
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
bool CHexObject::beginMove() {
	if (isRobot)
		int b = 0;

	if (travelPath.empty())
		return false;


	CHex& nextHex = travelPath[0];
	moving = true;
	destination = travelPath[0];
	worldSpaceDestination = cubeToWorldSpace(destination);
	CHex moveVectorHex = destination - hexPosition;
	moveVector = glm::normalize(worldSpaceDestination - cubeToWorldSpace(hexPosition));
	

	//rotation
	destinationDirection =  neighbourDirection(hexPosition, destination);
	float rotationDir = shortestRotation(facing, destinationDirection);
	if (rotationDir == 0.0f)
		return true;
	turning = true;
	rotationalVelocity = (rotationDir > 0) - (rotationDir < 0) ;
	destinationAngle = dirToAngle(destinationDirection);
	return true;
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
//	if (action == actNone)
	//	return false;


	if (moving) {

		if (turning) {
			rotation += rotationalVelocity * 10.0f * dT;
			rotation = glm::mod<float>(rotation, 2 * M_PI);			
			float gap = std::fmod(rotation - destinationAngle + M_PI, 2 * M_PI) - M_PI;
			if (abs(gap) < 0.1f) {
				turning = false;
				setDirection(destinationDirection);
				return true; //because moving itself not yet resolved
			}
			buildWorldMatrix();
			return true;
		}


		float speed = 7.0f;
		glm::vec3 velocity = moveVector * speed * dT;
		if (glm::distance(worldPos, worldSpaceDestination) < 0.15f) {
			if (isRobot)
				sysLog << "\nRobot reached destination!";
			moving = false;
			velocity = glm::vec3(0);
			setPosition(destination.x,destination.y,destination.z);
			if (!travelPath.empty() && destination == travelPath[0])
				travelPath.erase(travelPath.begin());
			return false;
		}
		else {
			if (isRobot) {
				sysLog << "\nbot worldPos: " << worldPos << " destination " << worldSpaceDestination << " dist "
					<< glm::distance(worldPos, worldSpaceDestination);
				sysLog << "\nbot velocity " << velocity;
			}
			worldPos += velocity;
		}

		buildWorldMatrix();
		return true;
	}
	return false;
}


bool CHexObject::resolvingSerialAction() {
	return action & actSerial;
}


void CHexObject::findTravelPath(CHex& target) {
	//ordinarily, just find the path from where we are now.
	//but if we've moving, makes sense that the new path will start
	//where we end up
	if (moving)
		travelPath = hexWorld->getPathCB(destination, target);
	else
		travelPath = hexWorld->getPathCB(hexPosition,target);

}

/** Construct this object's world matrix from its known position and rotation.*/
void CHexObject::buildWorldMatrix() {
	worldMatrix = glm::translate(glm::mat4(1), worldPos);
	worldMatrix = glm::rotate(worldMatrix, rotation, glm::vec3(0, 0, -1));
}




