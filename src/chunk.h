#pragma once
#include "superChunk.h"
#include "renderer/model.h"
#include "direction.h"

extern int uniTag;

class CSuperChunk;

enum  TChunkStatus { chToSkin, chSkinned, chFree, chRemoveOnAlert };

/** A class for regular cubes of terrain geometry. */
class Chunk  {
public:
	Chunk();
	void setSamplePos(glm::vec3& pos);
	void init();
	void setCreatorSC(CSuperChunk* creator);
	void getSkinned();
	void setOverlap(Tdirection overlap);

	glm::vec3 samplePos; ///<Position where this chunk starts sampling volumetric data.
	glm::vec4 colour; ///Colour chunk will be shaded in.
	float cubeSize;
	int LoD;

	glm::i32vec3 scIndex; ///<Position in a virtual 3D array.

	int tag;

	int noAttribs;
	int noTris;

	glm::vec3 terrainPos;

	unsigned int id;

	Tdirection overlapDir; ///<Direction of any SC overlapped by this chunk.

	TChunkStatus status;

private:
	CSuperChunk* creatorSC;
};


