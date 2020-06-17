#pragma once

#include "glm/glm.hpp"

//#include "physics2/physObj2.h"


struct TContactPoint {
	glm::vec3 position;
	float penetration;
};

class CPhysObj2;
class Contact {
public:
	Contact();
	void applyImpulse();
	void correctPenetration();


	glm::vec3 normal;
	int numPoints;
	TContactPoint points[4];

	CPhysObj2* objA;
	CPhysObj2* objB;
};



class ContactKey {
public:
	ContactKey(CPhysObj2* a, CPhysObj2* b);
	bool operator < (const ContactKey& a2) const;

	CPhysObj2* objA;
	CPhysObj2* objB;
};

//inline bool operator < (const ContactKey& a1, const ContactKey& a2)
//{
//	if (a1.objA < a2.objA)
//		return true;
//
//	if (a1.objA == a2.objA && a1.objB < a2.objB)
//		return true;
//
//	return false;
//}