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
	bool beginMove();
	void setZheight(float height);
	void setTravelPath(THexList& path);
	THexList& getTravelPath();
	virtual bool update(float dT) ;
	void findTravelPath(CHex& target);
	virtual void chooseTurnAction() {};
	virtual void beginTurnAction() {};
	virtual bool resolvingSerialAction();

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
	IhexObjectCallback* hexWorld;

	int action; ///<The action this entity is performing this turn.

private:

};


using TEntities = std::vector<CHexObject*>;

class IhexObjectCallback {
public:
	virtual THexList getPathCB(CHex& start, CHex& end) = 0; 
	virtual CHexObject* getEntityAtCB(CHex& hex) = 0;
	virtual void onPlayerTurnDoneCB() = 0;
	virtual CHex getPlayerPositionCB() = 0;
	virtual CHex getPlayerDestinationCB() = 0;
	virtual bool isEntityDestinationCB(CHex& hex) = 0;
};


//Action flags
const int actSerial = 0x80000000;
const int actNone = 0x0;
const int actChasePlayer = 0x1;
const int actAttackPlayer = 0x80000002;
const int actCombatPassive = 0x3;
const int actTrackPlayer = 0x4;
