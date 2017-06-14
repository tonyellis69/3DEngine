#include "terrainPhysObj.h"
#include "collision.h"

#include <iostream> //for cerr

using namespace glm;

glm::vec3 CTerrainPhysObj::collisionCheck(CBasePhysObj& collider) {
	//ask terrain to provide a pointer to the tris for this position.
	TChunkVert* pBuf = NULL;
	unsigned int noTris = 0;
	vec3 colliderPos = collider.position;
	pTerrain->getTris(colliderPos, pBuf, noTris);
	if (noTris == 0)
		return vec3(0);

	collider.bSphere.setCentre(colliderPos);
	float restitution = 0.5f;

	bool hit = false;
	//test pos agains these triangles
	//for every vert in the buffer
	vec3 vert; int vCount = 1; vec3 p; vec3 deepest(0);
	for (int v = 0; v < noTris * 3; v+=3) {
		int intersect = triSphereIntersection(collider.bSphere, pBuf[v].v, pBuf[v+ 1].v, pBuf[v + 2].v, p);
		//cerr << "\nintersection: " << intersect << " " << p.x << " " << p.y << " " << p.z;
		if (intersect) {
			//let's create a contact!
			//find vector from sphere centre to contact point
			vec3 contactDir = p - collider.bSphere.centre;
			contactDir = vec3(0, 1, 0);//////
			contactDir = glm::normalize(contactDir);

			pManager->addContact(&collider, NULL, contactDir, restitution);


			//create radius-length vector in same direction
			vec3 rad = glm::normalize(contactDir) * collider.bSphere.radius;
			vec3 diff = rad - contactDir;
			if (glm::length(diff) > glm::length(deepest))
				deepest = diff;
		}
 	}

	return deepest;
}

void CTerrainPhysObj::attachModel(CModel * model) {
	pTerrain = (CTerrain*)model;
	pModel = model;
}

glm::vec3 CTerrainPhysObj::update(const float & dT) {
	return glm::vec3(0);
}
