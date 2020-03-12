#pragma once

#define _USE_MATH_DEFINES //for cmath

#include <vector>


#include <glm/glm.hpp>


enum THexDir {hexEast, hexSE, hexSW, hexWest, hexNW, hexNE, hexNone};
static glm::i32vec3 moveVectorCube[]{ {1,-1,0}, {0,-1,1}, {-1,0,1}, {-1,1,0}, {0,1,-1}, {1,0,-1} };

const float hexWidth = sqrt(3.0f);
const float hexHeight = 2;

static glm::vec3 moveVector3D[]{ {hexWidth,0,0}, {hexWidth * 0.5f,-1.5f,0}, 
	{-hexWidth * 0.5f,-1.5f,0}, {-hexWidth,0,0}, {-hexWidth * 0.5f,1.5f,0}, {hexWidth * 0.5f,1.5f,0} };

/** A class describing a hex position. */
class CHex {
public:
	CHex();
	CHex(int x, int y, int z);
	CHex(int q, int r);
	CHex operator + (CHex& hex2);
	CHex operator + (glm::i32vec3& hex2);
	CHex operator - (CHex& hex2);
	bool operator == (CHex& hex2);
	bool operator != (CHex& hex2);
	CHex(glm::vec3& worldSpace);
	glm::i32vec2 getAxial();
	glm::i32vec3 getCubeVec();

	//cube coordinates:
	int x;
	int y;
	int z;
};

using THexList = std::vector<CHex> ;

CHex cubeToAxial(CHex& cube);
glm::vec3 axialToCube(float q, float r);
glm::i32vec2 cubeToOffset(CHex& hex);
glm::i32vec2 axialToOffset(int q, int r);
glm::vec3 cubeToWorldSpace(CHex& hex);
CHex offsetToCube(int x, int y);
CHex hexRound(glm::vec3& cubePos);
CHex hexRound(float q, float r);
CHex worldSpaceToHex(glm::vec3& worldSpace);
int cubeDistance(CHex& cubeA, CHex& cubeB);
THexList* hexLine(CHex& cubeA, CHex& cubeB);
THexDir neighbourDirection(CHex& hex, CHex& neighbour);
bool isNeighbour(CHex& hex, CHex& neighbour);
float dirToAngle(THexDir direction);
glm::vec3 directionToVec(THexDir direction);
CHex getNeighbour(CHex& hex, THexDir direction);
bool badHex(CHex& hex);
float shortestRotation(THexDir start, THexDir end);
float hexAngle(CHex& start, CHex& end);