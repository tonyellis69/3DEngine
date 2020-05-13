#pragma once

#define _USE_MATH_DEFINES //for cmath
#include <glm/glm.hpp>

#include "../renderer/buf.h"
#include "hex.h"

#include "../hex/IHexRenderer.h"
#include "hex/lineModel.h"




class IGameHexArray;
/** A class for defining moveable objects in the hex world.*/
class CHexObject  {
public:
	CHexObject();
	static void setHexRenderer(IHexRenderer* rendrObj);
	virtual void setPosition(int x, int y, int z);
	void setPosition(int x, int y);
	void setPosition(CHex& hex);
	virtual void setLineModel(const std::string& name);
	//void setLineModel(CLineModel& model); //scrap
	void setDirection(THexDir direction);
	virtual bool beginMove();
	void setZheight(float height);
	void setTravelPath(THexList& path);
	THexList& getTravelPath();
	virtual bool update(float dT) { return false; };
	virtual void calcTravelPath(CHex& target) {};
	bool isNeighbour(CHex& position);
	bool updateMove(float dT);
	bool updateRotationOnly(float dT);
	virtual void draw();
	THexDir getDirection() {
		return facing;
	}

	glm::mat4* worldMatrix; ///<Position and orientation in the 3D universe.
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.
	glm::vec3 worldPos; ///<Position in world space.

	CHex destination; ///<The hex we're travelling to.

	CLineModel lineModel;

	IGameHexArray* map; ///<The map this object exists in.



protected:
	void buildWorldMatrix();	
	void initMoveToAdjacent(CHex& adjacent);
	bool initTurnToAdjacent(CHex& adjacent);
	bool updateRotation(float dT);
	bool updateMovement(float dT);

	float zHeight; ///<Height above XY plane where drawn.
	
	glm::vec3 worldSpaceDestination; ///<Worldspace point we're travelling to.
	glm::vec3 moveVector; ///<Direction of travel as a 3D space vector.

	THexDir facing; ///<Direction entity is facing.
	THexDir destinationDirection; ///<Direction of destination hex.
	float destinationAngle; ///<Rotation we're trying to achieve.
	float rotation; ///<Angle of object's z-rotation in world space.
	float rotationalVelocity; ///<Which way we're turning, how fast.
	bool turning; ///<True if we're engaged in turning.

	THexList travelPath; ///<Route for movement.

	float animCycle; ///<Records how far through an animation we are.

	inline static IHexRenderer* hexRendr;

	
	float proximityCutoff;
	float moveSpeed;
	
	glm::vec4 colour;
	

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

