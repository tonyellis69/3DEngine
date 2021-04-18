#include "hexObject.h"

#include "..\utils\log.h"

#include "..\3Dtest\src\hexRenderer.h"

#include <glm/gtc/matrix_transform.hpp>	


CHexObject::CHexObject() {
	worldPos = glm::vec3(0);
	worldMatrix = &lineModel.model.matrix;

	//setZheight(0.05f);
	setZheight(0.0f);

	facing = hexEast;
	rotation = 0.0f;
	moveSpeed = 7.0f;

	buildWorldMatrix();
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

void CHexObject::setLineModel(const std::string& name) {
	lineModel = hexRendr2.getLineModel(name);
}

/** Set the rotation and facing direction of this object. */
void CHexObject::setDirection(THexDir direction) {
	facing = direction;
	rotation = dirToAngle(direction);
	buildWorldMatrix();
}


/** Set the height at which this object is drawn. */
void CHexObject::setZheight(float height) {
	zHeight = height;
	worldPos.z = height;
	buildWorldMatrix();
}


void CHexObject::draw(){
	hexRendr2.drawLineModel(lineModel);
}


/** Construct this object's world matrix from its known position and rotation.*/
void CHexObject::buildWorldMatrix() {
	*worldMatrix = glm::translate(glm::mat4(1), worldPos);
	*worldMatrix = glm::rotate(*worldMatrix, rotation, glm::vec3(0, 0, -1));
	//NB: we use a CW system for angles
}


///////////////private functions



