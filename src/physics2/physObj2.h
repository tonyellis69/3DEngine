#pragma once

#include "glm/glm.hpp"

#include "physics2/contact.h"

class Contact; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

enum TCollider {collideAAbaseVert, collideTerrain };

struct TAaBB {
public:
	bool clips(TAaBB& boxB);
	bool clips(glm::vec3& baseVertB);

	glm::vec3 AABBmin;
	glm::vec3 AABBmax;
};

/** A class defining the basic physics object. */
class CPhysObj2 {
public:
	CPhysObj2();
	void updateVelocity(float dT);
	void updatePosition(float dT);
	void setMass(float mass);
	void setVelocity(glm::vec3& velocity);


	virtual void updateModelPosition(glm::vec3& dPos) = 0;
	virtual glm::vec3 getPos() = 0;
	virtual TAaBB calcAABB() = 0;
	virtual glm::vec3 calcBaseVertPos() = 0;

	virtual Contact checkCollision(CPhysObj2* objB);
	//TO DO: diamond problem! Contacts already have to know phys objs
	//so phys objs ideally should not know contacts

	glm::vec3 velocity;
	static inline glm::vec3 gravity;
	float invMass;
	glm::vec3 force;
	float restitution; ///<Bounciness.

	TCollider collideType = collideAAbaseVert;

	
	};