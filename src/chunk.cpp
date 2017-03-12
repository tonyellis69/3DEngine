#include "chunk.h"

Chunk::Chunk() {
	nAttribs = 3;
	init();
}

void Chunk::init() {
 //TO DO: get rid of this?
}

/** Set the position in a space where this chunk starts sampling volumetric data. */
void Chunk::setSamplePos(glm::vec3& pos) {
	samplePos = pos;
}

