#include "terrainPhysObj.h"
#include "collision.h"


#include "..\watch.h"
#include <iostream> //for cerr

using namespace glm;

void CTerrainPhysObj::collisionCheck2(CBasePhysObj& collider) {
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
/*	for (int v = 0; v < noTris * 3; v += 3) {
		int intersect = triSphereIntersection(collider.bSphere, pBuf[v].v, pBuf[v + 1].v, pBuf[v + 2].v, p);
		if (intersect) {
			//let's create a contact!
			intersections = true;
			//find tri normal
			vec3 a = pBuf[v + 1].v - pBuf[v].v;
			vec3 b = pBuf[v + 2].v - pBuf[v].v;
			triNorm = normalize(cross(a, b));




			//find vector from sphere centre to contact point
			contactDir = p - collider.bSphere.centre;
			//create radius-length vector in same direction
			vec3 rad = glm::normalize(contactDir) * collider.bSphere.radius;
			vec3 diff = rad - contactDir;
			float penetration = length(diff);
			if (penetration > maxPenetration) {
				maxPenetration = penetration;
			}

			if (p.y > collider.bSphere.centre.y)
				std::cerr << "\n wrong side penetration of " << penetration;
			if (contactCount < 5) {
				pManager->addContact(&collider, NULL, vec3(0, 1, 0), restitution, penetration);
				contactDir = -normalize(collider.velocity);

		

				vec3 reflection = collider.velocity - (2.0f * triNorm* dot(triNorm, collider.velocity));
				reflection = normalize(reflection);

				//pManager->addContact(&collider, NULL, triNorm, restitution, penetration);
				//^^^^^just about works
				contactCount++;
			}

		}
	} */

	if (!intersections) { // see if we  tunnelled through a triangle
	
		float a, b, c;

		for (int v = 0; v < noTris * 3; v += 3) {
			int intersect = triLineIntersection(colliderPos, projectedPos, pBuf[v].v, pBuf[v + 1].v, pBuf[v + 2].v,
				a,b,c);
			 
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
	}


	return;
}


void CTerrainPhysObj::collisionCheck(CBasePhysObj& collider) {
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

unsigned int CTerrainPhysObj::chunkCheck(glm::vec3 & start, glm::vec3 & end, TChunkVert *& pBuf) {
	float smallestSCsize =  pTerrain->cubesPerChunkEdge * pTerrain->LoD1cubeSize;
	vec3 searchVec = normalize(end - start) * smallestSCsize;
	vec3 checkPos = start;
	float searchLength = length(end - start);
	unsigned int noTris = 0; 
	while (noTris == 0) {
		pTerrain->getTris(checkPos, pBuf, noTris);
		cerr << " tris found: " << noTris;
		checkPos += searchVec;
		if (length(start - checkPos) > searchLength)
			return noTris;
	}
	return noTris;
}
