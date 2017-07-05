#include "physObj.h"

#include "..\watch.h"
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

/** Integrate the object's position and velocity, over the given time. */
void CPhysObj::integrate(float dT) {

//	lastVelocity = velocity;

	//update position from velocity
	position += velocity * dT;

	std::cerr << "\nIntegration moves collider to " << position.x << " " << position.y << " " << position.z;

	if (position.y > oldHeight) {
	//	position.y = oldHeight;
	}
	oldHeight = position.y;


	//update velocity from acceleration
	velocity += acceleration * dT;

	//apply damping
	velocity *= pow(damping,dT);

	

}

void CPhysObj::repositionModel() {
	pModel->setPos(position);
}

/** Position with its associate model. */
void CPhysObj::updatePosition() {
	position = pModel->getPos();
}
