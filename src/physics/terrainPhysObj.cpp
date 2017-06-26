#include "terrainPhysObj.h"
#include "collision.h"


#include "..\watch.h"
#include <iostream> //for cerr

using namespace glm;

void CTerrainPhysObj::collisionCheck(CBasePhysObj& collider) {
	float restitution = 0.1f;
	float roundingError = 1.0f; //At least, I think that's what it is.
	CAABB* aabb = &collider.AABB;
	aabb->setPos(collider.position);
	vec3 contactDir, finalContactDir; 
	float maxPenetration = 0; float penetration;
	//for each diagonal segment of the AABB, see if it's in an occupied chunk
	for (int cornerNo = 4; cornerNo < 8; cornerNo++) {
		penetration = checkDiagonal(aabb->corner[cornerNo], aabb->corner[7 - cornerNo], contactDir);
		if (penetration > maxPenetration) {
			maxPenetration = penetration;
			finalContactDir = contactDir;
		}
	}

	if (maxPenetration) {
			pManager->addContact(&collider, NULL, vec3(0,1,0) /*finalContactDir*/, restitution, maxPenetration - roundingError);
			return;
	}

	//if we're here, we're either above terrain or tunnelled right through it
	

	//for each bottom corner plot a line in the reverse-velocity direction to find a chunk, use that
	for (int cornerNo = 4; cornerNo < 8; cornerNo++) {
		vec3 cornerOffset = aabb->corner[cornerNo] - collider.position;
		vec3 backProjectedPos = collider.lastContact + cornerOffset;

		penetration = checkTunnellingLine(aabb->corner[cornerNo], backProjectedPos, contactDir);

		if (penetration > maxPenetration) {
			maxPenetration = penetration;
			finalContactDir = contactDir;
		}
	}

	if (maxPenetration) {
		pManager->addContact(&collider, NULL, vec3(0, 1, 0) /*contactDir*/, restitution, maxPenetration - roundingError);
		return;
	}

	//still here? we're probably just falling above terrain then
	collider.lastContact = collider.position; //update where we start checking for tunnelling
}

void CTerrainPhysObj::collisionCheckLine(CBasePhysObj& collider) {
	//from the collider's current position to its bounding sphere position, does it pass through a non-empty chunk?
	//if so, check for a collision with the triangles of this chunk.
	TChunkVert* pBuf = NULL;
	vec3 colliderPos = collider.position;
	vec3 projectedPos = colliderPos + normalize(collider.velocity) * collider.bSphere.radius;
	unsigned int noTris = chunkCheck(collider.lastContact, projectedPos, pBuf);

	if (noTris == 0)
		return;
	std::cerr << "\nIn a populated chunk... velocity " << collider.velocity.x << " " << collider.velocity.y
		<< " " << collider.velocity.z;
	collider.bSphere.setCentre(colliderPos);
	float restitution = 0.1f;
	std::cerr << "\nCentre pos at " << colliderPos.x << " " << colliderPos.y << " " << colliderPos.z;

	bool hit = false;
	//test pos agains these triangles
	//for every vert in the buffer
	vec3 vert; vec3 p; float maxPenetration = 0; vec3 contactDir; vec3 triNorm;
	int contactCount = 0; bool intersections = false;


	// see if we  tunnelled through a triangle

	float a, b, c;

	for (int v = 0; v < noTris * 3; v += 3) {
		int intersect = triLineIntersection(colliderPos, projectedPos, pBuf[v].v, pBuf[v + 1].v, pBuf[v + 2].v,
			a, b, c);

		if (intersect) {
			vec3 intersectionPoint = (pBuf[v].v * a) + (pBuf[v + 1].v * b) + (pBuf[v + 2].v * c);
			std::cerr << "\nIntersection! Tri " << v << ", y: " << intersectionPoint.y << " penetrating point y: " << projectedPos.y;
			//which side of the triangle will the current velocity put the leading point of the bounding sphere?
			vec3 a = pBuf[v + 1].v - pBuf[v].v;
			vec3 b = pBuf[v + 2].v - pBuf[v].v;
			triNorm = normalize(cross(a, b));
			vec3 test = normalize(projectedPos - intersectionPoint);
			float compare = dot(triNorm, test);
			std::cerr << "\ncomparison: " << compare;
			//if (compare > 0) {//TO DO: see if we can return here. No other triangle should be in contention
			if (compare > 0) {
				std::cerr << "\nRejecting for being in front of triangle";
				continue;
			}
			std::cerr << "\nTunnelling detected! Tri " << v;
			watch::watch2 << "hit";

			//create a contact
			//project back along our tunnelling line - in this case, velocity
			contactDir = glm::normalize(-collider.velocity);

			//contactDir = -normalize(collider.velocity);

			float penetration = glm::length(intersectionPoint - colliderPos);
			penetration += collider.bSphere.radius;

			//	penetration = intersectionPoint.y - projectedPos.y;

			penetration = glm::length(intersectionPoint - projectedPos);

			pManager->addContact(&collider, NULL, contactDir, restitution, penetration);
		}

		//no intersection? Then move our start point for the next search out of this populated chunk
		collider.lastContact = colliderPos;
	}



	return;
}


