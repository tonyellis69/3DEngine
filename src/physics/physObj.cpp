#include "physObj.h"


#include <iostream>
using namespace glm;

void CPhysObj::attachModel(C3dObject * model) {
	pModel = model; 
	position = pModel->getPos();
	lastContact = position;
}

void CPhysObj::setVelocity(glm::vec3 & newVelocity) {
	velocity = newVelocity;
}

void CPhysObj::setMass(float newMass) {
	mass = newMass;
}




C3dObject * CPhysObj::getModel() {
	return pModel;
}

void CPhysObj::modifyVelocity(glm::vec3 & modifier) {
	velocity += modifier;
}

void CPhysObj::applyVelocity() {
	pModel->translate(velocity);
}

/** Integrate the object's positionHint and velocity, over the given time. */
void CPhysObj::integrate(float dT) {

	//apply faux friction
	if (groundContact) {
		velocity.x *= 0.9f;
		velocity.z *= 0.9f;
	}

	//update positionHint from velocity
	position += velocity * dT;

	//update velocity from acceleration
	velocity += acceleration * dT;

	//apply damping
	velocity *= pow(damping,dT);
	
	
	
}

void CPhysObj::repositionModel() {
	pModel->setPos(position);
}

/** Reposition at associated model. */
void CPhysObj::moveToModelPosition() {
	position = pModel->getPos();
 }
