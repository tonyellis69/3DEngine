#include "physObj.h"

#include "..\watch.h"

using namespace glm;

void CPhysObj::attachModel(CModel * model) {
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




CModel * CPhysObj::getModel() {
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
	//update position from velocity
	position += velocity * dT;

	//update velocity from acceleration
	velocity += acceleration * dT;

	//apply damping
	velocity *= pow(damping,dT);

}

void CPhysObj::repositionModel() {
	pModel->setPos(position);
}
