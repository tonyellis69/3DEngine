#pragma once

#include "glm/glm.hpp"

enum  TChunkStatus { chToSkin, chSkinned, chFree, chRemoveOnAlert, chDead };

struct TDrawDetails {
	int childBufNo;
	unsigned int vertStart;
	unsigned int vertCount;
	glm::vec4 colour;
};


/** A handle object representing a cubic, indivisible block of terrain. */
class Chunk2 {
public:
	Chunk2();
	void setSample(glm::vec3& sampleCorner, float sampleSize) {
		this->sampleCorner = sampleCorner;
		this->sampleSize = sampleSize;
	}

	glm::i32vec3 index; ///<3D position of this chunk within its superChunk.
	glm::vec3 sampleCorner;
	float sampleSize;
	float cubeSize;
	float LoD;
	glm::vec3 terrainPos;

	unsigned int bufId;
	TDrawDetails drawDetails;
	TDrawDetails drawDetails2;

	glm::vec4 colour; ///Colour chunk will be shaded in.
	TChunkStatus status;

};