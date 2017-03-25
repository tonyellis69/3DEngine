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

//LOD counts down from the outmost layer towards the innermost, 1.
//However, in the layers array 0 is the outermost
//Maybe change this sometime?



/** A class for creating and managing a terrain made out of superchunks. */

class CSuperChunk;

class CTerrainLayer;



class CSkinningOrder;

typedef std::vector<std::vector<std::vector<CSuperChunk*>>> T3dArray;

extern CSuperChunk*  dbgSC;

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
	void createChunkMesh(Chunk& chunk);
	void freeChunkModel(CModel* chunk);
	void addTwoIncomingLayers(int layerNo, Tdirection face);
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

	glm::mat4 chunkOrigin; ///<Chunks are drawn relative to this point;
	};

/** Represents one of a terrain's concentric layers of superchunks. */
class CTerrainLayer {
public:
	CTerrainLayer();
	bool advance(glm::i32vec3& scrollVec);
	void scroll(glm::i32vec3& scrollVec);
	Chunk* getChunkAt(glm::vec3& pos);
	CSuperChunk* getNearestSC(glm::vec3& pos);
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


class CSkinningOrder {
public:
	CSuperChunk* parentSC;
	Chunk* chunk;
	//CSuperChunk* overlappedSC;
};


