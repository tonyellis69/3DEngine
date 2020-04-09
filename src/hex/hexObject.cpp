#include "hexObject.h"

#include <cmath>
#include <algorithm>

#include "..\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	

#include "..\TC\Debug\tigConst.h" //temp!!!!

CHexObject::CHexObject() {
	worldPos = glm::vec3(0);
	setZheight(0.05f);

	facing = hexEast;
	rotation = 0.0f;
	rotationalVelocity = 0.0f;
	turning = false;
	proximityCutoff = 0.15f;
	moveSpeed = 7.0f;

;

	drawData = { &worldMatrix,&colour,buf };

	buildWorldMatrix();
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

void CHexObject::setBuffer(const std::string& bufName) {
	drawData.buf = hexRendr->getBuffer(bufName);
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
	if (travelPath.empty() )
		return false;
	initMoveToAdjacent(travelPath[0]);
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


bool CHexObject::isNeighbour(CHex& position) {
	return ::isNeighbour(hexPosition, position);
}

bool CHexObject::updateMove(float dT) {
		if (turning) {
			updateRotation(dT);
			return true;
		}
		return updateMovement(dT);
	return false;
}

bool CHexObject::updateRotationOnly(float dT) {
	if ( turning) {
		if (!updateRotation(dT)) {
			return false;
		}
	}
	return true;
}



void CHexObject::draw(){
	hexRendr->drawLines(drawData);
}



/** Construct this object's world matrix from its known position and rotation.*/
void CHexObject::buildWorldMatrix() {
	worldMatrix = glm::translate(glm::mat4(1), worldPos);
	worldMatrix = glm::rotate(worldMatrix, rotation, glm::vec3(0, 0, -1));
	//NB: we use a CW system for angles
}




/* Initialise a move action to the given hex. */
void CHexObject::initMoveToAdjacent(CHex& adjacent) {
	//moving = true;
	destination = travelPath[0];
	worldSpaceDestination = cubeToWorldSpace(destination);
	moveVector = glm::normalize(worldSpaceDestination - cubeToWorldSpace(hexPosition));
}

//TO DO: need a function that returns the approximate direction for *any* hex
	//not just an adjacent one
/** Initialise a rotation action to face the given adjacent hex. */
bool CHexObject::initTurnToAdjacent(CHex& adjacent) {
	destinationAngle = 0;
	destinationDirection = neighbourDirection(hexPosition, adjacent);
	if (destinationDirection == hexNone)
		return false;

	float dist = 2 * M_PI + dirToAngle(destinationDirection) - rotation;
	dist = fmod(dist, 2 * M_PI);
	if (dist > M_PI)
		dist = -(2 * M_PI - dist);
		
	if (dist == 0.0f)
		return false;
	//moving = true;
	turning = true;
	rotationalVelocity = (dist > 0) - (dist < 0);
	destinationAngle = dirToAngle(destinationDirection);
	return true;
}

///////////////private functions

/** Apply dT seconds of the rotation this object is undergoing. */
bool CHexObject::updateRotation(float dT){	
	float gap = std::fmod(rotation - destinationAngle + M_PI, 2 * M_PI) - M_PI;
	if (abs(gap) < 0.1f) {
		turning = false;
		//setDirection(destinationDirection);
		facing = angleToDir(rotation);
		return false; 
	}
	else {
		rotation += rotationalVelocity * 10.0f * dT;
		rotation = glm::mod<float>(rotation, 2 * M_PI);
		
	}
	buildWorldMatrix();
	return true;
}


/**	Apply dT seconds of the movement this object is undergoing. */
bool CHexObject::updateMovement(float dT) {
	glm::vec3 velocity = moveVector * moveSpeed * dT;

	if (glm::distance(worldPos, worldSpaceDestination) < proximityCutoff) {
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





