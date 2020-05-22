#include "physObj2.h"

#include "utils/log.h"

CPhysObj2::CPhysObj2() {
	force = glm::vec3(0.0f) ;
	invMass = 0;
	restitution = 0.01f;
}

void CPhysObj2::updateVelocity(float dT) {
	if (invMass == 0)
		return;
	velocity += dT * (gravity + invMass * force);
	sysLog << "\nvelocity updated to " << velocity;
}

void CPhysObj2::updatePosition(float dT) {
	if (invMass == 0)
		return;
	updateModelPosition(dT * velocity);
}




void CPhysObj2::setMass(float mass) {
	invMass = 1.0f / mass;
}

void CPhysObj2::setVelocity(glm::vec3& velocity) {
	this->velocity = velocity;
}



bool TAaBB::clips(TAaBB& boxB) {
	// Exit with no intersection if separated along an axis
	if (AABBmax.x < boxB.AABBmin.x || AABBmin.x > boxB.AABBmax.x) return false;
	if (AABBmax.y < boxB.AABBmin.y || AABBmin.y > boxB.AABBmax.y) return false;
	if (AABBmax.z < boxB.AABBmin.z || AABBmin.z > boxB.AABBmax.z) return false;
	// Overlapping on all axes means AABBs are intersecting
	return true;
}

bool TAaBB::clips(glm::vec3& baseVertB) {
	if (baseVertB.x < AABBmin.x || baseVertB.x > AABBmax.x) return false;
	if (baseVertB.y < AABBmin.y || baseVertB.y > AABBmax.y) return false;
	if (baseVertB.z < AABBmin.z || baseVertB.z > AABBmax.z) return false;

	return true;;
}
