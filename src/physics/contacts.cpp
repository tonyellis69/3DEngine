#include "contacts.h"
#include "glm\glm.hpp"

#include <iostream>

using namespace glm;

contact::contact(CBasePhysObj * collider, CBasePhysObj* collidee) {
	contact::collider = collider;
	contact::collidee = collidee;
	resolved = false;
	resting = false;
}

/** Return the separating velocity between two objects. The direction is dictated by the line between the objects:
	separating objects have a positive velocity, closing objects have negative. */
float contact::calcSeparatingVelocity() {
	vec3 relativeVelocity = collider->velocity;
	if (collidee)
		relativeVelocity -= collidee->velocity;
	return dot(relativeVelocity, contactNormal);
}

void contact::resolve(float dT) {
	resolveVelocity(dT);
	resolvePenetration(dT);
	//if (penetration < -1)
	//	resolved = true;
	std::cerr << "\n resolved " << id << " had a penetration of " << penetration;
}

void contact::resolveVelocity(float dT) {

	//find the velocity along the direction of the contact
	float separatingVelocity = calcSeparatingVelocity(); 
	// eg, -19 for a 0,-19,0 velocity impact with the ground

	if (separatingVelocity > 0) {//objects heading apart, collision already resolved
		std::cerr << "\n" << id << " now separating";
		//resolved = true;
		return;
	}

	//adjust for springiness of the contact
	float newSeparatingVelocity = -separatingVelocity * restitution;
	//small, positive (1.7) for an impact with the ground

	
	//Code to prevent vibrations caused by illegal acceleration in a resting object
	vec3 accCausedVelocity = collider->acceleration;
	if (collidee) 
		accCausedVelocity -= collidee->acceleration;
	float accCausedSepVelocity = glm::dot(accCausedVelocity,contactNormal) * dT;
	//only caused by 'boring into' collidee between frames - wouldn't happen in real life

	// If we’ve got a closing velocity due to acceleration build-up,
	// remove it from the new separating velocity.
	if (accCausedSepVelocity < 0)
	{
		newSeparatingVelocity += restitution * accCausedSepVelocity;
		//removes from the separating velocity any (small) amount caused by acceleration build-up

		std::cerr << "\n" << id << " now considered at rest";
		resting = true;
		// Make sure we haven’t removed more than was
		// there to remove.
		if (newSeparatingVelocity < 0) 
			newSeparatingVelocity = 0;
	}
	
	

	//find change in velocity
	float dVelocity = newSeparatingVelocity - separatingVelocity;
	//eg, positive, slightly larger value (20) than collision velocity

	//apply velocity to colliding objects in proportion to their mass
	float totalInverseMass = collider->inverseMass;
	if (collidee) 
		totalInverseMass += collidee->inverseMass;

	if (totalInverseMass <= 0) //objects have infinite mass. nothing happening here
		return;

	float impulse = dVelocity / totalInverseMass;
	//big positive value here, eg, 200

	vec3 impulsePerUnitMass = contactNormal * impulse;
	//...big velocity in opposite direction to object's colliding velocity

	// Apply impulses: in the direction of the contact, proportional to the inverse mass.
	collider->setVelocity(collider->velocity + impulsePerUnitMass * collider->inverseMass);
	//small velocity (1.7) in opposite direction to original colliding velocity

	vec3 debugVar = impulsePerUnitMass * collider->inverseMass;
	std::cerr << "\nImpulse of " << debugVar.x << " " << debugVar.y << " " << debugVar.z;
	std::cerr << "\changed velocity to " << collider->velocity.x << " " << collider->velocity.y << " " << collider->velocity.z;

	if (collidee) {
		// goes in the opposite direction.
		collidee->setVelocity(collidee->velocity + impulsePerUnitMass * -collidee->inverseMass);
	}

}

/** Adjust the position of the objects in this contact to remove any interpenetration. 
	NB: the penetration member itself is not reset here, but by the calling process. */
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
	//for an object colliding with scenery it will be the full penetration length in the contact normal direction
	colliderMovement = movePerIMass * collider->inverseMass;
	if (collidee) {
		collideeMovement = movePerIMass * -collidee->inverseMass;
	}
	else {
		collideeMovement = vec3(0);
	}

	// Apply the penetration resolution
	std::cerr << "\nposition moved from " << collider->position.x << " " << collider->position.y << " "
		<< collider->position.z;

	collider->position = collider->position + colliderMovement ;
	if (collidee) {
		collidee->position = collidee->position + collideeMovement;
	}
	std::cerr << "\nto " << collider->position.x << " " << collider->position.y << " "
		<< collider->position.z << " to solve penetration of " << penetration;
}
