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
vec3 CPhysObj::update(const float & dT) {
	velocity = velocity + (vec3(0, -1, 0) *dT);
	return velocity;
}

CModel * CPhysObj::getModel() {
	return pModel;
}

void CPhysObj::modifyVelocity(glm::vec3 & modifier) {
	velocity += modifier;
}

void CPhysObj::applyVelocity() {
	pModel->translate(velocity);
}
