#include "physObjManager.h"

CPhysObjManager::~CPhysObjManager() {
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		delete physObjList[obj];
	}
}

CPhysObj * CPhysObjManager::addModel(CModel * model) {
	CPhysObj* obj = new CPhysObj();
	obj->attachModel(model);
	physObjList.push_back(obj);
	return obj;
}


/** Update the physics of all registered physics objects. */
void CPhysObjManager::update(const float & dT) {
	for (size_t obj = 0; obj < physObjList.size(); obj++) {
		physObjList[obj]->update(dT);


	}

}
