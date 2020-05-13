#pragma once

#include <vector>
#include <map>

#include "physModel.h"
#include "contact.h"

/** The definitive physics engine. */
class CPhysEng {
public:
	CPhysEng();
	void addObj(CPhysObj2* physObj);
	void update(float dT);


private:
	void broadphase();
	Contact checkCollision(CPhysObj2* objA, CPhysObj2* objB);

	std::vector<CPhysObj2*> objs;

	std::map<ContactKey, Contact> contacts;
};