void CTerrainPhysObj::collisionCheckSphere(CBasePhysObj& collider) {
	//from the collider's current position to its bounding sphere position, does it pass through a non-empty chunk?
	//if so, check for a collision with the triangles of this chunk.
	TChunkVert* pBuf = NULL;
	vec3 colliderPos = collider.position;
	vec3 projectedPos = colliderPos + normalize(collider.velocity) * collider.bSphere.radius;
	unsigned int noTris = chunkCheck(collider.lastContact, projectedPos, pBuf);

	if (noTris == 0) {
		std::cerr << "\no tris found, returning";
		return;
	}
	std::cerr << "\n\nIn a populated chunk... velocity " << collider.velocity.x << " " << collider.velocity.y
		<< " " << collider.velocity.z;
	collider.bSphere.setCentre(colliderPos);
	float restitution = 0.1f;

	std::cerr << "\nCentre pos at " << colliderPos.x << " " << colliderPos.y << " " << colliderPos.z;
	bool hit = false;
	//test pos agains these triangles
	//for every vert in the buffer
	vec3 vert; vec3 p; float maxPenetration = 0; vec3 contactDir; vec3 triNorm; float penetration;
	int contactCount = 0; bool intersections = false; int triNo = 0;
	for (int v = 0; v < noTris * 3; v += 3) {
		int intersect = triSphereIntersection(collider.bSphere, pBuf[v].v, pBuf[v + 1].v, pBuf[v + 2].v, p);
		if (intersect) {
			std::cerr << "\ntri-sphere intersection found, tri " << v << " "
				<< "intersection y is " << p.x << " " << p.y << " " << p.z;
			intersections = true;
			//see if this is the deepest penetration so far	
			//find distance of triangle plane from sphere centre
			//find plane normal
			vec3 a = pBuf[v + 1].v - pBuf[v].v;
			vec3 b = pBuf[v + 2].v - pBuf[v].v;
			vec3 planeNorm = normalize(cross(a, b));
			vec3 diff = p - colliderPos;
			//vec3 diff = pBuf[v].v - colliderPos;
			std::cerr << "\np - colliderPos = " << diff.x << " " << diff.y << " " << diff.z;
			std::cerr << " length: " << length(diff);
			
			std::cerr << "\nplaneNorm = " << planeNorm.x << " " << planeNorm.y << " " << planeNorm.z;
			float planeDist = dot(planeNorm,diff);

			std::cerr << "\nplaneDist  = " << planeDist;

			//which side of tri are we on?
			if (planeDist > 0)
				penetration = (collider.bSphere.radius + length(diff));
			else
				penetration = (collider.bSphere.radius - length(diff));
			std::cerr << " penetration " << penetration;
			if (penetration > maxPenetration) {
				maxPenetration = penetration;
				contactDir = planeNorm;
				triNo = v;
			}
		}	
	}
	if (intersections) {
		contactDir = vec3(0, 1, 0);
		std::cerr << "\nTri " << triNo << " chosen, penetration of " << maxPenetration;
		pManager->addContact(&collider, NULL, contactDir, restitution, maxPenetration-0.1f);
		//this tiny reduction to penetration is *essential* (can be much larger). Some kind of rounding error?
		return;
	}

	//still here? let's see if we tunnelled right through a chunk
	if (!intersections) { 
		std::cerr << "\nno sphere intersections, checking for tunnelling...";
		float a, b, c;

		for (int v = 0; v < noTris * 3; v += 3) {
			int intersect = triLineIntersection(colliderPos, projectedPos, pBuf[v].v, pBuf[v + 1].v, pBuf[v + 2].v,
				a, b, c);

			if (intersect) {
				vec3 intersectionPoint = (pBuf[v].v * a) + (pBuf[v + 1].v * b) + (pBuf[v + 2].v * c);
				std::cerr << "\nIntersection! Tri " << v << ", y: " << intersectionPoint.y << " penetrating point y: " << projectedPos.y;
				//which side of the triangle will the current velocity put the leading point of the bounding sphere?
				vec3 a = pBuf[v + 1].v - pBuf[v].v;
				vec3 b = pBuf[v + 2].v - pBuf[v].v;
				triNorm = normalize(cross(a, b));
				vec3 test = normalize(projectedPos - intersectionPoint);
				float compare = dot(triNorm, test);
				std::cerr << "\ncomparison: " << compare;
				//if (compare > 0) {//TO DO: see if we can return here. No other triangle should be in contention
				if (compare > 0) {
					std::cerr << "\nRejecting for being in front of triangle";
					continue;
				}
				std::cerr << "\nTunnelling detected! Tri " << v;
				watch::watch2 << "hit";

				//create a contact
				//project back along our tunnelling line - in this case, velocity
				contactDir = glm::normalize(-collider.velocity);

				//contactDir = -normalize(collider.velocity);

				float penetration = glm::length(intersectionPoint - colliderPos);
				penetration += collider.bSphere.radius;

				//	penetration = intersectionPoint.y - projectedPos.y;

				penetration = glm::length(intersectionPoint - projectedPos);

				pManager->addContact(&collider, NULL, contactDir, restitution, penetration);
				std::cerr << "\ncontact added, returning";
			}

			//no intersection? Then move our start point for the next search out of this populated chunk
			collider.lastContact = colliderPos;
		}
	}


	return;
}

