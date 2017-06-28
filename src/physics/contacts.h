#pragma once

#include "basePhysObj.h"
#include "glm\glm.hpp"

/** Keeps track of a collision between two physics objects. */

class contact {
public:
	contact() {};
	contact(CBasePhysObj* collider, CBasePhysObj* collidee);
	float calcSeparatingVelocity();
	void resolve(float dT);
	void resolveVelocity(float dT);
	void resolvePenetration(float dT);

	CBasePhysObj* collider;
	CBasePhysObj* collidee;

	float restitution; ///<Coefficient of resistution - overall springiness of the collision. 1 = hard 0 = sticky.
	glm::vec3 contactNormal; ///<Direction of the contact

	float penetration; ///<Depth of penetration of this contact.

	glm::vec3 colliderMovement; ///<Amount collider is moved in penetration resolution.
	glm::vec3 collideeMovement; ///<Amount collidee is moved in penetration resolution.

	bool resolved;
	bool resting;

	float depenetrationAggression;

	unsigned int id;
};