#include "superChunk2.h"

#include <iostream>

ITerrainCallback* CSuperChunk2::pCallbackApp = NULL;

void CSuperChunk2::checkForIntersection() {
	//call intersection check callout
	
//	std::cerr << "\n#" << origIndex.x << " " << origIndex.y << " " << origIndex.z << " "
//		<< sampleSpacePos.x << " " << sampleSpacePos.y << " " << sampleSpacePos.z;
	isEmpty = pCallbackApp->scIntersectionCheckCallback(sampleSpacePos, sampleSize);
}

void CSuperChunk2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

void CSuperChunk2::setSampleSize(float sampleSize) {
	this->sampleSize = sampleSize;
}

void CSuperChunk2::setCallbackApp(ITerrainCallback * pApp) {
	pCallbackApp = pApp;
}
