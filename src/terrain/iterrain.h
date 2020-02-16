#pragma once

#include <vector>
#include "glm/glm.hpp"


#include "..\3DEngine\src\direction.h"
#include "chunk2.h"

#include "..\vertBufs.h"

typedef vBuf::T3DnormVert TChunkVert2;


class ITerrainAppCallback {
public:
	virtual bool scIntersectionCheckCallback(glm::vec3& pos, float scSize) { return false; };
	virtual bool chunkCheckCallback(glm::vec3& chunkPos, float chunkSize) { return false; };
	virtual void createChunkMesh(Chunk2& chunk) {};
	virtual void deleteChunkMesh(Chunk2& chunk) {};
	virtual unsigned int getChunkTrisCallback(int chunkId, TChunkVert2* buf) { return 0; };
	virtual void onTerrainScroll(glm::vec3& movement) {}

};

class ITerrainCallback {
public:
	virtual void realignOuterShells(int shell, Tdirection moveDirection) = 0;
	
	virtual void recentreOuterShells(int shellNo, Tdirection moveDirection) = 0;
	virtual void scrollSampleSpace(Tdirection scrollDir, float shift) = 0;

	virtual void preScrollUpdate(int shellNo, Tdirection direction) = 0;

	virtual int createChunk(glm::i32vec3& index, glm::vec3& sampleCorner, int shellNo, glm::vec3& terrainPos) = 0;
	virtual void removeChunk(int id) = 0;
	virtual glm::vec3 getSCworldPos(int shellNo, const glm::i32vec3& origIndex) = 0;
	virtual void scrollViewpoint(Tdirection scrollDir) = 0;
	virtual void rebuildOuterShell(int shellNo, Tdirection scrollOutFace) = 0;
};
