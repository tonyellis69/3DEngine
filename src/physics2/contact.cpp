#include "contact.h"

#include <algorithm>

Contact::Contact() {
	numPoints = 0;

}

void Contact::applyImpulse() {
	//calculate relative velocity of A & B
	glm::vec3 relativeVelocity = objB->velocity = objA->velocity;

	//find velocity along collision normal
	float velocityAlongNormal = glm::dot(relativeVelocity, normal);

	//Separating? Then don't resolve
	if (velocityAlongNormal > 0)
		return;

	//find the resitution
	float e = std::min(objA->restitution, objB->restitution);

	//calcualate the impulse


	//apply it to the bodies relative to their mass
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
