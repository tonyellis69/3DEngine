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

void contact::resolve(float dT) {
	resolveVelocity(dT);
	resolvePenetration(dT);
}

void contact::resolveVelocity(float dT) {

	//find the velocity in the direction of the contact
	float separatingVelocity = calcSeparatingVelocity();

	if (separatingVelocity > 0) //objects already heading apart
		return;


	//adjust for springiness of the contact
	float newSeparatingVelocity = -separatingVelocity * restitution;

	
	// Check the velocity build-up due to acceleration only.
	vec3 accCausedVelocity = collider->acceleration;
	if (collidee) 
		accCausedVelocity -= collidee->acceleration;
	float accCausedSepVelocity = glm::dot(accCausedVelocity,contactNormal) * dT;
	// If we’ve got a closing velocity due to acceleration build-up,
	// remove it from the new separating velocity.
	if (accCausedSepVelocity < 0)
	{
		newSeparatingVelocity += restitution * accCausedSepVelocity;
		// Make sure we haven’t removed more than was
		// there to remove.
		if (newSeparatingVelocity < 0) 
			newSeparatingVelocity = 0;
	}
	
	

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

	collider->setVelocity(collider->velocity + impulsePerUnitMass * collider->inverseMass);

	if (collidee) {
		// goes in the opposite direction.
		collidee->setVelocity(collidee->velocity + impulsePerUnitMass * -collidee->inverseMass);
	}

}

void contact::resolvePenetration(float dT) {
	// If we don't have any penetration, skip this step.
	if (penetration <= 0) 
		return;

	// The movement of each object is based on their inverse mass, so total that.
	float totalInverseMass = collider->inverseMass;
	if (collidee) 
		totalInverseMass += collidee->inverseMass;

	// If all particles have infinite mass, then we do nothing
	if (totalInverseMass <= 0) 
		return;

	// Find the amount of penetration resolution per unit of inverse mass
	vec3 movePerIMass = contactNormal * (penetration / totalInverseMass);

	// Calculate the the movement amounts
	colliderMovement = movePerIMass * collider->inverseMass;
	if (collidee) {
		collideeMovement = movePerIMass * -collidee->inverseMass;
	}
	else {
		collideeMovement = vec3(0);
	}

	// Apply the penetration resolution
	collider->position = collider->position + colliderMovement;
	if (collidee) {
		collidee->position = collidee->position + collideeMovement;
	}
}
