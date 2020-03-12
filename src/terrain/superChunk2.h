#pragma once

#include <vector>
#include <tuple>

#include "glm/glm.hpp"

#include "..\3DEngine\src\direction.h"
#include "chunk2.h"


#include "iterrain.h"


/** Stores the inner dimensions of a shell, eg, the innermost superchunk
	layers before they are entirely replaced by the shell within, if any.*/
struct TBoxVolume {
	glm::i32vec3 bl;
	glm::i32vec3 tr;
};

class CBoxVolume;
class CBoxVolume {
public:
	void set(glm::vec3& bl, glm::vec3& tr) {
		this->bl = bl;
		this->tr = tr;
	}
	std::tuple<bool, CBoxVolume> findOverlap(CBoxVolume clippee);

	bool doesNotEntirelyEnvelop(CBoxVolume& clippee);

	glm::vec3 bl;
	glm::vec3 tr;
};





/** A container for zero or more chunks occupying a cubic volume of space. */
class CTerrain2;
class CSuperChunk2 {
public:
	CSuperChunk2()  {};
	void checkForTerrainIntersection();
	void setSampleSpacePosition(glm::vec3& pos);
	void setSampleSize(float sampleSize);
	void setCallbackApp(ITerrainAppCallback* pApp);
	void setTerrainCallback(ITerrain* pTerrain);
	void createChunk(glm::i32vec3& index);

	void clearChunks();
	void clearChunks(CBoxVolume& unitVolume);
	void addChunks(CBoxVolume& unitVolume);
	bool chunkExists(glm::i32vec3& index);
	void addChunksOutside(CBoxVolume& unitVolume);
	void addChunksBetween(CBoxVolume& outerUnitVolume, CBoxVolume& innerUnitVolume);

	//void getTris(const glm::vec3& pos, TChunkVert*& buf, unsigned int& noTris);

	void clearOverlappedChunks(TBoxVolume& innerChunkVolume);
	void clearOuterLayerChunks(Tdirection face, int maxChunks);
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

	ITerrain* pTerrainObj;///<Pointer to terrain callback obj
};

