#include "chunk.h"

Chunk::Chunk() {
	noAttribs = 3;
	init();
}

void Chunk::init() {
 //TO DO: get rid of this?
	noTris = 0;
	id = 0;
	creatorSC = NULL;
}

void Chunk::setCreatorSC(CSuperChunk * creator) {
	creatorSC = creator;
}

void Chunk::getSkinned() {
	creatorSC->skinChunk(this);

}

/** Set the position in a space where this chunk starts sampling volumetric data. */
void Chunk::setSamplePos(glm::vec3& pos) {
	samplePos = pos;
}

