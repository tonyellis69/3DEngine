#include "hex.h"

#include <cmath>
#include <glm/gtx/rotate_vector.hpp>

#include "..\utils\log.h"


CHex::CHex() {
	x = 0; y = 0; z = 0;
}

CHex::CHex(int n) {
	if (n == -1) { //special negated hex
		x = -1; y = -1; z = -1;
	}
	else {
		x = n; z = n; y = -x - z;
	}
}

/** Construct from cube coordinates. */
CHex::CHex(int x, int y, int z) {
	this->x = x; this->y = y; this->z = z;
}

/** Construct from axial coordinates. */
CHex::CHex(int q, int r) {
	x = q;
	z = r;
	y = -x - z;
}

CHex CHex::operator+(CHex& hex2) {
	return CHex(x + hex2.x, y + hex2.y, z + hex2.z);
}

CHex CHex::operator+(glm::i32vec3& hex2) {
	return CHex(x + hex2.x, y + hex2.y, z + hex2.z);
}

CHex CHex::operator-(CHex& hex2) {
	return CHex(x - hex2.x, y - hex2.y, z - hex2.z);
}

bool CHex::operator==(const CHex& hex2) const {
	return (x == hex2.x && y == hex2.y && z == hex2.z);
}

bool CHex::operator!=(CHex& hex2) {
	return (x != hex2.x) || (y != hex2.y) || (z != hex2.z);
}

/** Construct from a worldspace location.*/
CHex::CHex(glm::vec3& worldSpace) {
	

}

/** Return position in axial coordiantes. */
glm::i32vec2 CHex::getAxial() {
	return glm::i32vec2(x,z);
}

glm::i32vec3 CHex::getCubeVec()
{
	return glm::i32vec3(x,y,z);
}




/**Convert cube coordinates to axial.*/
CHex cubeToAxial(CHex & cube) {
	return CHex(cube.x, cube.z);
}

/**Convert axial coordinates to cube (float version). */
glm::vec3 axialToCube(float q, float r) {
	float x = q;
	float z = r;
	float y = -x - z;
	return glm::vec3(x, y, z);
}

/** Convert cube coordinates to odd-row offset coordinates.*/
glm::i32vec2 cubeToOffset(CHex& hex) {
	return glm::i32vec2(hex.x + (hex.z - (hex.z & 1)) / 2, hex.z);
}

/** Convert axial coordinates to odd-row offset coordinates. */
glm::i32vec2 axialToOffset(int q, int r) {
	return glm::i32vec2(q + (r - (r & 1)) / 2, r);
}

/** Convert cube coordinates to a point in worldspace. */
glm::vec3 cubeToWorldSpace(CHex& hex) {
	//float x = (hex.x + (-0.5f * (hex.z & 1))) * hexWidth;
	//float y = -hex.z * 1.5f;
	glm::i32vec2 offset = cubeToOffset(hex);
	float x = (offset.x + (0.5f * (hex.z & 1)))* hexWidth;
	float y = -offset.y * 1.5f;
	return glm::vec3(x,y,0);
}

/** Convert odd-row offset coordinates to cube coordinates. */
CHex offsetToCube(int x, int y) {
	CHex hex;
	hex.x = x - (y - (y & 1)) / 2;
	hex.z = y;		
	hex.y = -hex.x - hex.z;
	return hex;
}

/** Round floating point coordinates to the nearest hex. */
CHex hexRound(glm::vec3& cubePos) {
	int rx = round(cubePos.x);
	int ry = round(cubePos.y);
	int rz = round(cubePos.z);

	float x_diff = abs(rx - cubePos.x);
	float y_diff = abs(ry - cubePos.y);
	float z_diff = abs(rz - cubePos.z);

	//reset component with largest change. This ensures
	//x + y + z = 0 as it should.
	if (x_diff > y_diff&& x_diff > z_diff)
		rx = -ry - rz;
	else if (y_diff > z_diff)
		ry = -rx - rz;
	else
		rz = -rx - ry;

	return CHex(rx, ry, rz);
}

/** Convert fractional axial coordinates to integer axials .*/
CHex hexRound(float q, float r) {
	return cubeToAxial(hexRound(axialToCube(q, r)));
}

/** Treat points as being on an XY plane, with z=0. */
CHex worldSpaceToHex(glm::vec3& worldSpace) {
	//find fractional position
	float q = sqrt(3.0f) / 3.0f * worldSpace.x - 1.0f / 3.0f * -worldSpace.y;
	float r = -2.0f / 3.0f * worldSpace.y;

	return hexRound(q, r);
}

/** Distance between two hex coordinates. */
int cubeDistance(CHex& cubeA, CHex& cubeB) {
	return (abs(cubeA.x - cubeB.x) + abs(cubeA.y - cubeB.y) +  abs(cubeA.z - cubeB.z)) / 2;
}

