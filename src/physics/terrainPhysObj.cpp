#include "terrainPhysObj.h"

glm::vec3 CTerrainPhysObj::collisionCheck(glm::vec3 pos) {
	return glm::vec3();
}

void CTerrainPhysObj::attachModel(CModel * model) {
	pTerrain = (CTerrain*)model;
	pModel = model;
}

glm::vec3 CTerrainPhysObj::update(const float & dT) {
	return glm::vec3(0);
}
