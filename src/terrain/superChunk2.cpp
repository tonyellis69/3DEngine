#include "superChunk2.h"

void CSuperChunk2::checkForIntersection() {
	//call intersection check callout

}

void CSuperChunk2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

void CSuperChunk2::setSampleSize(float sampleSize) {
	this->sampleSize = sampleSize;
}
