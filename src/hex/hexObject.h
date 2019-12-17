#pragma once

#define _USE_MATH_DEFINES //for cmath
#include <glm/glm.hpp>

#include "../renderer/buf.h"
#include "hex.h"

class IhexObjectCallback;
/** A class for defining moveable objects in the hex world.*/
class CHexObject {
public:
	CHexObject();
	void setCallbackObj(IhexObjectCallback* obj);
	void setPosition(int x, int y, int z);
	void setPosition(int x, int y);
	void setPosition(CHex& hex);
	void setDirection(THexDir direction);
	void newMove();
	void setZheight(float height);
	void setTravelPath(THexList& path);
	THexList& getTravelPath();
	virtual bool update(float dT) ;
	void findTravelPath(CHex& target);
	virtual void chooseTurnAction() {};

	bool moving; ///<True if we're in the process of travelling to destination.
	CBuf* buf; ///<Identifies the graphics buffer to use for drawing this object 
	glm::mat4 worldMatrix; ///<Position and orientation in the 3D universe.
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.
	glm::vec3 worldPos; ///<Position in world space.

	bool isRobot;
	CHex destination; ///<The hex we're travelling to.
protected:
	void buildWorldMatrix();

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

protected:
	IhexObjectCallback* callbackObj;
};


using TEntities = std::vector<CHexObject*>;

class IhexObjectCallback {
public:
	virtual THexList getPathCB(CHex& start, CHex& end) = 0; 
	virtual CHexObject* getEntityAtCB(CHex& hex) = 0;
	virtual void onPlayerTurnDoneCB() = 0;
	virtual CHex getPlayerPositionCB() = 0;
};
