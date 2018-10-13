#pragma once

#include "glm/glm.hpp"

#include "..\direction.h"

class CShell {
public:
	CShell(int LoD, float chunkSize, int SCsize, int shellSCs);
	void advancePlayer(Tdirection direction);
	int chunkExtent(Tdirection direction);

	int LoD; //<1=highest, then 2,4,8, etc
	int SCchunks; //<SC size in chunks.
	int shellSCs; //<shell size in superChunks.

	float chunkSize; //<In worldspace.
	float SCsize; //<In worlsspace
	float worldSpaceSize; 
	glm::i32vec3 worldSpacePos;
	int minimumChunkExtent; //<Terrain must always extend this far from player.
	glm::i32vec3 playerChunkPos;  //<Player position in chunks, relative to origin
};