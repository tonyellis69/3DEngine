#pragma once

#include <vector>

#include "physModel.h"

/** The definitive physics engine. */
class CPhysEng {
public:
	CPhysEng();
	void addObj(CPhysObj2* physObj);
	void update(float dT);


private:
	void broadphase();
	void checkCollision(CPhysObj2* objA, CPhysObj2* objB);

	std::vector<CPhysObj2*> objs;

};