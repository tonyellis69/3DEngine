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
}

CBasePhysObj * CPhysObjManager::addModel(CModel * model) {
	CPhysObj* obj = new CPhysObj();
	obj->attachModel(model);
	addPhysObj(obj);
	obj->pManager = this;
	return obj;
}

void CPhysObjManager::addPhysObj(CBasePhysObj * obj) {
	physObjList.push_back(obj);
	((CPhysObj*)obj)->pManager = this;
}


/** Update the physics of all registered physics objects. */
void CPhysObjManager::update(const float & dT) {

	tmp = tmp + dT;
	watch::watch1 << tmp;

	CPhysObjManager::dT = dT;
	integrateObjects();

	collisionCheck();

	contactResolver();

	
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
	CBasePhysObj* collider, *collidee;
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		collider = physObjList[obj];
		if (!collider->collides)
			//2. maybe make this check later, and swap objects accordingly
			continue;
		for (size_t obj2 = 0; obj2 < physObjList.size(); obj2++) {
			collidee = physObjList[obj2];
			//TO DO: more elegant way to compare each pair only once?
			//remove an object after comparing it to every other, then go on to the next?
			if (collidee == collider)
				continue;
			//check for collision between this pair, create contact if so
			collidee->collisionCheck(*collider);
		}
	}
}


void CPhysObjManager::contactResolver() {

	maxIterations = contactList.size(); //TO DO: should be able to set this
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


		//exit if that's still > 0

		//run the collision response algorith for the lowest separating velocity


		// Resolve this contact.
		contactList[maxIndex].resolve();

		contactList.erase(contactList.begin() + maxIndex);
	
		currentIteration++;
	}

}

void CPhysObjManager::addContact(CBasePhysObj * collider, CBasePhysObj * collidee, glm::vec3& contactNormal, float restitution) {
	contact newContact(collider, collidee);
	newContact.contactNormal = contactNormal;
	newContact.restitution = restitution;
	contactList.push_back(newContact);
}
