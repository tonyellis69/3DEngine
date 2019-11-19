#pragma once

#include <glm/glm.hpp>

#include "../renderer/buf.h"
#include "hex.h"


/** A class for defining moveable objects in the hex world.*/
class CHexObject {
public:
	CHexObject() { zHeight = 0; };
	void setPosition(int x, int y, int z);
	void move(THexDir direction);
	void setZheight(float height);

	CBuf* buf; ///<Identifies the graphics buffer to use for drawing this object 
	glm::mat4 worldMatrix; ///<Position in the 3D universe.
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.

private:
	float zHeight; ///<Height above XY plane where drawn.
	

};