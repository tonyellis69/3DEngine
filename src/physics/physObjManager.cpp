#include "physObjManager.h"
#include "physObj.h"
#include "..\watch.h"


#include <iostream> //for cerr

using namespace glm;

using namespace watch;

CPhysObjManager::~CPhysObjManager() {
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		delete physObjList[obj];
	}
	for (size_t obj = 0; obj < sceneryList.size(); obj++) {
		delete sceneryList[obj];
	}
}

CBasePhysObj * CPhysObjManager::addModel(CModel * model) {
	CPhysObj* obj = new CPhysObj();
	obj->attachModel(model);
	addPhysObj(obj);
	obj->pManager = this;
	return obj;
}

void CPhysObjManager::addPhysObj(CBasePhysObj * obj) {
	((CPhysObj*)obj)->pManager = this;
	if (obj->collides)
		physObjList.push_back(obj);
	else
		sceneryList.push_back(obj);
	
}


/** Update the physics of all registered physics objects. */
void CPhysObjManager::update(const float & dT) {

	tmp = tmp + dT;
	watch::watch1 << tmp;

	CPhysObjManager::dT = dT;
	integrateObjects();

	collisionCheck();

	contactResolver();

	/*
	for (auto it = contactList.begin(); it != contactList.end();) {
		if (it->resting && contactList.size() > 2) {
			 it = contactList.erase(it);
		}
		else {
			++it;
		}
	}
	*/
	

	watch::watch2 << contactList.size();


	
	repositionModels();
}



void CPhysObjManager::integrateObjects() {
	CBasePhysObj* physObj;
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		physObj = physObjList[obj];
		physObj->integrate(dT);
	}
}

void CPhysObjManager::repositionModels() {
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		physObjList[obj]->repositionModel();
	}
}


void CPhysObjManager::collisionCheck() {
	CBasePhysObj* collider, *sceneryObj;
	for (size_t scenery = 0; scenery < sceneryList.size(); scenery++) {
		sceneryObj = sceneryList[scenery];
		for (size_t obj = 0; obj < physObjList.size(); obj++) {
			collider = physObjList[obj];
			//check for collision between this pair, create contact if so
			sceneryObj->collisionCheck(*collider);
		}
	}
}


void CPhysObjManager::contactResolver() {

	maxIterations = 100; // contactList.size() * 3; //TO DO: should be able to set this
	int currentIteration = 0;

	while (currentIteration < maxIterations) {

		//calculate the separating velocity of each contact
		//keep track of the lowest
		float max = 0;
		unsigned maxIndex = contactList.size()-1;
		for (int i = 0; i < contactList.size(); i++)
		{
			float sepVel = contactList[i].calcSeparatingVelocity();
			if (sepVel < max)
			{
				max = sepVel;
				maxIndex = i;
			}
		}

		if (maxIndex == contactList.size() -1 ) 
			break;


		//run the collision response algorith for the lowest separating velocity


		// Resolve this contact.
		contactList[maxIndex].resolve(dT);

		// Update the interpenetrations for all particles
		vec3 colliderMove = contactList[maxIndex].colliderMovement;
		vec3 collideeMove = contactList[maxIndex].collideeMovement;
		for (int i = 0; i < contactList.size(); i++)
		{
			if (contactList[i].collider == contactList[maxIndex].collider)
			{
				contactList[i].penetration -= dot(colliderMove,contactList[i].contactNormal);
			}
			else if (contactList[i].collider == contactList[maxIndex].collidee)
			{
				contactList[i].penetration -= dot(collideeMove, contactList[i].contactNormal);
			}
			if (contactList[i].collidee)
			{
				if (contactList[i].collidee == contactList[maxIndex].collider)
				{
					contactList[i].penetration += dot(colliderMove, contactList[i].contactNormal);
				}
				else if (contactList[i].collidee == contactList[maxIndex].collidee)
				{
					contactList[i].penetration += dot(collideeMove, contactList[i].contactNormal);
				}
			}
		}

		
		
	
		currentIteration++;
	}

}

void CPhysObjManager::addContact(CBasePhysObj * collider, CBasePhysObj * collidee, glm::vec3& contactNormal, float restitution,
	float penetration) {
	contact newContact(collider, collidee);
	newContact.contactNormal = contactNormal;
	newContact.restitution = restitution;
	newContact.penetration = penetration;
	contactList.push_back(newContact);
	std::cerr << "\nNew contact " << &contactList.back() << ", penetration " << penetration << " collider " << collider;
}
