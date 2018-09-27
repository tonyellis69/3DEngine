#pragma once

#include "glm/glm.hpp"

class CShell {
public:
	CShell(int LoD, float chunkSize, int SCsize, int shellSCs);

	int LoD; //<1=highest, then 2,4,8, etc
	int SCchunks; //<SC size in chunks.
	int shellSCs; //<shell size in superChunks.

	float chunkSize; //<In worldspace.
	float SCsize; //<In worlsspace
	float worldSpaceSize; 
	glm::i32vec3 worldSpacePos;
};