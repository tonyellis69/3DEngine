#pragma once

#include "glm\glm.hpp"
#include "..\renderer\model.h"

/** The basic physics object. */
class CPhysObj {
public:

	void attachModel(CModel* model);
	void setVelocity(glm::vec3& newVelocity);
	void setMass(float newMass);
	void update(const float & dT);


private:
	glm::vec3 velocity;
	float mass;

	CModel* pModel;


};