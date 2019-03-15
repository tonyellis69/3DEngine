#include "superChunk2.h"

#include <iostream>

ITerrainCallback* CSuperChunk2::pCallbackApp = NULL;

void CSuperChunk2::checkForIntersection() {
	isEmpty = pCallbackApp->scIntersectionCheckCallback(sampleSpacePos, sampleSize);
}

void CSuperChunk2::setSampleSpacePosition(glm::vec3 & pos) {
	if (origIndex == glm::i32vec3(0, 0, 0))
		std::cerr << "\n x= 0 samplespace pos of " << sampleSpacePos.x << " " << sampleSpacePos.y << " "
		<< sampleSpacePos.z << " changed to " << pos.x << " " << pos.y << " " << pos.z;
	if (origIndex == glm::i32vec3(4, 0, 0))
		std::cerr << "\n x= 4 samplespace pos of " << sampleSpacePos.x << " " << sampleSpacePos.y << " "
		<< sampleSpacePos.z << " changed to " << pos.x << " " << pos.y << " " << pos.z;
	sampleSpacePos = pos;
}

void CSuperChunk2::setSampleSize(float sampleSize) {
	this->sampleSize = sampleSize;
	chunkSampleSize = sampleSize / SCchunks;
}

void CSuperChunk2::setCallbackApp(ITerrainCallback * pApp) {
	pCallbackApp = pApp;
}

/** Divide this superchunk into chunk volumes, and create a chunk for each one
	intersected by terain. */
void CSuperChunk2::createAllChunks() {
	//find each chunk volume, using the number of chunks and their size or
	//the size of the SC, 
	//and thus find the samplespace position to check
	//send that and the sample area to check to the callout.
	glm::vec3 chunkSampleSpacePos;
	for (int x = 0; x < SCchunks; x++) {
		for (int y = 0; y < SCchunks; y++) {
			for (int z = 0; z < SCchunks; z++) {
				chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
				if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
					createChunk(glm::vec3(x, y, z));
				}

			}
		}
	}


}

void CSuperChunk2::createChunk(glm::vec3 & index) {
}
