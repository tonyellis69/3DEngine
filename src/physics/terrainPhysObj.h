#pragma once

#include "glm\glm.hpp"
#include "..\terrain.h"
#include "PhysObj.h"

/** The terrain physics object. */
class CTerrainPhysObj : public CPhysObj {
public:
	void collisionCheck(CBasePhysObj& collider);
	void collisionCheck2(CBasePhysObj& collider);
	void attachModel(CModel* model);
	glm::vec3 update(const float & dT);

	unsigned int chunkCheck(glm::vec3& start, glm::vec3& end, TChunkVert* & buf);

	CTerrain* pTerrain;

};