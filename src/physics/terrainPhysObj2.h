#pragma once

#include "glm\glm.hpp"
#include "..\terrain.h"
#include "..\terrain\terrain2.h"
#include "PhysObj.h"



/** The terrain physics object. */
class CTerrainPhysObj2 : public CPhysObj {
public:
	CTerrainPhysObj2() { tmp = false; }
	void collisionCheck(CBasePhysObj& collider);
	void attachModel(CModel* model);
	glm::vec3 update(const float & dT);

	
	float checkAllWayDown(glm::vec3 & segTop, glm::vec3 & contactDir);

	float checkAllWayUp(glm::vec3 & searchBase, glm::vec3 & contactDir);

	CTerrain2* pTerrain;

	bool tmp;
};