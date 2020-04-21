#include  "physEng.h"

CPhysEng::CPhysEng() {
	CPhysObj2::gravity = glm::vec3(0, -9.8f, 0);
}

void CPhysEng::addObj(CPhysObj2* physObj) {
	objs.push_back(physObj);
}

void CPhysEng::update(float dT) {
	//chech for collisions
	broadphase();



	//integrate forces acting on each object
	for (auto obj : objs) {
		obj->updateVelocity(dT);
	}



	//integrate velocity of each object
	for (auto obj : objs) {
		obj->updatePosition(dT);
	}

}

void CPhysEng::broadphase() {
	for (unsigned int a = 0; a < objs.size(); a++) {
		CPhysObj2* objA = objs[a];

		for (unsigned int b = a + 1; b < objs.size(); b++) {
			CPhysObj2* objB = objs[b];

			if (objA->invMass == 0 && objB->invMass == 0)
				continue;

			checkCollision(objA, objB);

		}

	}
}

/** If these two objects are colliding, record the collision/ */
void CPhysEng::checkCollision(CPhysObj2* objA, CPhysObj2* objB) {
	
	
	//AABB check - costs two matrix calcs
	TAaBB objAbb = objA->calcAABB();
	TAaBB objBbb = objB->calcAABB();

	if (objAbb.clips(objBbb)) {
		objA->invMass = 0;
		objB->invMass = 0;
	}

}
