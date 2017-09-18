#pragma once

#include "superChunk.h"
#include "vertBufs.h"
#include "Delegate.h"
#include <glm/glm.hpp>
#include "chunk.h"
#include "renderer/renderer.h"
#include "direction.h"
#include "renderer\phongShader.h"

#include <vector>
#include <deque>
#include <queue>
#include <set>

//LOD counts down from the outmost layer towards the innermost, 1.
//However, in the layers array 0 is the outermost
//Maybe change this sometime?



/** A class for creating and managing a terrain made out of superchunks. */

class CSuperChunk;

class CTerrainLayer;


class Chunk;



typedef std::vector<std::vector<std::vector<CSuperChunk*>>> T3dArray;
typedef vBuf::T3DnormVert TChunkVert;

class TChunkTriBuf {
public:
	TChunkTriBuf() { id = noTris = 0; /*buf = NULL;*/ }
	unsigned int id;
	unsigned int noTris;
	TChunkVert buf[4200];
};

const int chunkTriCacheSize = 6;


class CTerrain : public CModel  {
public:
	CTerrain();
	void reserveBuf(unsigned int elementsUsed);
	void setSizes(int _chunksPerSChunkEdge, int _cubesPerChunkEdge, float _cubeSize);
	void createLayers(float terrainSize, float LoD1extent, int steps);
	void createAllChunks();
	void resize3dArray(T3dArray &scArray, glm::i32vec3& size);
	void createSuperChunks(T3dArray &scArray, std::vector<CSuperChunk*>& container);
	void initSuperChunks(T3dArray &scArray, int layerNo, glm::i32vec3& nodeGridSize);
	void findLayerFaces(T3dArray &scArray, int layerNo);
	void createInnerLayer(T3dArray xArray, int scPerEdge, glm::vec3& nwLayerPos, glm::vec3& sampleStart);
	void connectSuperChunks(T3dArray &tmpArray);
	void insertLayer(T3dArray &outerArray, T3dArray& innerArray, int outerLayerThickness);
	void hollowLayer(T3dArray &layerArray, int layerNo, int depth);
	Chunk* getFreeChunk();
	void prepareToFree(Chunk* chunk);
	void update();
	void advance(Tdirection direction);
	virtual bool superChunkIsEmpty(CSuperChunk& SC) { return false; };
	virtual bool chunkExists(glm::vec3& samplePos,int LoD) { return false; };
	void createChunkMesh(Chunk& chunk);
	void freeChunkModel(CModel* chunk);
	void addTwoIncomingLayers(int layerNo, Tdirection face);
	void newChunkRequest(glm::vec3& samplePos, CSuperChunk* parentSC, glm::i32vec3& index);
	void handleNextChunkRequest();
	void freeChunk(Chunk& chunk);
	CSuperChunk* getSC(const glm::vec3& pos);
	Chunk* getChunk(const glm::vec3& pos);
	void getChunkTris(Chunk& chunk, TChunkVert* buf);
	void getTris(const glm::vec3& pos, TChunkVert* &buf, unsigned int& noTris);
	glm::vec3 getChunkPos(const glm::vec3& pos);
	void clear();
	void setSampleCentre(glm::vec3& centrePos);
	void updateVisibleSClist(glm::mat4& camMatrix);
	void drawVisibleChunks();
	void drawGrass(glm::mat4 & mvp, std::vector<CSuperChunk*>& drawList);
	void drawTrees(glm::mat4 & mvp, std::vector<CSuperChunk*>& drawList);
	void draw();
	~CTerrain();

	CMultiBuf multiBuf;
	CRenderer* pRenderer;

	std::vector<Chunk*> spareChunks; ///<Stores unused chunks to recycle.

	DelegateP<bool, CSuperChunk&> EXTsuperChunkIsEmpty;
	DelegatePP<bool,glm::vec3&, int> EXTchunkExists;
	DelegateP<void,CModel*> EXTfreeChunkModel;
	DelegateP<void,Chunk&> EXTcreateChunkMesh;


	glm::i32vec3 sizeInChunks; ///<Width, height and depth in chunks.
	glm::vec3 worldSize; ///<width, height and depth in world space.
	int cubesPerChunkEdge; ///<Number of cubes per edge of a chunk.
	float LoD1cubeSize; ///<Size of smallest marching cubes in world space units.
	int chunksPerSChunkEdge;
	int totalChunks; ///<Maximum possible chunks in the chunk array

	std::deque<Chunk*> toSkin; ///<List of chunks that need a mesh created.


	int totalTris;

	std::vector<Chunk*> allChunks; ///<Pointers to all the chunks of this terrain, in use or not. 

	std::vector<CTerrainLayer> layers; ///<A simple list of all the layers of this terrain.

	glm::mat4 chunkOrigin; ///<Chunks are drawn relative to this point.
	glm::vec3 scrollTriggerPoint; ///<Tracks how close terrain is to scrolling in any direction.
	glm::i32vec3 chunkOriginInt;

	//float sampleScale; ///<ratio of sample space to world space, eg, sample space units to metres.
	
	float chunkProcessDelay;

	glm::vec3 tmp;

	std::vector<Chunk*> toFree; ///<List of chunks to be taken out of play.

	int totalSCs;

	TChunkTriBuf cachedChunkTris[chunkTriCacheSize]; ///<Chunk triangles recently downloaded from graphics memory.
	int freeChunkTriCache;

	float worldUnitsPerSampleUnit;

	CMultiBuf grassMultiBuf; ///<Stores grass location points for all chunks.

	std::vector<CSuperChunk*> visibleSClist; ///<SCs considered to be in sight.
	CPhongShader* chunkDrawShader; ///<Whatever shader we're using to draw chunks.
	};



/** Represents one of a terrain's concentric layers of superchunks. */
class CTerrainLayer {
public:
	CTerrainLayer();
	bool advance(glm::i32vec3& scrollVec);
	void scroll(glm::i32vec3& scrollVec);
	Chunk* getChunkAt(glm::vec3& pos);
	CSuperChunk* getNearestSC(glm::vec3& pos);
	bool resetCheck(glm::i32vec3& scrollVec);

	std::vector<CSuperChunk*> superChunks;
	glm::vec3 nwLayerPos;
	float cubeSize;
	int LoD;
	glm::i32vec3 scrollState; ///<Tracks how close layer is to scrolling in any direction.
	
	static float LoD1chunkSize;
	float scSize;

	std::vector<CSuperChunk*> faceGroup[6];
	glm::i32vec3 resetState; ///<Tracks how close layer is to returning to its reset position.
	float SCsampleStep;
	float chunkSampleStep;
	glm::vec3 nwSampleCorner;
};



