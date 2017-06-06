#pragma once

#include "glm\glm.hpp"
#include "..\renderer\model.h"
#include "basePhysObj.h"

/** The basic physics object. */
class CPhysObj : public CBasePhysObj {
public:

	void attachModel(CModel* model);
	void setVelocity(glm::vec3& newVelocity);
	void setMass(float newMass);
	glm::vec3 update(const float & dT);
	CModel* getModel();
	void modifyVelocity(glm::vec3 & modifier);
	void applyVelocity();

	//CModel* pModel;

private:
//	glm::vec3 velocity;
//	float mass;




};