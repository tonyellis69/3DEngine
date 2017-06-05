#include "physObj.h"

#include "..\watch.h"

using namespace glm;

void CPhysObj::attachModel(CModel * model) {
	pModel = model; 
}

void CPhysObj::setVelocity(glm::vec3 & newVelocity) {
	velocity = newVelocity;
}

void CPhysObj::setMass(float newMass) {
	mass = newMass;
}


/** Update the physics state. */
void CPhysObj::update(const float & dT) {
	velocity = velocity + (vec3(0, -1, 0) *dT);
	pModel->translate(velocity);
}
