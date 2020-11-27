#pragma once

#define _USE_MATH_DEFINES //for cmath

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>




enum THexDir {hexEast, hexSE, hexSW, hexWest, hexNW, hexNE, hexNone};
static glm::i32vec3 moveVectorCube[]{ {1,-1,0}, {0,-1,1}, {-1,0,1}, {-1,1,0}, {0,1,-1}, {1,0,-1} };

const float hexWidth = sqrt(3.0f);
const float hexHeight = 2;

static glm::vec3 moveVector3D[]{ {hexWidth,0,0}, {hexWidth * 0.5f,-1.5f,0}, 
	{-hexWidth * 0.5f,-1.5f,0}, {-hexWidth,0,0}, {-hexWidth * 0.5f,1.5f,0}, {hexWidth * 0.5f,1.5f,0} };

static glm::vec3 corners[]{ {hexWidth * 0.5f, 0.5f, 0}, {hexWidth * 0.5f, -0.5f, 0}, {0, -1, 0},
	{-hexWidth * 0.5f, -0.5f,0}, {-hexWidth * 0.5f, 0.5f,0}, {0, 1, 0}

};

/** A class describing a hex position. */
class CHex {
public:
	CHex();
	CHex(int n);
	CHex(int x, int y, int z);
	CHex(int q, int r);
	CHex operator + (CHex& hex2);
	CHex operator + (glm::i32vec3& hex2);
	CHex operator - (CHex& hex2);
	bool operator == (const CHex& hex2) const;
	bool operator != (CHex& hex2);
	CHex(glm::vec3& worldSpace);
	glm::i32vec2 getAxial();
	glm::i32vec3 getCubeVec();

	//cube coordinates:
	int x;
	int y;
	int z;
};

static CHex nullHex(-1);

struct hex_hash {
	size_t operator()(const CHex& hex) const {
		return std::hash<glm::i32vec3>()(glm::i32vec3(hex.x, hex.y, hex.z));
	}
};

using THexList = std::vector<CHex> ;

CHex cubeToAxial(CHex& cube);
glm::vec3 axialToCube(float q, float r);
glm::i32vec2 cubeToOffset(const CHex& hex);
glm::i32vec2 axialToOffset(int q, int r);
glm::vec3 cubeToWorldSpace(CHex& hex);
CHex offsetToCube(int x, int y);
CHex hexRound(glm::vec3& cubePos);
CHex hexRound(float q, float r);
CHex worldSpaceToHex(glm::vec3& worldSpace);
int cubeDistance(CHex& cubeA, CHex& cubeB);
THexList* hexLine(CHex& cubeA, CHex& cubeB);
THexList* hexLine2(CHex& cubeA, CHex& cubeB);
THexList* hexLine3(CHex& cubeA, CHex& cubeB, int corner);
THexList* hexLine4(CHex& cubeA, CHex& cubeB, int offset);
THexDir neighbourDirection(CHex& hex, CHex& neighbour);
bool isNeighbour(CHex& hex, CHex& neighbour);
float dirToAngle(THexDir direction);
glm::vec3 directionToVec(THexDir direction);
CHex getNeighbour(CHex& hex, THexDir direction);
bool badHex(CHex& hex);
float shortestRotation(THexDir start, THexDir end);
float hexAngle(CHex& start, CHex& end);
THexDir angleToDir(float angle);
THexDir opposite(THexDir direction);
THexDir intToDir(int n);
std::tuple<CHex, CHex> findTriangle(CHex& apex, int hexLength, float angle, float rotation);
THexList* findArc(CHex& apex, int hexLength, float angle, float rotation);
THexList* findRing( int radius, CHex& centre);
std::tuple<int, int> findRingHex(int radius, float angle);
THexList* findArc2(CHex& apex, int radius, float angle, float rotation);
CHex findRingCornerHex(int radius, int corner);




