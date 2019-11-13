#pragma once

#include <glm/glm.hpp>

#include "../renderer/buf.h"
#include "hex.h"

enum THexDir { hexNE, hexEast, hexSE, hexSW, hexWest, hexNW};

/** A class for defining moveable objects in the hex world.*/
class CHexObject {
public:

	void setPosition(int x, int y, int z);
	void move(THexDir direction);

	CBuf* buf; ///<Identifies the graphics buffer to use for drawing this object 
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.
	glm::mat4 worldMatrix; ///<Position in the 3D universe.

	

};