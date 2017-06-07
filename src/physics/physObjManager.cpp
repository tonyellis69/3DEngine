#include "physObjManager.h"
#include "physObj.h"

using namespace glm;

CPhysObjManager::~CPhysObjManager() {
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		delete physObjList[obj];
	}
}

CBasePhysObj * CPhysObjManager::addModel(CModel * model) {
	CPhysObj* obj = new CPhysObj();
	obj->attachModel(model);
	addPhysObj(obj);
	return obj;
}

void CPhysObjManager::addPhysObj(CBasePhysObj * obj) {
	physObjList.push_back(obj);
}


/** Update the physics of all registered physics objects. */
void CPhysObjManager::update(const float & dT) {
	vec3 newVelocity, response;
	CBasePhysObj* physObj;
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		physObj = physObjList[obj];
		newVelocity = physObj->update(dT);

		if (physObj->collides) {
			response = checkForCollisions(physObj, newVelocity);
			newVelocity = response;
		}

		physObj->setVelocity(newVelocity);
		physObj->applyVelocity();
	}

}

vec3 CPhysObjManager::checkForCollisions(CBasePhysObj * collider, glm::vec3 & velocity) {
	vec3 response; CBasePhysObj* collidee;
	vec3 pos = collider->getModel()->getPos() + velocity;
	//iterate through physics objects, seeing if we collide with any
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		collidee = physObjList[obj];
		if (collidee == collider)
			continue;	
		response = collidee->collisionCheck(pos);
		if (response.y == 0)
			return velocity;
		else
			return vec3(0);
	}

	return velocity;
}
