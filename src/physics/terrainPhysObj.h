#pragma once

#include "glm\glm.hpp"
#include "..\terrain.h"
#include "PhysObj.h"

/** The terrain physics object. */
class CTerrainPhysObj : public CPhysObj {
public:
	CTerrainPhysObj() { tmp = false; }
	void collisionCheck(CBasePhysObj& collider);
	void collisionCheckSphere(CBasePhysObj& collider);
	void collisionCheckLine(CBasePhysObj& collider);
	void attachModel(CModel* model);
	glm::vec3 update(const float & dT);

	unsigned int chunkCheck(const glm::vec3& start, const glm::vec3& end, TChunkVert* & buf);

	float checkAABBsegment(const glm::vec3 & baseCorner, const glm::vec3 & topCorner, glm::vec3 & contactDir);

	float checkTunnellingLine(const glm::vec3 & lineP, const glm::vec3 & lineQ, glm::vec3 & contactDir);

	CTerrain* pTerrain;

	bool tmp;
};