glm::vec3 hexLerp(CHex& cubeA, CHex& cubeB, float t) {
	return glm::vec3(glm::mix(cubeA.x, cubeB.x, t),
		glm::mix(cubeA.y, cubeB.y, t),
		glm::mix(cubeA.z, cubeB.z, t));

}

/** Return the line of hexes between the two hexes given. */
THexList* hexLine(CHex& cubeA, CHex& cubeB) {
	int N = cubeDistance(cubeA, cubeB);
	static THexList results;
	results.clear();

	glm::vec3 A(cubeA.x, cubeA.y, cubeA.z);
	A += glm::vec3(1e-6, 2e-6, -3e-6);
	glm::vec3 B(cubeB.x, cubeB.y, cubeB.z);
	for (int h = 0; h <= N; h++) {
		results.push_back(hexRound(glm::mix(A, B, 1.0 / N * h)  ));
	}
	return &results;
}

THexList* hexLine2(CHex& cubeA, CHex& cubeB) {
	glm::vec3 worldA = cubeToWorldSpace(cubeA) +glm::vec3(1e-4, 2e-4, -3e-4);
	glm::vec3 worldB = cubeToWorldSpace(cubeB);// +corners[4] * 0.75f;
	
	CHex cubeHex = cubeA;

	glm::vec3 faceA; glm::vec3 faceB;

	static THexList results;
	results.clear();

	while (cubeHex != cubeB) {	
		results.push_back(cubeHex);

		glm::vec3 currentHex = cubeToWorldSpace(cubeHex);


		for (int face = 0; face < 6; face++) {
			faceA = currentHex + corners[face];
			if (face < 5)
				faceB = currentHex + corners[face + 1];
			else
				faceB = currentHex + corners[0];

			glm::vec3 faceAB = faceB - faceA;
			glm::vec3 pointB = worldB - faceA;

			if (glm::cross(faceAB, pointB).z < 0)
				continue;

			if (segIntersect(worldA, worldB, faceA, faceB)) {
				cubeHex = getNeighbour(cubeHex, THexDir(face));
				break;
			}
		}
	}

	results.push_back(cubeB);

	if (results.size() >=2 )
	for (auto hex = results.begin() + 2; hex != results.end();) {
		if (isNeighbour(*hex, *(hex - 2)) && isNeighbour(*hex, *(hex -1)))
			hex = results.erase(hex-1);
		else
			hex++;
	}

	return &results;
}



THexList* hexLine3(CHex& cubeA, CHex& cubeB, int corner) {
	glm::vec3 worldA = cubeToWorldSpace(cubeA) + glm::vec3(1e-4, 2e-4, -3e-4);
	glm::vec3 worldB = cubeToWorldSpace(cubeB) + corners[corner] * 0.75f;

	CHex cubeHex = cubeA;

	glm::vec3 faceA; glm::vec3 faceB;

	static THexList results;
	results.clear();

	while (cubeHex != cubeB) {
		results.push_back(cubeHex);

		glm::vec3 currentHex = cubeToWorldSpace(cubeHex);


		for (int face = 0; face < 6; face++) {
			faceA = currentHex + corners[face];
			if (face < 5)
				faceB = currentHex + corners[face + 1];
			else
				faceB = currentHex + corners[0];

			glm::vec3 faceAB = faceB - faceA;
			glm::vec3 pointB = worldB - faceA;

			if (glm::cross(faceAB, pointB).z < 0)
				continue;

			if (segIntersect(worldA, worldB, faceA, faceB)) {
				cubeHex = getNeighbour(cubeHex, THexDir(face));
				break;
			}
		}
	}

	results.push_back(cubeB);


	if (results.size() >= 2)
	for (auto hex = results.begin() + 2; hex != results.end();) {
		if (isNeighbour(*hex, *(hex - 2)) && isNeighbour(*hex, *(hex - 1)))
			hex = results.erase(hex - 1);
		else
			hex++;
	}

	return &results;
}



