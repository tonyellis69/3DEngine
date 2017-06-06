#pragma once

#include "glm\glm.hpp"
#include "..\renderer\model.h"

/** The base basic physics object. */
class CBasePhysObj {
public:
	CBasePhysObj();
	virtual void attachModel(CModel* model) {};
	void setVelocity(glm::vec3& newVelocity);
	void setMass(float newMass);
	virtual glm::vec3 update(const float & dT) { return glm::vec3(0); };
	virtual CModel* getModel() { return 0; };
	virtual void modifyVelocity(glm::vec3& modifier) {};
	virtual void applyVelocity() {};
	void setCollides(bool status);
	virtual glm::vec3 collisionCheck(glm::vec3 pos) { return glm::vec3(0); };

	bool collides; ///<True if this object collides *into* other objects.

	CModel* pModel;
protected:
	glm::vec3 velocity;
	float mass;

	



};