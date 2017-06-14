#pragma once

#include "glm\glm.hpp"
#include "..\renderer\model.h"
#include "bsphere.h"

/** The base basic physics object. */
class CBasePhysObj {
public:
	CBasePhysObj();
	virtual void attachModel(CModel* model) {};
	void setVelocity(glm::vec3& newVelocity);
	void setMass(float newMass);
	void setAcceleration(glm::vec3& accel);
	virtual CModel* getModel() { return 0; };
	virtual void modifyVelocity(glm::vec3& modifier) {};
	virtual void applyVelocity() {};
	void setCollides(bool status);
	virtual glm::vec3 collisionCheck(CBasePhysObj & collider) { return glm::vec3(0); };
	virtual void integrate(float dT) {};
	virtual void repositionModel() {};

	bool collides; ///<True if this object collides *into* other objects.

	CModel* pModel;

	CBsphere bSphere; ///<Bounding sphere for this object
	glm::vec3 velocity;
	glm::vec3 position;
	float inverseMass;

protected:
	float mass;
	
	glm::vec3 acceleration;
	float damping;

	



};