THexList* hexLine4(CHex& cubeA, CHex& cubeB, int offset) {
	glm::vec3 worldA = cubeToWorldSpace(cubeA) +glm::vec3(1e-4, 2e-4, 0);
	glm::vec3 worldB = cubeToWorldSpace(cubeB);// -glm::vec3(1e-4, 2e-4, 0);
	glm::vec3 AB = worldB - worldA;

	if (offset == 1) {
		glm::vec3 offsetVec;
		offsetVec.x = AB.y;
		offsetVec.y = -AB.x;
		offsetVec = glm::normalize(offsetVec) * 0.75f;
		worldA += offsetVec;
		worldB += offsetVec;
	}
	else if (offset == 2) {
		glm::vec3 offsetVec;
		offsetVec.x = -AB.y;
		offsetVec.y = AB.x;
		offsetVec = glm::normalize(offsetVec) * 0.75f;
		worldA += offsetVec;
		worldB += offsetVec;

	}
 




	CHex cubeHex = cubeA;

	glm::vec3 faceA; glm::vec3 faceB;

	static THexList results;
	results.clear();

	while (cubeHex != cubeB) {
		results.push_back(cubeHex);

		glm::vec3 currentHex = cubeToWorldSpace(cubeHex);


		for (int face = 0; face < 6; face++) {
			faceA = currentHex + corners[face];
			if (face < 5)
				faceB = currentHex + corners[face + 1];
			else
				faceB = currentHex + corners[0];

			glm::vec3 faceAB = faceB - faceA;
			glm::vec3 pointB = worldB - faceA;

			if (glm::cross(faceAB, pointB).z < 0)
				continue;

			if (segIntersect(worldA, worldB, faceA, faceB)) {
				cubeHex = getNeighbour(cubeHex, THexDir(face));
				break;
			}
		}
	}

	results.push_back(cubeB);


	if (results.size() >= 2)
		for (auto hex = results.begin() + 2; hex != results.end();) {
			if (isNeighbour(*hex, *(hex - 2)) && isNeighbour(*hex, *(hex - 1)))
				hex = results.erase(hex - 1)+1;
			else
				hex++;
		}

	return &results;
}


/** Return the direction of the neighbouring hex. */
THexDir neighbourDirection(CHex& hex, CHex& neighbour) {
	CHex dirVector = neighbour - hex;
	for (int dir = hexEast; dir <= hexNE; dir++) {
		if (dirVector.x == moveVectorCube[dir].x && dirVector.y == moveVectorCube[dir].y
			&& dirVector.z == moveVectorCube[dir].z)
			return (THexDir)dir;
	}
	return hexNone;
}



bool isNeighbour(CHex& hex, CHex& neighbour) {
	if (cubeDistance(hex, neighbour) == 1.0f)
		return true;
	return false;
}

/** Return the relative angle of the neighbouring hex, where hexEas = 0; */
float dirToAngle(THexDir direction) {
	return (float)direction * M_PI / 3.0f;
}

/** Convert a cube direction as a normalised 3D space vector. */
glm::vec3 directionToVec(THexDir direction) {
	return glm::normalize(moveVector3D[direction]);
}

/** Return the adjacent hex in this direction. */
CHex getNeighbour(CHex& hex, THexDir direction){
	return hex + moveVectorCube[direction];
}

/** Return true if cube coordinated ar malformed. */
bool badHex(CHex& hex) {
	return hex.x + hex.y + hex.z;
}

/** Return the shortest angle of rotation required to get from start to end. */
float shortestRotation(THexDir start, THexDir end) {
	float dist = (6 + end - start) % 6;
	if (dist > 3)
		dist = -(6 - dist);

	return dist * M_PI / 3.0f;
}

/** Return the angle the end hex lies at, relative to the start hex. */
float hexAngle(CHex& start, CHex& end) {
	glm::vec3 angleVec = cubeToWorldSpace(end) - cubeToWorldSpace(start);
	angleVec = glm::normalize(angleVec);
	
	float angle = glm::acos(glm::dot(angleVec,glm::vec3(1,0,0)) );
	if (angleVec.y > 0)
		angle = 2 * M_PI - angle;
	return angle;
}

/** Return the nearest hex direction to the given angle. */
THexDir angleToDir(float angle) {
	angle += M_PI / 6.0f;
	angle = fmod(angle, 2 * M_PI);

	THexDir dir = THexDir(angle / (M_PI / 3.0f));
	return dir;
}

/** Return opposite directon. */
THexDir opposite(THexDir direction) {
	direction = THexDir(direction + 3);
	direction = THexDir(direction % 6);

	return direction;
}

/**	Convert n to a direction, with wraparound. */
THexDir intToDir(int n) {
	return THexDir(n % 6);
}

/** Return the corner hexes of this triangle. */
std::tuple<CHex,CHex> findTriangle(CHex& apex, int hexLength, float angle, float rotation) {
	//find the far midpoint in world space
	//rotate it to A
	//rotate it to B
	//convert these to hexes

	glm::vec3 farMid = glm::vec3(hexLength * hexWidth, 0, 0);
	glm::vec3 A = glm::rotate(farMid, rotation - (angle * 0.5f), glm::vec3(0, 0, -1));
	glm::vec3 B = glm::rotate(farMid, rotation + (angle * 0.5f), glm::vec3(0, 0, -1));
	A += cubeToWorldSpace(apex);
	B += cubeToWorldSpace(apex);

	CHex hexA = worldSpaceToHex(A);
	CHex hexB = worldSpaceToHex(B);
	return { hexA, hexB };
}

