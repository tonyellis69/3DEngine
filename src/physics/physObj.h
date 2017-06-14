#pragma once

#include "glm\glm.hpp"
#include "..\renderer\model.h"
#include "physObjManager.h" 
#include "basePhysObj.h"

/** The basic physics object. */
class CPhysObj : public CBasePhysObj {
public:

	void attachModel(CModel* model);
	void setVelocity(glm::vec3& newVelocity);
	void setMass(float newMass);
	CModel* getModel();
	void modifyVelocity(glm::vec3 & modifier);
	void applyVelocity();
	void integrate(float dT);
	void repositionModel();

	//CModel* pModel;
	CPhysObjManager* pManager;
private:
//	glm::vec3 velocity;
//	float mass;




};