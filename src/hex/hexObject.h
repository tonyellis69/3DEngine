#pragma once

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
	void move(THexDir direction);
	void startTravel();
	void setZheight(float height);
	void setTravelPath(THexList& path);
	THexList& getTravelPath();
	bool update(float dT);
	void findTravelPath(CHex& target);

	bool moving; ///<True if we're in the process of travelling to destination.
	CBuf* buf; ///<Identifies the graphics buffer to use for drawing this object 
	glm::mat4 worldMatrix; ///<Position in the 3D universe.
	CHex hexPosition; ///<Position on a hex grid in cube coordinates.

	bool isRobot;
protected:
	float zHeight; ///<Height above XY plane where drawn.
	CHex destination; ///<The hex we're travelling to.
	glm::vec3 worldSpaceDestination; ///<Worldspace point we're travelling to.
	glm::vec3 moveVector; ///<Direction of travel as a 3D space vector.

	THexList travelPath; ///<Route for movement.

private:
	IhexObjectCallback* callbackObj;
};


using TEntities = std::vector<CHexObject*>;

class IhexObjectCallback {
public:
	virtual THexList getPathCallback(CHex& start, CHex& end) = 0; // { return THexList x; }
};
