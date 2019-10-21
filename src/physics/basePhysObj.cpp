#include "basePhysObj.h"


using namespace glm;

CBasePhysObj::CBasePhysObj() {
	collides = true;
	velocity = vec3(0);
	setMass(10);
	damping =  0.9f;// 0.4f;;
	//setAcceleration(vec3(0,-7,0));
	setAcceleration(vec3(0, 0, 0)); ///TEMP!!! switched off gravity
	asleep = false;
	groundContact = false;
}



void CBasePhysObj::setVelocity(glm::vec3 & newVelocity) {
	velocity = newVelocity;
}

void CBasePhysObj::setMass(float newMass) {
	inverseMass = 1.0f / mass;
	mass = newMass;
}

void CBasePhysObj::setAcceleration(vec3 & accel) {
	acceleration = accel;
}









void CBasePhysObj::setCollides(bool status) {
	collides = status;
}