void CTerrainPhysObj::attachModel(CModel * model) {
	pTerrain = (CTerrain*)model;
	pModel = model;
}

glm::vec3 CTerrainPhysObj::update(const float & dT) {
	return glm::vec3(0);
}

unsigned int CTerrainPhysObj::chunkCheck(const glm::vec3 & start, const glm::vec3 & end, TChunkVert *& pBuf) {
	float smallestSCsize =  pTerrain->cubesPerChunkEdge * pTerrain->LoD1cubeSize;
	vec3 searchVec = normalize(end - start) * smallestSCsize;
	vec3 checkPos = start;
	float searchLength = length(end - start);
	if (searchLength < 1)
		return 0;
	unsigned int noTris = 0; 
	while (noTris == 0) {
		pTerrain->getTris(checkPos, pBuf, noTris);
		//cerr << " tris found: " << noTris;
		checkPos += searchVec;
		if (length(start - checkPos) > searchLength)
			return noTris;
	}
	return noTris;
}

float CTerrainPhysObj::checkDiagonal(const glm::vec3& baseCorner,const glm::vec3& topCorner, glm::vec3& contactDir) {
	float penetration = 0; 
	TChunkVert* pBuf = NULL; unsigned int noTris = 0;
	pTerrain->getTris(baseCorner, pBuf, noTris);
	float u, v, w, t; vec3 intersectionPoint, triNorm;
	if (pBuf) { //corner in a non-empty chunk
		for (int vNo = 0; vNo < noTris * 3; vNo += 3) { //check for collision with chunk triangles
			int intersect = triSegmentIntersection(topCorner, baseCorner, pBuf[vNo].v, pBuf[vNo + 1].v, pBuf[vNo + 2].v, u, v, w, t);
			if (intersect) {
				intersectionPoint = (pBuf[vNo].v * u) + (pBuf[vNo + 1].v * v) + (pBuf[vNo + 2].v * w);
				penetration = length(baseCorner - intersectionPoint);

				vec3 a = pBuf[vNo + 1].v - pBuf[vNo].v;
				vec3 b = pBuf[vNo + 2].v - pBuf[vNo].v;
				triNorm = normalize(cross(a, b));
				break; //TO DO: may not want to do this, or at least try for 2 hits
			}
		}	
	}
	return penetration;
}


float CTerrainPhysObj::checkTunnellingLine(const glm::vec3& lineP, const glm::vec3& lineQ, glm::vec3& contactDir) {
	float penetration = 0;
	TChunkVert* pBuf = NULL; unsigned int noTris = 0;
	float u, v, w;
	vec3 intersectionPoint;

	noTris = chunkCheck(lineQ, lineP, pBuf);
	if (noTris) { //line passes through a non-empty chunk
		for (int vNo = 0; vNo < noTris * 3; vNo += 3) { //check for collision with chunk triangles
			int intersect = triLineIntersection(lineQ, lineP, pBuf[vNo].v, pBuf[vNo + 1].v, pBuf[vNo + 2].v, u, v, w);
			if (intersect) {
				intersectionPoint = (pBuf[vNo].v * u) + (pBuf[vNo + 1].v * v) + (pBuf[vNo + 2].v * w);

				//TO DO: can scrap all this if we use a tri-segment test. faster?

				//check which side of triangle the starting point of our line is on.
				vec3 a = pBuf[vNo + 1].v - pBuf[vNo].v;
				vec3 b = pBuf[vNo + 2].v - pBuf[vNo].v;
				vec3 triNorm = normalize(cross(a, b));
				vec3 test = normalize(lineP - intersectionPoint);
				float compare = dot(triNorm, test);
				if (compare > 0) { //start point in front of triangle, therefore doesn't intersect it
					break;
				}

				penetration = length(lineP - intersectionPoint);
				contactDir = normalize(intersectionPoint - lineP);
				return penetration;
			}
		}
	}

	return 0;
}
