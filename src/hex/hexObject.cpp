#include "hexObject.h"

#include <cmath>
#include <algorithm>

#include "..\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	


CHexObject::CHexObject() {
	moving = false;
	isRobot = false;
	worldPos = glm::vec3(0);
	setZheight(0.05f);
	action = actNone;
	facing = hexEast;
	rotation = 0.0f;
	rotationalVelocity = 0.0f;
	turning = false;
	proximityCutoff = 0.15f;
	moveSpeed = 7.0f;
	lungeSpeed = 3.0f;
	hitPoints = 1;
	meleeDamage = 1;

	drawData = { &worldMatrix,&colour,buf };

	buildWorldMatrix();
}

void CHexObject::setHexWorld(IhexObjectCallback* obj) {
	hexWorld = obj;
}

void CHexObject::setHexRenderer(IHexRenderer* rendrObj) {
	hexRendr = rendrObj;
}



/** Set position using individual hex cube coordinates. */
void CHexObject::setPosition(int x, int y, int z) {
	hexPosition.x = x;
	hexPosition.y = y;
	hexPosition.z = z;
	destination = hexPosition;

	glm::i32vec2 axial = hexPosition.getAxial();
	
	worldPos.x = hexWidth * axial.x + hexWidth / 2.0f * axial.y;
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

void CHexObject::setBuffer(CBuf* buffer) {
	buf = buffer;
	drawData.buf = buf;
}

/** Set the rotation and facing direction of this object. */
void CHexObject::setDirection(THexDir direction) {
	facing = direction;
	rotation = dirToAngle(direction);
	buildWorldMatrix();
}


/**	Initialise this object to start moving to the next hex on its current travel path when it gets
	updated. This may include rotating to face that hex. */
bool CHexObject::beginMove() {
	if (travelPath.empty() || hexWorld->entityMovingTo(travelPath[0]) )
		return false;
	initMoveToAdjacent(travelPath[0]);
	initTurnToAdjacent(travelPath[0]);
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

bool CHexObject::isResolvingSerialAction() {
	return action & actSerial;
}

bool CHexObject::isNeighbour(CHex& position) {
	return ::isNeighbour(hexPosition, position);
}

bool CHexObject::updateMove(float dT) {
	if (moving) {
		if (turning) {
			updateRotation(dT);
			return true;
		}
		return updateMovement(dT);
	}
	return false;
}

void CHexObject::receiveDamage(CHexObject& attacker, int damage) {
	hitPoints -= damage;
}

void CHexObject::draw(){
	hexRendr->drawLines(drawData);
}


void CHexObject::calcTravelPath(CHex& target) {
	//ordinarily, just find the path from where we are now.
	//but if we've moving, makes sense that the new path will start
	//where we end up
	if (moving)
		travelPath = hexWorld->calcPath(destination, target);
	else
		travelPath = hexWorld->calcPath(hexPosition,target);
}

/** Advance the lunge animation. */
bool CHexObject::updateLunge(float dT) {
	float lungeDistance = animCycle * 2.0f - 1.0f;
	//float sign = lungeDistance > 0 ? 1.0f : -1.0f;

	lungeDistance = 1.0f - pow(abs(lungeDistance), 0.6f);
	lungeDistance *= hexWidth;
	glm::vec3 lungeVec = moveVector * lungeDistance;

	worldPos = cubeToWorldSpace(hexPosition) + lungeVec;
	buildWorldMatrix();

	animCycle += dT * lungeSpeed;
	if (animCycle > 1.0f) {
		setPosition(hexPosition); //ensures we don't drift.
		hitTarget();
		return false;
	}
	else {
		return true;
	}
}

/** Construct this object's world matrix from its known position and rotation.*/
void CHexObject::buildWorldMatrix() {
	worldMatrix = glm::translate(glm::mat4(1), worldPos);
	worldMatrix = glm::rotate(worldMatrix, rotation, glm::vec3(0, 0, -1));
}




/* Initialise a move action to the given hex. */
void CHexObject::initMoveToAdjacent(CHex& adjacent) {
	moving = true;
	destination = travelPath[0];
	worldSpaceDestination = cubeToWorldSpace(destination);
	moveVector = glm::normalize(worldSpaceDestination - cubeToWorldSpace(hexPosition));
}

/** Initialise a rotation action to face the given hex. */
bool CHexObject::initTurnToAdjacent(CHex& adjacent) {
	destinationAngle = 0;
	destinationDirection = neighbourDirection(hexPosition, adjacent);
	float rotationDir = shortestRotation(facing, destinationDirection);
	if (rotationDir == 0.0f)
		return false;
	moving = true;
	turning = true;
	rotationalVelocity = (rotationDir > 0) - (rotationDir < 0);
	destinationAngle = dirToAngle(destinationDirection);
	return true;
}

///////////////private functions

/** Apply dT seconds of the rotation this object is undergoing. */
bool CHexObject::updateRotation(float dT){	
	float gap = std::fmod(rotation - destinationAngle + M_PI, 2 * M_PI) - M_PI;
	if (abs(gap) < 0.1f) {
		turning = false;
		setDirection(destinationDirection);
		return false; 
	}
	else {
		rotation += rotationalVelocity * 10.0f * dT;
		rotation = glm::mod<float>(rotation, 2 * M_PI);
	}
	buildWorldMatrix();
	return true;
}

/** Deliver damage to our current target. */
void CHexObject::hitTarget() {
	attackTarget->receiveDamage(*this, meleeDamage);
}

/**	Apply dT seconds of the movement this object is undergoing. */
bool CHexObject::updateMovement(float dT) {
	glm::vec3 velocity = moveVector * moveSpeed * dT;

	if (glm::distance(worldPos, worldSpaceDestination) < proximityCutoff) {
		moving = false;
		velocity = glm::vec3(0);
		setPosition(destination.x, destination.y, destination.z);
		if (!travelPath.empty() && destination == travelPath[0])
			travelPath.erase(travelPath.begin());
		return false;
	}
	else {
		worldPos += velocity;
	}

	buildWorldMatrix();
	return true;
}





