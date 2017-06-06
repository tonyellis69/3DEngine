#pragma once

#include "basePhysObj.h"

/** Manager of physics objects. */
class CPhysObjManager {
public:
	CPhysObjManager() {}
	~CPhysObjManager();
	CBasePhysObj* addModel(CModel* model);
	void addPhysObj(CBasePhysObj* obj);
	void update(const float & dT);
	glm::vec3 checkForCollisions(CBasePhysObj* collider, glm::vec3& velocity);


private:
	std::vector<CBasePhysObj*> physObjList;
};