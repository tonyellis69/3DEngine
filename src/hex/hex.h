#pragma once

#include <vector>

#include <glm/glm.hpp>


enum THexDir { hexNE, hexEast, hexSE, hexSW, hexWest, hexNW };
static glm::i32vec3 moveVectorCube[]{ {1,0,-1}, {1,-1,0}, {0,-1,1}, {-1,0,1}, {-1,1,0}, {0,1,-1} };

const float hexWidth = sqrt(3);
const float hexHeight = 2;

static glm::vec3 moveVector3D[]{ {hexWidth *0.5f,1.5f,0}, {hexWidth,01,0}, {hexWidth * 0.5f,-1.5f,0}, 
	{-hexWidth * 0.5f,-1.5f,0}, {-hexWidth,0,0}, {-hexWidth * 0.5f,1.5f,0} };

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

	//cube coordinates:
	int x;
	int y;
	int z;
};

typedef std::vector<CHex> THexList;

CHex cubeToAxial(CHex& cube);
glm::vec3 axialToCube(float q, float r);
glm::i32vec2 cubeToOffset(CHex& hex);
glm::vec3 cubeToWorldSpace(CHex& hex);
CHex hexRound(glm::vec3& cubePos);
CHex hexRound(float q, float r);
CHex worldSpaceToHex(glm::vec3& worldSpace);
int cubeDistance(CHex& cubeA, CHex& cubeB);
THexList* hexLine(CHex& cubeA, CHex& cubeB);
THexDir neighbourDirection(CHex& hex, CHex& neighbour);
glm::vec3 directionToVec(THexDir direction);
CHex getNeighbour(CHex& hex, THexDir direction);