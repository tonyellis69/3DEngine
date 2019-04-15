#include "superChunk2.h"

#include <iostream>

#include "..\utils\log.h"

ITerrainCallback* CSuperChunk2::pCallbackApp = NULL;

void CSuperChunk2::checkForIntersection() {
	isEmpty = pCallbackApp->scIntersectionCheckCallback(sampleSpacePos, sampleSize);
}

void CSuperChunk2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

void CSuperChunk2::setSampleSize(float sampleSize) {
	this->sampleSize = sampleSize;
	chunkSampleSize = sampleSize / SCchunks;
}

void CSuperChunk2::setCallbackApp(ITerrainCallback * pApp) {
	pCallbackApp = pApp;
}

/** For each chunk volume within this SC's chunk clipping limits (if any), create a chunk 
	if the volume is intersected by terain. */
void CSuperChunk2::createAllChunks() {
	glm::vec3 chunkSampleSpacePos;
	for (int x = nwChunkStart.x; x <= seChunkEnd.x; x++) {
		for (int y = nwChunkStart.y; y <= seChunkEnd.y; y++) {
			for (int z = nwChunkStart.z; z <= seChunkEnd.z; z++) {
				chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
				if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
					createChunk(glm::i32vec3(x, y, z));
				}

			}
		}
	}


}

/** Acquire a new chunk at the given index position. */
void CSuperChunk2::createChunk(glm::i32vec3 & index) {
	chunks.push_back(index);
}


////////////////////////////////////////
//TO DO: oops, may not need this. Why not just use creatAllChunks() and simply have a rule
//that if there's already a chunk at a given index point we pass on?
//However - there is a slight cost in looking up each chunk index in the chunks vector
//-that's going to add up. 
// When I've got everything working, see if this is an optimisation.


/** This SC's chunk clipping limits have been extended by two layers. Iterating within that space,
	create a chunk where terrain intersects a chunk volume. */
void CSuperChunk2::addFaceChunks(Tdirection face) {
	glm::i32vec3 localnwChunkStart = nwChunkStart;
	glm::i32vec3 localseChunkEnd = seChunkEnd;
	int axis = getAxis(face);

	if (face == north || face == west || face == down) {
		localseChunkEnd = seChunkEnd;
		localseChunkEnd[axis] = nwChunkStart[axis] + 1;
	}
	else {
		localnwChunkStart = nwChunkStart;
		localnwChunkStart[axis]  = seChunkEnd[axis] - 1;
	}


	glm::vec3 chunkSampleSpacePos;
	for (int x = localnwChunkStart.x; x <= localseChunkEnd.x; x++) {
		for (int y = localnwChunkStart.y; y <= localseChunkEnd.y; y++) {
			for (int z = localnwChunkStart.z; z <= localseChunkEnd.z; z++) {
				chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
				if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
					createChunk(glm::i32vec3(x, y, z));
				}

			}
		}
	}


}

/** Remove all th chunks of this superchunk.*/
void CSuperChunk2::clearChunks() {
	//TO DO: when we have actual chunks, recycle them.
	chunks.clear();
}

/** Remove any chunks withing the given volume of chunkspace. */
void CSuperChunk2::clearOverlappedChunks(TBoxVolume & innerChunkVolume) {
	//for each chunk...
	//determine its position in chunk space
	//if it falls within the given volume, remove it.

	for (auto chunk : chunks) {



	}



}
