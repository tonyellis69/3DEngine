#pragma once

#include "renderer/3dobject.h"
#include "vertBufs.h"
#include "Delegate.h"
#include "terrain.h"
#include "direction.h"
#include "chunk.h"

#include <glm/glm.hpp>

#include <vector>

/** A class for creating and managing 3D terrain. */
//#include "cnode.h"



class CTerrain;
class CSuperChunk {
public:
	CSuperChunk();
	~CSuperChunk();
	void setSizes(glm::i32vec3& _sizeInChunks, int _cubesPerChunkEdge, float _cubeSize);
	void setSamplePos(glm::vec3 pos);
	void createChunks();
	Chunk* createChunk(glm::i32vec3& chunkPosition);
	void setView(glm::vec3& pos); 
	void draw();
	void scroll(glm::i32vec3& scrollVec);
	void shiftChunksBy1(glm::vec3 tVec);
	void removeFace(Tdirection faceDir);
	void addFace(Tdirection faceDir);
	void removeSClayer(CSuperChunk& superChunk, Tdirection faceDir);
	CSuperChunk*& adj(const Tdirection dir);
	CSuperChunk*&op(const Tdirection dir);
	void acquireChunks(Tdirection inFace);
	void extendBoundary(Tdirection face);
	void shrinkBoundary(Tdirection face);
	int firstEmptyLayer(Tdirection face);
	int outerLayer(Tdirection face);
	void flagFaceForRemoval(Tdirection face);
	void createChunkAsRequired(glm::i32vec3& pos, glm::vec3& samplePos,CSuperChunk* replaceSC);
	void removeChunk(Chunk* chunk);

	glm::i32vec3 sizeInChunks; ///<Width, height and depth in chunks.
	int cubesPerChunkEdge; ///<Number of cubes per edge of a chunk.
	float cubeSize; ///<Size of marching cubes in world space units. 
	float chunkSize;///<Size of a chunk in world space, ie, cubesPerEdge * cubeSize.

	glm::vec3 nwSamplePos; ///<Position of NW corner of terrain, in noise space.
	glm::vec3 nwWorldPos; ///<Position of NW corner of terrain in world space.

	glm::vec4 genColour; ///Colour of this generation of chunks

	CTerrain* terrain; ///<Pointer to parent terrain;

	CSuperChunk* nSChunk; ///<Points to north neighbour, if any.
	CSuperChunk* eSChunk;
	CSuperChunk* sSChunk;
	CSuperChunk* wSChunk;
	CSuperChunk* uSChunk;
	CSuperChunk* dSChunk;

	int LoD; ///<Level of detail.
	float LoDscale; ///<level of detail expressed as a square



	int tag;
	glm::i32vec3 tmpIndex;

	glm::vec3 scrollVec; ///<Simple vector giving current scroll direction

	std::vector<Chunk*> chunkList; ///<Chunks currently owned.

	int faceBoundary[6]; ///<Records the position of the current outermost layer of chunks on each face.

private:
	

};

