#pragma once

#include "basePhysObj.h"
#include "contacts.h"

/** Manager of physics objects. */
class CPhysObjManager {
public:
	CPhysObjManager() { tmp = 0; }
	~CPhysObjManager();
	CBasePhysObj* addModel(CModel* model);
	void addPhysObj(CBasePhysObj* obj);
	void update(const float & dT);
	void integrateObjects();
	void repositionModels();
	void collisionCheck();
	void contactResolver();
	void addContact(CBasePhysObj * collider, CBasePhysObj * collidee, glm::vec3& contactNormal, float restitution);

	std::vector<contact> contactList;

private:
	std::vector<CBasePhysObj*> physObjList;
	float dT; ///<Time difference since last frame.
	int maxIterations;

	double tmp;
};