#pragma once

#include <vector>
#include<queue>

#include "..\terrain\shell.h"
#include "..\direction.h"
#include "chunk2.h"

#include "..\vertBufs.h"
#include "..\renderer\multiBuf2.h"

typedef vBuf::T3DnormVert TChunkVert2;

class TChunkTriBuf2 {
public:
	TChunkTriBuf2() { id = noTris = 0; /*buf = NULL;*/ }
	unsigned int id;
	unsigned int noTris;
	TChunkVert2 buf[4200];
};

struct TSCident {
	int shellNo;
	glm::i32vec3 index;
};

const int chunkTriCacheSize2 = 6;

class CTerrain2 : public ITerrain {
public:
	CTerrain2();
	void setInitialChunkStorageSize(int bytes);
	void setChunkVertLayout(std::initializer_list<int> attribs);
	void createCentralShell(float _LoD1cubeSize, int numChunkCubes, int numSCchunks, int shellSCs);
	void addShell(int extent);
	void onPlayerMove(glm::vec3& move);
	void createTerrain();
	void setSampleSpacePosition(glm::vec3& pos);
	void setWorldScale(float scale);
	void setCallbackApp(ITerrainAppCallback* pApp);
	void update(float dT);
	TSCident getSC(const glm::vec3& pos);
	void getTris(CSuperChunk2* sc, const glm::vec3& pos, TChunkVert2*& buf, unsigned int& noTris);
	int storeChunkMesh(CBuf& src, int size);
	void freeChunkMesh(int addr);
	unsigned int getChunkMeshVAO();
	void setViewpoint(glm::vec3& pos);
	ITerrainAppCallback* getTerrainApp();
	float getShellSize(unsigned int shellNo);

	std::vector<CShell> shells;

	int numChunkCubes;	//<cubes per chunk edge.
	int numSCchunks;  //<SC size in chunks.

	glm::vec3 sampleSpacePos; //<Position of terrain in sample space.
	float worldToSampleScale; ///<Number of world units to one sample unit
	ITerrainAppCallback* pCallbackApp; ///<Pointer to the app used for callbacks.

	std::vector<Chunk2> chunks; ///<The complete reservoir of chunks.
	
	
	glm::mat4 chunkOrigin; ///<TO DO: keep this, or do some other way?

	
	glm::vec3 viewpoint; ///>The position of the terrain viewer relative to origin
	glm::vec3 oldViewpoint;

private:
	void initialiseChunks(int numChunks);
	void removeChunkOverlaps(Tdirection inDirection);
	void realignOuterShells(int shell, Tdirection moveDirection);
	void recentreOuterShells(int shellNo, Tdirection moveDirection);

	void shellPreScrollUpdate(int shellNo, Tdirection direction);

	void scrollSampleSpace(Tdirection scrollDir, float shift);
	int createChunk(glm::i32vec3& index, glm::vec3& sampleCorner, int shellNo, glm::vec3& terrainPos);
	int getFreeChunk();
	void removeChunk(int id);
	glm::vec3 getSCworldPos(int shellNo, const glm::i32vec3& origIndex);
	void scrollViewpoint(Tdirection scrollDir);
	void rebuildOuterShell(int shellNo, Tdirection scrollOutFace);
	glm::i32vec3 getChunkIndex(int chunkId);

	const int approxChunksRequired = 2000; 
	int chunksToSkinPerFrame;
	
	float LoD1cubeSize; //<In worldspace.
	float LoD1chunkSize; //<In worldspace.

	std::vector<int> freeChunks;
	std::queue<int> chunksToMesh;

	TChunkTriBuf2 cachedChunkTris[chunkTriCacheSize2]; ///<Chunk triangles recently downloaded from graphics memory.
	int freeChunkTriCache;

	CMultiBuf2 multiBuf;

	
};



