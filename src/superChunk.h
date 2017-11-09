#pragma once

#include "renderer/3dobject.h"
#include "vertBufs.h"
#include "Delegate.h"
#include "terrain.h"
#include "direction.h"
#include "chunk.h"

#include <glm/glm.hpp>

#include <vector>



enum  TremoveOutgoing{ waitToRemove, freeToRemove};

/** A class for creating and managing 3D terrain. */

class CTerrain;
class CSuperChunk {
public:
	CSuperChunk();
	~CSuperChunk();
	void setSizes(glm::i32vec3& _sizeInChunks, int _cubesPerChunkEdge, float _cubeSize);
	void setSamplePos(glm::vec3& pos);
	void createAllChunks();
	Chunk* createChunk(glm::i32vec3& chunkPosition, Tdirection overlap);
	void setView(glm::vec3& pos); 
	void draw();
	void scroll(glm::i32vec3& scrollVec);
	void removeFace(Tdirection faceDir);

	void addFace(Tdirection faceDir);
	void initFaceWalk(Tdirection faceDir);
	bool faceWalkNextSpace(glm::i32vec3& index);
	bool faceWalkNextChunk(Chunk* &chunk);
	void removeSClayer(CSuperChunk& superChunk, Tdirection faceDir);
	CSuperChunk*& adj(const Tdirection dir);
	CSuperChunk*&op(const Tdirection dir);
	void acquireChunks(Tdirection inFace);
	void extendChunkSpace(Tdirection face);
	void retractChunkSpace(Tdirection face);
	int firstEmptyLayer(Tdirection face);
	int outerLayer(Tdirection face);
	void removeChunk(Chunk* chunk);
	void removeOutscrolledChunks(Tdirection faceDir);
	void addTwoIncomingLayers(Tdirection faceDir, Tdirection xStart, Tdirection yStart);
	void skinChunk(Chunk* chunk);
	void removeAllChunks();
	void raiseOverlapCount(int chunks, Tdirection faceDir);
	void overlapAlert(Tdirection overlap);
	bool inFrontOfPlane(glm::vec3 & planePos, glm::vec3 & planeNormal);
	bool isOutsideFustrum(glm::mat4& mvp);

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


	
	int chunksToSkin; ///<Number of chunks currently queued for skinning.

	
	int overlapCount; ///<Number of smaller SCs now overlapping this one.

	int layerNo;

	float sampleStep;
	bool nonEmpty;

private:
	glm::i32vec3 faceWalkPos, faceWalkStart, faceWalkEnd;
	Tdirection faceWalkDir;
	std::vector<Chunk*>::iterator faceWalkIt;
};

