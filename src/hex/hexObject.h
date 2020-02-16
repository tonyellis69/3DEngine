#pragma once

#define _USE_MATH_DEFINES //for cmath
#include <glm/glm.hpp>

#include "../renderer/buf.h"
#include "hex.h"

#include "../hex/IHexRenderer.h"

#include "../VMtest/src/ITigObj.h"

class IhexObjectCallback;
/** A class for defining moveable objects in the hex world.*/
class CHexObject : public CTigObjptr {
public:
	CHexObject();
	void setHexWorld(IhexObjectCallback* obj);
	static void setHexRenderer(IHexRenderer* rendrObj);
	void setPosition(int x, int y, int z);
	void setPosition(int x, int y);
	void setPosition(CHex& hex);
	void setBuffer(CBuf* buffer);
	void setDirection(THexDir direction);
	bool beginMove();
	void setZheight(float height);
	void setTravelPath(THexList& path);
	THexList& getTravelPath();
	virtual bool update(float dT) { return false; };
	void calcTravelPath(CHex& target);
	virtual void chooseTurnAction() {};
	virtual void beginTurnAction() {};
	virtual bool isResolvingSerialAction();
	bool isNeighbour(CHex& position);
	bool updateMove(float dT);
	virtual void receiveDamage(CHexObject& attacker, int damage);
	virtual void draw();

	bool moving; ///<True if we're in the process of travelling to destination.
	CBuf* buf; ///<Identifies the graphics buffer to use for drawing this object 
	glm::mat4 worldMatrix; ///<Position and orientation in the 3D universe.
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.
	glm::vec3 worldPos; ///<Position in world space.

	bool isRobot;
	CHex destination; ///<The hex we're travelling to.

	int shields[6] = { 0 }; //TO DO: might not suit base class

protected:
	void buildWorldMatrix();
	virtual bool updateLunge(float dT);
	void initMoveToAdjacent(CHex& adjacent);
	bool initTurnToAdjacent(CHex& adjacent);
	bool updateRotation(float dT);

	void hitTarget();

	void tigCall(int memberId) {};

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
	//exDir targetDirection;

	float animCycle; ///<Records how far through an animation we are.

	IhexObjectCallback* hexWorld;

	inline static IHexRenderer* hexRendr;

	int action; ///<The action this entity is performing this turn.
	
	float proximityCutoff;
	float moveSpeed;
	float lungeSpeed;

	int hitPoints;
	int meleeDamage;
	CHexObject* attackTarget;

	glm::vec4 colour;
	THexDraw drawData;

private:
	virtual void beginAttack(CHexObject& target) {};
	


	bool updateMovement(float dT);


};


using TEntities = std::vector<CHexObject*>;

class IhexObjectCallback {
public:
	virtual THexList calcPath(CHex& start, CHex& end) = 0; 
	virtual CHexObject* getEntityAt(CHex& hex) = 0;
	virtual CHexObject* entityMovingTo(CHex& hex) = 0;
	virtual void onPlayerTurnDoneCB() = 0;
	virtual CHexObject* getPlayerObj() = 0;
	virtual CHex getPlayerPosition() = 0;
	virtual CHex getPlayerDestinationCB() = 0;
	virtual bool isEntityDestinationCB(CHex& hex) = 0;
	virtual int diceRoll(int dice) = 0;
};


//Action flags
const int actSerial = 0x80000000;
const int actNone = 0x0;
const int actChasePlayer = 0x1;
const int actAttackPlayer = 0x80000002;
const int actCombatPassive = 0x3;
const int actTrackPlayer = 0x4;
const int actPlayerAttack = 0x80000005;
const int actPlayerTurnToAttack = 0x80000006;
const int actDead = 0x7;
const int actDither = 0x8;
