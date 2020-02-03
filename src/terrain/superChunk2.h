#pragma once

#include <vector>
#include "glm/glm.hpp"


#include "..\3DEngine\src\direction.h"
#include "chunk2.h"

#include "..\vertBufs.h"

typedef vBuf::T3DnormVert TChunkVert2;

/** Stores the inner dimensions of a shell, eg, the innermost superchunk
	layers before they are entirely replaced by the shell within, if any.*/
struct TBoxVolume {
	glm::i32vec3 bl;
	glm::i32vec3 tr;
};


class CBoxVolume {
public:
	void set(glm::vec3& bl, glm::vec3& tr) {
		this->bl = bl;
		this->tr = tr;
	}
	bool isClippedBy(CBoxVolume& clippee);
	bool doesNotEntirelyEnvelop(CBoxVolume& clippee);

	glm::vec3 bl;
	glm::vec3 tr;
};


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
};



/** A container for zero or more chunks occupying a cubic volume of space. */
class CTerrain2;
class CSuperChunk2 {
public:
	CSuperChunk2()  {};
	void checkForIntersection();
	void setSampleSpacePosition(glm::vec3& pos);
	void setSampleSize(float sampleSize);
	void setCallbackApp(ITerrainAppCallback* pApp);
	void setTerrainCallback(ITerrainCallback* pTerrain);
	void createChunk(glm::i32vec3& index);

	void clearChunks();
	void clearChunks(CBoxVolume& unitVolume);
	void addChunks(CBoxVolume& unitVolume);
	bool chunkExists(glm::i32vec3& index);
	void addChunksOutside(CBoxVolume& unitVolume);
	void addChunksBetween(CBoxVolume& outerUnitVolume, CBoxVolume& innerUnitVolume);

	//void getTris(const glm::vec3& pos, TChunkVert*& buf, unsigned int& noTris);

	void clearOverlappedChunks(TBoxVolume& innerChunkVolume);
	void clearScrolledOutChunks(Tdirection face, int maxChunks);
	void restoreClippedChunks(TBoxVolume& innerChunkVolume);
	
	int getChunkAt(glm::i32vec3& pos);

	glm::vec4 colour; //TO DO probably temp
	glm::i32vec3 origIndex; //TO DO probably temp

	glm::vec3 sampleSpacePos;
	
	static ITerrainAppCallback* pCallbackApp; ///<Pointer to the app used for callbacks.

	bool isEmpty; ///<True unless terrain intersects SC.

	//TO DO: this info is the same for all SCs of a shell.
	//condsider replacing with a pointer to a struct owned by the shell
	float sampleSize;
	int numSCchunks; //<SC size in chunks.
	float chunkSampleSize; ///<Chunk size in samplespace

	//std::vector<glm::i32vec3> chunks; ///<The chunks owned by this superchunk
	std::vector<int> scChunks; ///<Ids of the chunks owned by this superChunk;

	float cubeSize; ///<The size of this shell's marching cubes.
	int shellNo; ///TO DO may mean above is redundant

	static CTerrain2* pTerrain; ///<Annoyingly necessary pointer to terrain object.
	//TO DO Scrap

	ITerrainCallback* pTerrainObj;///<Pointer to terrain callback obj
};

