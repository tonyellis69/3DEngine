#pragma once

#include "glm\glm.hpp"
#include "..\terrain.h"
#include "PhysObj.h"

/** The terrain physics object. */
class CTerrainPhysObj : public CPhysObj {
public:
	glm::vec3 collisionCheck(glm::vec3& pos);
	void attachModel(CModel* model);
	glm::vec3 update(const float & dT);

	CTerrain* pTerrain;

};