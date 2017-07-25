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

CBasePhysObj * CPhysObjManager::addModel(C3dObject * model) {
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

	
	for (auto it = contactList.begin(); it != contactList.end();) {
		if (it->resolved) {
			std::cerr << "\nContact " << it->id << " erased.";
			 it = contactList.erase(it);
		}
		else {
			++it;
		}
	}
	
	

	watch::watch2 << contactList.size();


	
	repositionModels();
}



void CPhysObjManager::integrateObjects() {
	CBasePhysObj* physObj;
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		physObj = physObjList[obj];
			if (!physObj->asleep) {
		//	physObj->moveToModelPosition(); //TO DO: why are we doing this every frame?
			//this is breaking physics to serve terrain repositioning. Move phys object explicitly,
			//when you move the model, and do it outside the phusics loop!
			physObj->integrate(dT);

		}
	}
}

void CPhysObjManager::repositionModels() {
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		physObjList[obj]->repositionModel();
	}
}


void CPhysObjManager::collisionCheck() {
	CBasePhysObj* collider, *sceneryObj;
	//check for collisions with sceneru:
	for (size_t scenery = 0; scenery < sceneryList.size(); scenery++) {
		sceneryObj = sceneryList[scenery];
		for (size_t obj = 0; obj < physObjList.size(); obj++) {
			collider = physObjList[obj];
			if (!collider->asleep)
				sceneryObj->collisionCheck(*collider);
		

		}
	}
}


void CPhysObjManager::contactResolver() {
	if (!contactList.size())
		return;

	maxIterations = 10; // contactList.size() * 3; //TO DO: should be able to set this
	int currentIteration = 0;

	while (currentIteration < maxIterations) {
	//	std::cerr << "\niteration: " << currentIteration;
		//calculate the separating velocity of each contact
		//keep track of the lowest
		float max = 0;
		int maxIndex = contactList.size()-1;
		for (int i = 0; i < contactList.size(); i++)
		{
			float sepVel = contactList[i].calcSeparatingVelocity(); 
			if (sepVel < max &&
				(sepVel < 0 || contactList[i].penetration > 0))
			{
				max = sepVel;
				maxIndex = i;
			}
		}

	//	if (maxIndex == contactList.size() -1 ) 
		//	break;

		if (contactList[maxIndex].collider->asleep)
			break;

		//we run the collision response algorith for the lowest separating velocity, ie, the most negative,
		//where the objects are therefore *not* separating but colliding
		contactList[maxIndex].resolve(dT);

		// Update the interpenetrations for all contacts the particles of this contact are involved in
		vec3 colliderMove = contactList[maxIndex].colliderMovement;
		vec3 collideeMove = contactList[maxIndex].collideeMovement;
		for (int i = 0; i < contactList.size(); i++)
		{
			if (contactList[i].collider == contactList[maxIndex].collider )
			{
				std::cerr << "\nContact " << contactList[i].id << " penetration reset from " << contactList[i].penetration;
				contactList[i].penetration -= dot(colliderMove,contactList[i].contactNormal);
				std::cerr << " to " << contactList[i].penetration;
				if (contactList[i].penetration < 0.1f) //was 0.1f
					contactList[i].resolved = true;
				//	contactList[i].penetration = 0;
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



		////////////////////////////////////
		//My fix to prevent the penetration distance of resting contacts being pushed endlessly into lower negative numbers
	//	contactList[maxIndex].colliderMovement = vec3(0);
		//contactList[maxIndex].collideeMovement = vec3(0);
		/////////////////////////////////////
		//contactList[maxIndex].resolved = true;
	
		currentIteration++;
	}


}

void CPhysObjManager::addContact(CBasePhysObj * collider, CBasePhysObj * collidee, glm::vec3& contactNormal, float restitution,
	float penetration) {
	
	contact newContact(collider, collidee);
	newContact.contactNormal = contactNormal;
	newContact.restitution = restitution;
	newContact.penetration = penetration;
	newContact.id = nextID++;
	contactList.push_back(newContact);
	collider->lastContact = collider->position;
	if (contactList.size() > maxContacts)
		contactList.pop_front();
	std::cerr << "\nNew contact " << newContact.id << ", penetration " << penetration << " collider " << collider;
}
