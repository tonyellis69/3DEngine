#pragma once

#include <vector>

#include "..\terrain\shell.h"

#include "..\direction.h"


class Chunk2;
class CTerrain2 {
public:
	CTerrain2() : playerOffset(0) {};
	void createLoD1shell(float _LoD1cubeSize, int chunkCubes, int SCchunks, int shellSCs);
	void addShell(int extent);
	float getShellSize(unsigned int shellNo);
	void playerWalk(glm::vec3& move);
	void removeChunkOverlaps(Tdirection inDirection);
	void fillShells();
	void displaceOuterShells(const CShell& sender, Tdirection moveDirection);
	void returnShellAndOuterShells(const CShell& sender, Tdirection moveDirection);
	void setSampleSpacePosition(glm::vec3& pos);
	void setWorldScale(float scale);
	void setCallbackApp(ITerrainCallback* pApp);
	void scrollSampleSpace(Tdirection scrollDir, float shift);
	void initialiseChunks(int numChunks);
	int createChunk(glm::i32vec3& index);
	int getFreeChunk();

	std::vector<CShell> shells;

	float LoD1cubeSize; //<In worldspace.
	float LoD1chunkSize; //<In worldspace.
	int chunkCubes;	//<cubes per chunk edge.
	int SCchunks;  //<SC size in chunks.

	glm::vec3 playerOffset; //<Deterimines if player moved far enough to advance scenery.

	glm::vec3 sampleSpacePos; //<Position of terrain in sample space.
	float worldToSampleScale; ///<Number of world units to one sample unit
	ITerrainCallback* pCallbackApp; ///<Pointer to the app used for callbacks.

	std::vector<Chunk2> chunks; ///<The complete reservoir of chunks.
	std::vector<int> freeChunks;
	std::vector<int> inUseChunks;
};

/** A handle object representing a cubic, indivisible block of terrain. */
class Chunk2 {
public:
	glm::i32vec3 index; ///<3D position of this chunk within its superChunk.

};