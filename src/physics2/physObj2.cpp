#include "physObj2.h"


CPhysObj2::CPhysObj2() {
	force = glm::vec3(0.0f) ;
	invMass = 0;
}

void CPhysObj2::updateVelocity(float dT) {
	if (invMass == 0)
		return;
	velocity += dT * (gravity + invMass * force);

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

void CPhysObj2::setBB(glm::vec3& size) {
	BBbl = size * -0.5f;
	BBtr = size * 0.5f;
}

bool TAaBB::clips(TAaBB& boxB) {
	// Exit with no intersection if separated along an axis
	if (AABBmax.x < boxB.AABBmin.x || AABBmin.x > boxB.AABBmax.x) return false;
	if (AABBmax.y < boxB.AABBmin.y || AABBmin.y > boxB.AABBmax.y) return false;
	if (AABBmax.z < boxB.AABBmin.z || AABBmin.z > boxB.AABBmax.z) return false;
	// Overlapping on all axes means AABBs are intersecting
	return true;
}
