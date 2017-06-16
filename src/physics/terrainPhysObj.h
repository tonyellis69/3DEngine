#pragma once

#include "glm\glm.hpp"
#include "..\terrain.h"
#include "PhysObj.h"

/** The terrain physics object. */
class CTerrainPhysObj : public CPhysObj {
public:
	void collisionCheck(CBasePhysObj& collider);
	void attachModel(CModel* model);
	glm::vec3 update(const float & dT);

	CTerrain* pTerrain;

};