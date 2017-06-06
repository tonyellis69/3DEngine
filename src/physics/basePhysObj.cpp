#include "basePhysObj.h"


using namespace glm;

CBasePhysObj::CBasePhysObj() {
	collides = true;
}



void CBasePhysObj::setVelocity(glm::vec3 & newVelocity) {
	velocity = newVelocity;
}

void CBasePhysObj::setMass(float newMass) {
	mass = newMass;
}









void CBasePhysObj::setCollides(bool status) {
	collides = status;
}
