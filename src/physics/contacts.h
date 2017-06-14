#pragma once

#include "basePhysObj.h"
#include "glm\glm.hpp"

/** Keeps track of a collision between two physics objects. */

class contact {
public:
	contact(CBasePhysObj* collider, CBasePhysObj* collidee);
	float calcSeparatingVelocity();
	void resolve();
	void resolveVelocity();

	CBasePhysObj* collider;
	CBasePhysObj* collidee;

	float restitution; ///<Coefficient of resistution - overall springiness of the collision. 1 = hard 0 = sticky.
	glm::vec3 contactNormal; ///<Direction of the contact

};