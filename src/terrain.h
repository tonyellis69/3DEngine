#pragma once

#include "superChunk.h"
#include "vertBufs.h"
#include "Delegate.h"
#include <glm/glm.hpp>
#include "chunk.h"
//#include "cnode.h"
#include "direction.h"

#include <vector>
#include <set>



/** A class for creating and managing a terrain made out of superchunks. */

class CSuperChunk;

class CTerrainLayer;

class CReplaceChunk;

class CSkinningOrder;

typedef std::vector<std::vector<std::vector<CSuperChunk*>>> T3dArray;

class CTerrain : public C3dObject  {
public:
	CTerrain();
	void setSizes(int _chunksPerSChunkEdge, int _cubesPerChunkEdge, float _cubeSize);
	void createLayers(int layerSize, int noLayers, int layerThickness);
	void createAllChunks();
	void resize3dArray(T3dArray &scArray, glm::i32vec3& size);
	void createSuperChunks(T3dArray &scArray, std::vector<CSuperChunk*>& container);
	void initSuperChunks(T3dArray &scArray, int layerNo, glm::vec3 nwLayerSamplePos,glm::i32vec3& nodeGridSize);
	void findLayerFaces(T3dArray &scArray, int layerNo);
	void createInnerLayer(T3dArray xArray, int scPerEdge, glm::vec3& nwLayerPos, glm::vec3& sampleStart);
	void connectSuperChunks(T3dArray &tmpArray);
	void insertLayer(T3dArray &outerArray, T3dArray& innerArray, int outerLayerThickness);
	void hollowLayer(T3dArray &layerArray, int layerNo, int depth);
	Chunk* getFreeChunk();
	void freeChunk(Chunk* chunk);
	void update();
	void advance(Tdirection direction);
	bool superChunkIsEmpty(glm::vec3& samplePos, int LoD);
	bool chunkExists(glm::vec3& samplePos,int LoD);
	float* getSampleData(glm::vec3& samplePos);
	void createChunkMesh(Chunk& chunk);
	void freeChunkModel(CModel* chunk);
	void extend(int layerNo, Tdirection face);
	void shortenOutgoing(int layerNo, Tdirection face);
	void replacementCheck(glm::vec3& pos, Chunk* chunk,CSuperChunk* replacedSC); 
	~CTerrain();

	std::vector<Chunk*> spareChunks; ///<Stores unused chunks to recycle.

	DelegatePP<bool,glm::vec3&,int> EXTsuperChunkIsEmpty;
	DelegatePP<bool,glm::vec3&, int> EXTchunkExists;
	DelegateP<void,CModel*> EXTfreeChunkModel;
	DelegateP<void,Chunk&> EXTcreateChunkMesh;


	glm::i32vec3 sizeInChunks; ///<Width, height and depth in chunks.
	glm::vec3 worldSize; ///<width, height and depth in world space.
	int cubesPerChunkEdge; ///<Number of cubes per edge of a chunk.
	float LoD1cubeSize; ///<Size of smallest marching cubes in world space units.
	int chunksPerSChunkEdge;
	float chunkSize;///<Size of a chunk in world space, ie, cubesPerEdge * cubeSize.
	int totalChunks; ///<Maximum possible chunks in the chunk array

	std::vector<CSkinningOrder> toSkin; ///<List of chunks that need a mesh created.


	int totalTris;

	std::vector<Chunk*> allChunks; ///<Pointers to all the chunks of this terrain, in use or not. 

	std::vector<CTerrainLayer> layers; ///<A simple list of all the layers of this terrain.

	std::vector<CReplaceChunk> replaceChunks; ///<A list of all chunks scheduled for removal.

	};

/** Represents one of a terrain's concentric layers of superchunks. */
class CTerrainLayer {
public:
	CTerrainLayer();
	bool advance(glm::i32vec3& scrollVec);
	void scroll(glm::i32vec3& scrollVec);
	std::vector<CSuperChunk*> superChunks;
	glm::vec3 nwLayerPos;
	float cubeSize;
	int LoD;
	glm::i32vec3 scrollState; ///<Tracks how close layer is to scrolling in any direction.
	
	bool shifted[6];
	float LoD1cubeSize;
	float cubesPerChunkEdge;

	std::vector<CSuperChunk*> faceGroup[6];
};

/** Records what higher detail chunks need to be complete before a lower-
	detail chunk can be removed. */
class CReplaceChunk {
public:
	CReplaceChunk()  {subChunksFound = 0; };
	Chunk* oldChunk; ///<Chunk we're replacing.
	glm::vec3 nwCorner; ///<nw lower corner of oldChunk's region of world space.
	glm::vec3 opCorner; ///<se upprt corner of oldChunk's region of world space.
	int subChunksFound; ///<Number of subchunks found occupying same space;
	Chunk* newChunk[8]; ///<The chunks replacing this larger chunk.
	CSuperChunk* parentSC;
};

class CSkinningOrder {
public:
	CSuperChunk* replaceSC;
	Chunk* chunk;
};


