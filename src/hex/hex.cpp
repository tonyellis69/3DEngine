#include "hex.h"

#include <cmath>

#include "..\utils\log.h"


CHex::CHex() {
	x = 0; y = 0; z = 0;
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

bool CHex::operator==(CHex& hex2) {
	return (x == hex2.x) && (y == hex2.y) && (z == hex2.z);
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

/** Convert cube coordiantes to odd-row offset coordinates.*/
glm::i32vec2 cubeToOffset(CHex& hex) {
	return glm::i32vec2(hex.x + (hex.z - (hex.z & 1)) / 2, hex.z);
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

CHex worldSpaceToHex(glm::vec3& worldSpace) {
	//find fractional position
	float q = sqrt(3.0f) / 3.0f * worldSpace.x - 1.0f / 3.0f * -worldSpace.y;
	float r = 2.0f / 3.0f * -worldSpace.y;

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

/** Return the direction of the neighbouring hex. */
THexDir neighbourDirection(CHex& hex, CHex& neighbour) {
	CHex dirVector = neighbour - hex;
	for (int dir = hexNE; dir <= hexNW; dir++) {
		if (dirVector.x == moveVectorCube[dir].x && dirVector.y == moveVectorCube[dir].y
			&& dirVector.z == moveVectorCube[dir].z)
			return (THexDir)dir;
	}
	fatalLog << "\nNeighbour direction not found!";
}

/** Convert a cube direction as a normalised 3D space vector. */
glm::vec3 directionToVec(THexDir direction) {
	return glm::normalize(moveVector3D[direction]);
}

/** Return the adjacent hex in this direction. */
CHex getNeighbour(CHex& hex, THexDir direction){
	return hex + moveVectorCube[direction];
}
