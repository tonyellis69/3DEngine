#pragma once

#define _USE_MATH_DEFINES //for cmath
#include <glm/glm.hpp>

#include "../renderer/buf.h"
#include "hex.h"

#include "../hex/IHexRenderer.h"
#include "hex/lineModel.h"

struct TFov { //describes a fov shape
	CHex A;
	CHex B;
	THexList arc;
};
/** A class for defining moveable objects in the hex world.*/
class CHexObject  {
public:
	CHexObject();
	virtual ~CHexObject() {}
	static void setHexRenderer(IHexRenderer* rendrObj);
	virtual void setPosition(int x, int y, int z);
	void setPosition(int x, int y);
	void setPosition(CHex& hex);
	virtual void setLineModel(const std::string& name);
	void setDirection(THexDir direction);
	void setZheight(float height);
	virtual bool update(float dT) { return false; };
	

	virtual void draw();
	THexDir getDirection() {
		return facing;
	}

	glm::mat4* worldMatrix; ///<Position and orientation in the 3D universe.
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.
	glm::vec3 worldPos; ///<Position in world space.
	CHex destination; ///<The hex we're travelling to.
	CLineModel lineModel;

	
protected:
	void buildWorldMatrix();	

	float zHeight; ///<Height above XY plane where drawn.
	
	THexDir facing; ///<Direction entity is facing.
	float rotation; ///<Angle of object's z-rotation in world space.

	THexList travelPath; ///<Route for movement.

	inline static IHexRenderer* hexRendr;


	float moveSpeed;

};



const int blocksNone = 0;
const int blocksEast = 1;
const int blocksSE = 2;
const int blocksSW = 4;
const int blocksWest = 8;
const int blocksNW = 16;
const int blocksNE = 32;
const int blocksAll = 63;
const int blocksAsDoor = 54;

