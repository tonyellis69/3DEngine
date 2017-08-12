#include "chunk.h"

Chunk::Chunk() {
	noAttribs = 2;
	init();
}

void Chunk::init() {
 //TO DO: get rid of this?
	noTris = 0;
	id = 0;
	creatorSC = NULL;
	overlapDir = none;
	status = chFree;
	drawDetails.childBufNo = -1;
	drawDetails.vertCount = 0;
	drawDetails.vertStart = 0;
}

void Chunk::setCreatorSC(CSuperChunk * creator) {
	creatorSC = creator;
}

void Chunk::getSkinned() {
	creatorSC->skinChunk(this);

}

void Chunk::setOverlap(Tdirection overlap) {
	overlapDir = overlap;
}

/** Set the position in a space where this chunk starts sampling volumetric data. */
void Chunk::setSamplePos(glm::vec3& pos) {
	samplePos = pos;
}