THexList* findArc(CHex& apex, int hexLength, float angle, float rotation) {
	glm::vec3 farMid = glm::vec3(hexLength * hexWidth , 0, 0);
	float angleA = rotation - (angle * 0.5f);
	float angleB = rotation + (angle * 0.5f);
	glm::vec3 A = glm::rotate(farMid, angleA, glm::vec3(0, 0, -1));
	glm::vec3 B = glm::rotate(farMid, angleB, glm::vec3(0, 0, -1));
//	A += cubeToWorldSpace(apex);
	//B += cubeToWorldSpace(apex);

	float step = (glm::distance(A, B) / hexWidth) * 2.0f;
	step = angle / step;

	static THexList hexes;
	hexes.clear();
	CHex lastHex(-1, -1, -1);
	glm::vec3 apexOffset = cubeToWorldSpace(apex);
	for (float a = angleA; a < angleB; a += step) {
		glm::vec3 Arotated = glm::rotate(farMid, a, glm::vec3(0, 0, -1));
		CHex currentHex = worldSpaceToHex(Arotated + apexOffset);
		if (currentHex != lastHex) {
			hexes.push_back(currentHex);
			lastHex = currentHex;
		}
	}


	return &hexes;
}

/**  Create a hexagonal 'ring' of hexes of the given radius, centred at centre.. */
THexList* findRing(int radius, CHex& centre = CHex( 0,0,0)) {
	CHex hex(radius, -radius, 0);
	static THexList hexes;
	hexes.clear();

	for (int corner = 0; corner < 6; corner++) {
		for (int h = 0; h < radius; h++) {
			hexes.push_back(hex + centre);
			hex = getNeighbour(hex, intToDir(corner +2));
		}
	}


	return &hexes;
}

/** Return the corner and offset of the hex that lies at the given angle
	of a hex 'ring' of the given radius. */
std::tuple<int, int> findRingHex(int radius, float angle) {
	float radsPerHex = M_PI / (3 * radius); //60

	int corner = std::floor(angle / (M_PI / 3.0f)) ; //60

	float remain = fmod(angle, M_PI / 3.0f); //60
	float hexOff = remain / radsPerHex;
	int hexOffset = std::ceil(remain / radsPerHex);

	return { corner, hexOffset };
}

/** Return the hexes in the arc lying at the given rotation around apex. */
THexList* findArc2(CHex& apex, int radius, float angle, float rotation) {
	
	float rotationToA = rotation - (angle / 2.0f);
	if (rotationToA < 0)
		rotationToA += M_PI * 2.0f;
	auto [startCorner, hexOffset] = findRingHex(radius, rotationToA);


	float radsPerHex = M_PI / (3 * radius);
	int hexesInArc = floor(angle / radsPerHex);

	//find the hex at A, the start of the arc
	CHex hex = findRingCornerHex(radius, startCorner);
	for (int h = 0; h < hexOffset; h++) {
		hex = getNeighbour(hex, intToDir(startCorner+2));
	}

	static THexList hexes;
	hexes.clear();
	for (int h = 0; h < hexesInArc; h++) {
		hexes.push_back(hex + apex);
		hexOffset++;
		if (hexOffset > radius) {
			hexOffset = 0;
			startCorner++;
		}
		hex = getNeighbour(hex,intToDir(startCorner + 2));
	}

	return &hexes;
}

/** Calculate the hex at the given corner of the hex 'ring' of this
	radius. Corner 0 = the NE corner, counting clockwise. */
CHex findRingCornerHex(int radius, int corner) {
	switch (corner) {
	case 0: return { radius, -radius, 0 };
	case 1: return { 0, -radius, radius };
	case 2: return { -radius, 0, radius };
	case 3: return { -radius, radius, 0 };
	case 4: return { 0, radius, -radius };
	case 5: return { radius, 0, -radius };
	}
}



bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B) {
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B); 
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A); 

	if (a1 * a2 < 0.0f)
		return true;

	return false;
}

bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B, glm::vec3& intersect) {
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B);
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A);

	if (a1 * a2 < 0.0f) {
		float a3 = signed2DTriArea(seg2A, seg2B, seg1A); 
		float a4 = a3 + a2 - a1;

		if (a3 * a4 < 0.0f) {
			float t = a3 / (a3 - a4);
			intersect = seg1A + t * (seg1B - seg1A);
			return true;
		}
	}

	return false;
}



float signed2DTriArea(glm::vec3& a, glm::vec3& b, glm::vec3& c) {
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}



