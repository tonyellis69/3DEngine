#pragma once

#include <vector>
#include<queue>

#include "..\terrain\shell.h"
#include "..\direction.h"
#include "chunk2.h"


#include "..\renderer\multiBuf2.h"

#include "physics2/physObj2.h"

#include "chunkDataCache.h"

extern int tmpCount2;

const int chunkTriCacheSize2 = 6;

class CTerrain2 : public ITerrain, public CPhysObj2 {
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
	//TSCident getSC(const glm::vec3& pos);
	int storeChunkMesh(CBuf& src, int size);
	void freeChunkMesh(int addr);
	unsigned int getChunkMeshVAO();
	void setViewpoint(glm::vec3& pos);
	ITerrainAppCallback* getTerrainApp();
	float getShellSize(unsigned int shellNo);
	void overwriteInnerShellChunks(int shellNo);

	std::vector<CShell> shells;

	int numChunkCubes;	//<cubes per chunk edge.
	int numSCchunks;  //<SC size in chunks.

	glm::vec3 sampleSpacePos; //<Position of terrain in sample space.
	float worldToSampleScale; ///<Number of world units to one sample unit
	ITerrainAppCallback* pTerrainApp; ///<Pointer to the app used for callbacks.

	std::vector<Chunk2> chunks; ///<The complete reservoir of chunks.
	
	
	glm::mat4 chunkOrigin; ///<TO DO: keep this, or do some other way?

	
	glm::vec3 viewpoint; ///>The position of the terrain viewer relative to origin
	glm::vec3 oldViewpoint;

private:
	void initialiseChunkPool(int numChunks);
	void removeShellOverlaps(Tdirection inDirection);
	void realignOuterShells(int shell, Tdirection moveDirection);
	void recentreOnInnerShells(int shellNo, Tdirection moveDirection);

	void prepShellForScroll(int shellNo, Tdirection direction);

	void scrollSampleSpace(Tdirection scrollDir, float shift);
	int createChunk(glm::i32vec3& index, glm::vec3& sampleCorner, int shellNo, glm::vec3& terrainPos);
	int getFreeChunk();
	void removeChunk(int id);
	glm::vec3 getSCworldPos(int shellNo, const glm::i32vec3& origIndex);
	void scrollViewpoint(Tdirection scrollDir);
	void rebuildOuterShellInnerFace(int shellNo, Tdirection scrollOutFace);
	glm::i32vec3 getChunkIndex(int chunkId);
	glm::vec3 getChunkOrigin() {
		return chunkOrigin[3];
	}
	float getChunkSize(int shellNo) {
		return shells[shellNo].chunkSize;
	}
	void setChunkColour(int chunkId, glm::vec4& colour) {
		chunks[chunkId].colour = colour;
	}

	const int approxChunksRequired = 2000;
	int chunksToSkinPerFrame;
	
	float LoD1cubeSize; //<In worldspace.
	float LoD1chunkSize; //<In worldspace.

	std::vector<int> freeChunks;
	std::queue<int> chunksToMesh;

	TChunkTriBuf2 cachedChunkTris[chunkTriCacheSize2]; ///<Chunk triangles recently downloaded from graphics memory.
	int freeChunkTriCache;

	CMultiBuf2 multiBuf;


	//physics stuff
	void updateModelPosition(glm::vec3& dPos) {};
	glm::vec3 getPos() {
		return glm::vec3(0);
	}
	TAaBB calcAABB();

	Contact checkCollision(CPhysObj2* objB);
	
	virtual glm::vec3 calcBaseVertPos() {
		return glm::vec3(0);
	}

	TChunkTriBuf2* getShell0ChunkDataAt(glm::vec3& pos);

	float checkChunkCollision( glm::vec3& baseVector, TChunkTriBuf2* chunkData);

	ChunkDataCache chunkDataCache;
};



