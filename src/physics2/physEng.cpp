#include  "physEng.h"

#include "utils/log.h"

CPhysEng::CPhysEng() {
	CPhysObj2::gravity = glm::vec3(0, -9.8f, 0);
}

void CPhysEng::addObj(CPhysObj2* physObj) {
	objs.push_back(physObj);
}

void CPhysEng::update(float dT) {
	//check for collisions
	broadphase();

	//integrate forces acting on each object, eg gravity
	for (auto obj : objs) {
		obj->updateVelocity(dT);
	}

	//resolution
	for (int i = 0; i < 10; i++) {
		for (auto& contact : contacts) {
			contact.second.applyImpulse();





		}


	}


	//integrate velocity of each object
	for (auto obj : objs) {
		obj->updatePosition(dT);
	}

	//correct penetration
	for (auto& contact : contacts) {
			contact.second.correctPenetration();
	}

}

void CPhysEng::broadphase() {
	for (unsigned int a = 0; a < objs.size(); a++) {
		CPhysObj2* objA = objs[a];

		for (unsigned int b = a + 1; b < objs.size(); b++) {
			CPhysObj2* objB = objs[b];

			if (objA->invMass == 0 && objB->invMass == 0)
				continue;

			Contact contact;
			//contact = checkCollision(objA, objB);
			contact = objA->checkCollision(objB);

			ContactKey key(objA, objB);

			if (contact.numPoints > 0) {
				contacts[key] = contact;
				//liveLog << "\nContact added!";

			}
			else {
				if (contacts.erase(key))
					;//	liveLog << "\nContact removed!";
			}

		}

	}
}

/** If these two objects are colliding, record the collision/ */
Contact CPhysEng::checkCollision(CPhysObj2* objA, CPhysObj2* objB) {
	Contact contact;
	if (objA < objB) {
		contact.objA = objA;
		contact.objB = objB;
	}
	else {
		contact.objB = objA;
		contact.objA = objB;
	}

	glm::vec3 baseVertB = objB->calcBaseVertPos();
	TAaBB objAbb = objA->calcAABB();

	if (objAbb.clips(baseVertB)) {
		contact.normal = glm::vec3(0, 1, 0);
		contact.numPoints = 1;
		float penetration = objAbb.AABBmax.y - baseVertB.y;
		contact.points[0] = { baseVertB, penetration };

	}

	return contact;

}
