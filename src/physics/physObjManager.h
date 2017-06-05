#pragma once

#include "physObj.h"

/** Manager of physics onjects. */
class CPhysObjManager {
public:
	CPhysObjManager() {}
	~CPhysObjManager();
	CPhysObj* addModel(CModel* model);
	void update(const float & dT);


private:
	std::vector<CPhysObj*> physObjList;
};