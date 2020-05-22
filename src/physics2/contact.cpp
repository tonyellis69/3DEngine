#include "contact.h"

#include <algorithm>

#include "utils/log.h"

Contact::Contact() {
	numPoints = 0;

}

void Contact::applyImpulse() {
	sysLog << "\nvelocity at impulse start " << objB->velocity;


	//calculate relative velocity of A & B
	glm::vec3 relativeVelocity = objB->velocity - objA->velocity;

	//find velocity along collision normal
	float velocityAlongNormal = glm::dot(relativeVelocity, normal);

	//Separating? Then don't resolve
	if (velocityAlongNormal > 0) {
		sysLog << "\nSeparating!";
		return;
	}

	if (objB->velocity.x == 0 && objB->velocity.z == 0)
		int d = 0;

	//find the resitution
	float e = std::min(objA->restitution, objB->restitution);

	//calcualate the impulse scalar
	float j = -(1 + e) * velocityAlongNormal;
	j /= objA->invMass + objB->invMass;

	//apply it to the bodies relative to their mass
	glm::vec3 impulse = j * normal;
	objA->velocity -= objA->invMass * impulse;
	objB->velocity += objB->invMass * impulse;

	

	//find the new relative velocity
	relativeVelocity = objB->velocity - objA->velocity;

	//find tangent to the collision normal in the direction of the relative velocity
	glm::vec3 tangent = relativeVelocity - (glm::dot(relativeVelocity, normal) * normal);

	if (glm::length(tangent) == 0) //no tangental velocity, so no friction to find
		return;
	tangent = glm::normalize(tangent);

	//find the relative velocity along that tangent and turn it inward
	float jt = -glm::dot(relativeVelocity, tangent);
	jt /= objA->invMass + objB->invMass;

	/* float friction = 0.2f;
	float maxFriction = friction * j;
	jt = glm::clamp(jt, -maxFriction, maxFriction);

	glm::vec3 frictionImpulse = jt * tangent;*/

	float mu = 0.2f; //TO DO: should be combined frictions of objects
	glm::vec3 frictionImpulse;

	if (abs(jt) < j * mu)
		frictionImpulse = jt * tangent;
	else {
		float df = 0.15f; //dynamic friction. Once object is moving, slightly less friction applies.
		frictionImpulse = -j * tangent * df;
	}


	objA->velocity -= objA->invMass * frictionImpulse;
	objB->velocity += objB->invMass * frictionImpulse;
}



void Contact::correctPenetration() {
	const float k_slop = 0.05f; // Penetration allowance
	const float percent = 0.4f; // Penetration percentage to correct

	//TO DO: kludge, should really find largest penetration probably.
	float penetration = points[0].penetration;

	glm::vec3 correction = (std::max(penetration - k_slop, 0.0f) / (objA->invMass + objB->invMass)) * normal * percent;
	objA->updateModelPosition(-correction * objA->invMass);
	objB->updateModelPosition(correction * objB->invMass);
}

ContactKey::ContactKey(CPhysObj2* a, CPhysObj2* b) {
	if (a < b) {
		objA = a;
		objB = b;
	}
	else {
		objA = b;
		objB = a;
	}
}

bool ContactKey::operator <( const ContactKey& a2) const {
	if (objA < a2.objA)
		return true;

	if (objA == a2.objA && objB < a2.objB)
		return true;

	return false;
}


