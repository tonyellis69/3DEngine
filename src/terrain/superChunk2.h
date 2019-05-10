#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "..\3DEngine\src\direction.h"

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
	glm::vec3 bl;
	glm::vec3 tr;
};

class ITerrainCallback {
public:
	virtual bool scIntersectionCheckCallback(glm::vec3& pos, float scSize) { return false; };
	virtual bool chunkCheckCallback(glm::vec3& chunkPos, float chunkSize) { return false; };

};

/** A container for zero or more chunks occupying a cubic volume of space. */
class CSuperChunk2 {
public:
	CSuperChunk2()  {};
	void checkForIntersection();
	void setSampleSpacePosition(glm::vec3& pos);
	void setSampleSize(float sampleSize);
	void setCallbackApp(ITerrainCallback* pApp);
	void createAllChunks();
	void createChunk(glm::i32vec3& index);

	void addFaceChunks(Tdirection face); 

	void clearChunks();
	void clearChunks(CBoxVolume& unitVolume);

	void clearOverlappedChunks(TBoxVolume& innerChunkVolume);
	void clearScrolledOutChunks(Tdirection face, int maxChunks);
	void restoreClippedChunks(TBoxVolume& innerChunkVolume);

	glm::vec4 colour; //TO DO probably temp
	glm::i32vec3 origIndex; //TO DO probably temp

	glm::vec3 sampleSpacePos;
	
	static ITerrainCallback* pCallbackApp; ///<Pointer to the app used for callbacks.

	bool isEmpty; ///<True unless terrain intersects SC.

	//TO DO: this info is the same for all SCs of a shell.
	//condsider replacing with a pointer to a struct owned by the shell
	float sampleSize;
	int SCchunks; //<SC size in chunks.
	float chunkSampleSize; ///<Chunk size in samplespace

	std::vector<glm::i32vec3> chunks; ///<The chunks owned by this superchunk
	glm::i32vec3 nwChunkStart; ///<Northwest lower Limit to where chunks are placed.
	glm::i32vec3 seChunkEnd; ///<Southeast upper Limit to where chunks are placed.

};

