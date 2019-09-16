#include "terrainPhysObj2.h"
#include "collision.h"


#include "..\watch.h"
#include <iostream> //for cerr

using namespace glm;

/** Check for collisions with this terrain from an AABB collider. */
void CTerrainPhysObj2::collisionCheck(CBasePhysObj& collider) {
	collider.groundContact = true;
	

	float restitution = 0.01f; 
	float miniBounceAllowance = 10;
	float tinyPenetration = 0.001f;
	CAABB* aabb = &collider.AABB;
	aabb->setPos(collider.position);
	vec3 contactDir, maxContactDir, segBase, segTop;
	float penetration = 0;  float distance; float minDistance = FLT_MAX; vec3 minContactDir; 
	float aabbHeight = aabb->halfHeight * 2; float maxDistance = 0;

	//for each upright of the AABB, perform one or more intersection tests with the nearest column of chunks
	for (int cornerNo = 4; cornerNo < 8; cornerNo++) {

	
		segTop = aabb->corner[cornerNo - 4];
		distance = checkAllWayDown(segTop, contactDir);
	
		if (distance < minDistance) {
			minDistance = distance;
			minContactDir = contactDir;
		}		
	}

	//all four corners checked, soo...
	

	if (minDistance == FLT_MAX) { //no terrain below any corner, we must have dropped through 
	
		maxDistance = 0;
		for (int cornerNo = 4; cornerNo < 8; cornerNo++) { //for each bottom corner
			segBase = aabb->corner[cornerNo];
			distance = checkAllWayUp(segBase, contactDir); //find distance to terrain above	
			if (distance > maxDistance) {
				maxDistance = distance;
				maxContactDir = contactDir;
			}
		}

		if (maxDistance == 0) {
				std::cerr << "\n\n\n\n!!!!!!!!!!!!!!Error no terain found above or below!";
				return;
		}
		std::cerr << "\ntunnelling contact created: ";
		collider.currentContactNormal = maxContactDir;
		pManager->addContact(&collider, NULL, vec3(0, 1, 0), restitution, maxDistance);
		return;
	}



	if (minDistance < aabbHeight) { //an ordinary intersection, within the aabb
		penetration = aabbHeight - minDistance ;
		if (penetration < tinyPenetration)  //0.001f works
			penetration = 0;
		collider.currentContactNormal = minContactDir;
		pManager->addContact(&collider, NULL, vec3(0, 1, 0), restitution, penetration);
		return;
	}


	if (minDistance < (aabbHeight + miniBounceAllowance) ) { //intersection just below aabb, are we mini-bouncing?
		std::cerr << "\nIn the mini-bounce area!";
		vec3 horizontalVelocity = collider.velocity; horizontalVelocity.y = 0;
		//check to see if we're running downhill:
		if (collider.velocity.y < 0 /*&& collider.velocity.y > -3.0f */ && length(horizontalVelocity) > 0.5f) { //really crude test
			std::cerr << "\nmini-bounce fix activated!";
			collider.position.y -= (minDistance - aabbHeight);
			pManager->addContact(&collider, NULL, vec3(0, 1, 0), restitution, 0);
			collider.currentContactNormal = minContactDir;
			return;
		}
	} 

	//still here? Presumably in mid-air, then.
	collider.currentContactNormal = vec3(0);
	std::cerr << "\n@@@falling?";
	collider.groundContact = false;
}

/** Modify attachModel to provide a pointer to the CTerrain class. */
void CTerrainPhysObj2::attachModel(CModel * model) {
	pTerrain = (CTerrain2*)model;
	pModel = model;
}

glm::vec3 CTerrainPhysObj2::update(const float & dT) {
	return glm::vec3(0);
}


