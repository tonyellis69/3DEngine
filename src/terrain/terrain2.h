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

//class CSuperChunk;

class CTerrain2 {
public:
	CTerrain2();
	void setInitialChunkStorageSize(int bytes);
	void setChunkVertLayout(std::initializer_list<int> attribs);
	void createLoD1shell(float _LoD1cubeSize, int chunkCubes, int SCchunks, int shellSCs);
	void addShell(int extent);
	float getShellSize(unsigned int shellNo);
	void onPlayerMove(glm::vec3& move);
	void removeChunkOverlaps(Tdirection inDirection);
	void fillShells();
	void alignOuterShellsWithScroll(const CShell& sender, Tdirection moveDirection);
	void recentreShellsAfterScroll(const CShell& sender, Tdirection moveDirection);
	void setSampleSpacePosition(glm::vec3& pos);
	void setWorldScale(float scale);
	void setCallbackApp(ITerrainCallback* pApp);
	void scrollSampleSpace(Tdirection scrollDir, float shift);
	void initialiseChunks(int numChunks);
	int createChunk(glm::i32vec3& index, glm::vec3& sampleCorner, int shellNo, glm::vec3& terrainPos);
	int getFreeChunk();
	void removeChunk(int id);

	void update(float dT);

	glm::vec3 getSCpos(int shellNo,const glm::i32vec3& origIndex);
	glm::vec3 getSCpos2(int shellNo, const glm::i32vec3& origIndex);

	TSCident getSC(const glm::vec3& pos);

	void getTris(CSuperChunk2* sc, const glm::vec3& pos, TChunkVert2*& buf, unsigned int& noTris);

	

	
	std::vector<CShell> shells;

	float LoD1cubeSize; //<In worldspace.
	float LoD1chunkSize; //<In worldspace.
	int chunkCubes;	//<cubes per chunk edge.
	int SCchunks;  //<SC size in chunks.

	glm::vec3 playerDisplacement; //<Deterimines if player moved far enough to advance scenery.

	glm::vec3 sampleSpacePos; //<Position of terrain in sample space.
	float worldToSampleScale; ///<Number of world units to one sample unit
	ITerrainCallback* pCallbackApp; ///<Pointer to the app used for callbacks.

	std::vector<Chunk2> chunks; ///<The complete reservoir of chunks.
	std::vector<int> freeChunks;
	std::queue<int> chunksToMesh;

	glm::mat4 chunkOrigin; ///<TO DO: keep this, or do some other way?

	TChunkTriBuf2 cachedChunkTris[chunkTriCacheSize2]; ///<Chunk triangles recently downloaded from graphics memory.
	int freeChunkTriCache;


	CMultiBuf2 multiBuf;

private:

};

