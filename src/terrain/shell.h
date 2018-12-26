#pragma once

#include "glm/glm.hpp"

#include "..\direction.h"
#include "scArray.h"

class CTerrain2;
class CShell {
public:
	CShell(int LoD, float chunkSize, int SCsize, int shellSCs);
	void playerAdvance(Tdirection direction);
	void advance(Tdirection direction);
	int getPlayerChunkExtent(Tdirection direction);
	void fillEntire(int chunkExtent);
	void addToFaceLayer(Tdirection direction);
	void scroll(Tdirection direction);
	void requestWorldMove(Tdirection scrollDirection);
	void initSuperChunks();

	int LoD; //<1=highest, then 2,4,8, etc
	int SCchunks; //<SC size in chunks.
	int shellSCs; //<shell size in superChunks.

	float chunkSize; //<In worldspace.
	float SCsize; //<In worlsspace
	float worldSpaceSize; 
	glm::i32vec3 worldSpacePos;
	int minimumChunkExtent; //<Terrain must always extend this far from player.
	glm::i32vec3 playerChunkPos;  //<Player position in chunks, relative to origin

	int chunkExtent[6]; //<How far chunks extend from origin in each direction
	bool faceLayerFull[6]; //<True if there's no room to add chunks to this face layer of SCs

	CTerrain2* pTerrain; ///<Pointer to parent terrain object
	int shellNo;

	CSCarry scArray;
};