#include "contacts.h"
#include "glm\glm.hpp"

#include <iostream>

using namespace glm;

contact::contact(CBasePhysObj * collider, CBasePhysObj* collidee) {
	contact::collider = collider;
	contact::collidee = collidee;
}

float contact::calcSeparatingVelocity() {
	vec3 relativeVelocity = collider->velocity;
	if (collidee)
		relativeVelocity -= collidee->velocity;
	return dot(relativeVelocity, contactNormal);
}

void contact::resolve() {
	resolveVelocity();
}

void contact::resolveVelocity() {

	//find the velocity in the direction of the contact
	float separatingVelocity = calcSeparatingVelocity();
	//TO DO: do we need to recalculate this? Check

	if (separatingVelocity > 0) //objects already heading apart
		return;

	//adjust for springiness of the contact
	float newSeparatingVelocity = -separatingVelocity * restitution;

	//find change in velocity
	float dVelocity = newSeparatingVelocity - separatingVelocity;

	//apply velocity to colliding objects in proportion to their mass
	float totalInverseMass = collider->inverseMass;
	if (collidee) 
		totalInverseMass += collidee->inverseMass;

	if (totalInverseMass <= 0) //objects have infinite mass. nothing happening here
		return;

	float impulse = dVelocity / totalInverseMass;

	vec3 impulsePerUnitMass = contactNormal * impulse;

	// Apply impulses : they are applied in the direction of the contact,
		// and are proportional to the inverse mass.
	std::cerr << "\ncollider velocity " << collider->velocity.x << " " << collider->velocity.y << " " <<
		collider->velocity.z;
	collider->setVelocity(collider->velocity + impulsePerUnitMass * collider->inverseMass);

	std::cerr << "\nchanged to " << collider->velocity.x << " " << collider->velocity.y << " " <<
		collider->velocity.z;

	if (collidee) {
		// goes in the opposite direction.
		collidee->setVelocity(collidee->velocity + impulsePerUnitMass * -collidee->inverseMass);
	}

}