/** Search for a terrain intersection from the given point vertically down until we find one or exit the volume. */
float CTerrainPhysObj2::checkAllWayDown(glm::vec3& searchTop, glm::vec3& contactDir) {
	TChunkVert* pBuf = NULL; unsigned int noTris = 0; vec3 scrolledSegTop, scrolledSegBase;
	float veryFarDown = 10000.0f; float u, v, w, t; vec3 intersectionPoint; float contactDistance; vec3 triNorm;
	mat4 inverseScroll = glm::inverse(pTerrain->chunkOrigin); CSuperChunk* sc;
	vec3 searchPos = searchTop;
	scrolledSegTop = inverseScroll * vec4(searchPos, 1);
	scrolledSegBase = scrolledSegTop - vec3(0, veryFarDown, 0);
	vec3 scrolledSearchTop = scrolledSegTop;

	//for each chunk space
	///////////////////////////////////////////////////////////////
	/*
	while (sc = pTerrain->getSC(searchPos)) {
		//check for chunks
		pTerrain->getTris(searchPos, pBuf, noTris);
		if (pBuf) {
			for (size_t vNo = 0; vNo < noTris * 3; vNo += 3) { //check for collision with chunk triangles
				int intersect = triSegmentIntersection(scrolledSegTop, scrolledSegBase, pBuf[vNo].v, pBuf[vNo + 1].v, pBuf[vNo + 2].v, u, v, w, t);
				if (intersect) {
					intersectionPoint = (pBuf[vNo].v * u) + (pBuf[vNo + 1].v * v) + (pBuf[vNo + 2].v * w);
					contactDistance = length(scrolledSearchTop - intersectionPoint);

					vec3 a = pBuf[vNo + 1].v - pBuf[vNo].v;
					vec3 b = pBuf[vNo + 2].v - pBuf[vNo].v;
					triNorm = normalize(cross(a, b));
					contactDir = triNorm;
					return contactDistance;
				}
			}
		}

		//no chunk intersection found? Look further down
		//move search to the top of the next chunk down
		float chunkHeight = sc->chunkSize;
		vec3 currentChunkCorner = pTerrain->getChunkPos(searchPos);
		searchPos.y -= chunkHeight;
	}
	*/
	std::cerr << "\n!!!!No terrain found below search point!";
	return FLT_MAX;
}

/** Search for a terrain intersection from the given point up until we exit the terrain volume. */
float CTerrainPhysObj2::checkAllWayUp(glm::vec3& searchBase, glm::vec3& contactDir) {
	TChunkVert* pBuf = NULL; unsigned int noTris = 0; vec3 scrolledSegTop, scrolledSegBase;
	float veryFarUp = 10000.0f; float u, v, w, t; vec3 intersectionPoint; float contactDistance; vec3 triNorm;
	mat4 inverseScroll = glm::inverse(pTerrain->chunkOrigin); CSuperChunk* sc;
	vec3 searchPos = searchBase;
	scrolledSegBase = inverseScroll * vec4(searchPos, 1);
	scrolledSegTop = scrolledSegBase + vec3(0, veryFarUp, 0);
	vec3 scrolledSearchBase = scrolledSegBase;

	//while (searchPos.y < abs(pTerrain->shells[0].nwLayerPos.y)) {
	/////////////////////////////////////////////////////////
	/*
	while (sc = pTerrain->getSC(searchPos)) {
		pTerrain->getTris(searchPos, pBuf, noTris);
		if (pBuf) {
			for (size_t vNo = 0; vNo < noTris * 3; vNo += 3) { //check for collision with chunk triangles
				int intersect = triSegmentIntersection(scrolledSegTop, scrolledSegBase, pBuf[vNo].v, pBuf[vNo + 1].v, pBuf[vNo + 2].v, u, v, w, t);
				if (intersect) {
					intersectionPoint = (pBuf[vNo].v * u) + (pBuf[vNo + 1].v * v) + (pBuf[vNo + 2].v * w);
					contactDistance = length(scrolledSearchBase - intersectionPoint);
					

					vec3 a = pBuf[vNo + 1].v - pBuf[vNo].v;
					vec3 b = pBuf[vNo + 2].v - pBuf[vNo].v;
					triNorm = normalize(cross(a, b));
					contactDir = triNorm;
					return contactDistance;
				}
			}
		}

		//no chunk intersection found? Look further up
		//move search to the top of the next chunk down
	
		float chunkHeight = sc->chunkSize;
		vec3 currentChunkCorner = pTerrain->getChunkPos(searchPos);
		searchPos.y += chunkHeight;
	}
	*/
	std::cerr << "\n????No terrain found above search point!";
	return 0;
}