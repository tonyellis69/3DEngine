#include "terrainPhysObj.h"

#include <iostream> //for cerr

using namespace glm;

glm::vec3 CTerrainPhysObj::collisionCheck(glm::vec3& pos) {
	Chunk* chunk = pTerrain->getChunk(pos);
	if (chunk == NULL) {
		return vec3(0);
	}

	return vec3(1);
}

void CTerrainPhysObj::attachModel(CModel * model) {
	pTerrain = (CTerrain*)model;
	pModel = model;
}

glm::vec3 CTerrainPhysObj::update(const float & dT) {
	return glm::vec3(0);
}
