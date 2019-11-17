#pragma once

#include <glm/glm.hpp>

/** A class describing a hex position. */
class CHex {
public:
	CHex();
	CHex(int x, int y, int z);
	CHex(int q, int r);
	CHex(glm::vec3& worldSpace);
	glm::i32vec2 getAxial();

	//cube coordinates:
	int x;
	int y;
	int z;
};


CHex cubeToAxial(CHex& cube);
glm::vec3 axialToCube(float q, float r);
CHex cubeRound(glm::vec3& cubePos);
CHex hexRound(float q, float r);
CHex worldSpaceToHex(glm::vec3& worldSpace);