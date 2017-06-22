#pragma once

#include "basePhysObj.h"
#include "contacts.h"

#include <deque>


/** Manager of physics objects. */
class CPhysObjManager {
public:
	CPhysObjManager() { tmp = 0; maxContacts = 100; 
	nextID = 0;
	}
	~CPhysObjManager();
	CBasePhysObj* addModel(CModel* model);
	void addPhysObj(CBasePhysObj* obj);
	void update(const float & dT);
	void integrateObjects();
	void repositionModels();
	void collisionCheck();
	void contactResolver();
	void addContact(CBasePhysObj * collider, CBasePhysObj * collidee, glm::vec3& contactNormal, float restitution, float penetration);

	std::deque<contact> contactList;

private:
	std::vector<CBasePhysObj*> physObjList;
	std::vector<CBasePhysObj*> sceneryList;
	float dT; ///<Time difference since last frame.
	int maxIterations; ///<How many times per frame we try to resolve contacts.
	int maxContacts; ///<How many contacts we can track.

	double tmp;
	unsigned int nextID;
};