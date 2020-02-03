#pragma once

#include "glm/glm.hpp"

#include "..\direction.h"
#include "scArray.h"



class CShellIterator;
class COuterSCIterator;
class CFaceIterator;
class CTerrain2;  ///TO DO get rid of dependency!

class ITerrainCallback;
class CShell {
public:
	CShell(int LoD, float chunkSize, int SCsize, int shellSCs);
	void init();
	void setTerrainCallbackObj(ITerrainCallback* obj);
	void onPlayerAdvance(Tdirection direction);
	void createTerrain();
	void initChunkExtent();
	
	void rotateSCs(Tdirection direction);
	void initSuperChunks();
	glm::vec3  calcSCsampleSpacePosition(glm::i32vec3& scIndex);
	void findAllSCchunks();
	void fillAllUnclippedSCs();
	CShellIterator getIterator();
	COuterSCIterator getOuterSCiterator();
	CFaceIterator getFaceIterator(Tdirection face);
	TBoxVolume getInnerBounds();
	void reinitialiseFaceSCs(Tdirection face);
	void addChunksToFaceSCs2(Tdirection face);
	void removeEncroachedOnChunks2(Tdirection face);
	void removeScrolledOutChunks(Tdirection face);
	void addInnerFaceChunks2(Tdirection face);

	TBoxVolume calcInnerFaceSCVolume(Tdirection face);

	TBoxVolume getInnerSCs();

	glm::i32vec3 getSCat(const glm::vec3& pos);

	CBoxVolume getChunkVolume2();

	void reinitialiseInnerSCs();

	glm::i32vec3 getRotatedIndex(const glm::i32vec3& origIndex);

	glm::i32vec3 getInvRotatedIndex(const glm::i32vec3& origIndex);

	int LoD; //<1=highest, then 2,4,8, etc
	int numSCchunks; //<SC size in chunks.
	int shellSCs; //<shell size in superChunks.

	float chunkSize; //<In worldspace.
	float SCsize; //<In worlsspace
	float worldSpaceSize; 
	glm::vec3 worldSpacePos;
	int minRequiredChunkExtent; //<Terrain must always extend this far from player.
	int maxChunkExtent; //<Maximum chunks that can extend from the origin before reaching the shell boundary.

	int chunkExtent[6]; //<How far chunks extend from origin in each direction
	bool faceLayerFull[6]; //<True if there's no room to add chunks to this face layer of SCs

	CTerrain2* pTerrain; ///<Pointer to parent terrain object
	unsigned int shellNo;

	CSCarry scArray;
	float scSampleStep;


	TBoxVolume innerBounds;


private:
	int chunkExtentFromViewpoint(Tdirection direction);
	void addToFaceLayer(Tdirection direction);

	void scroll(Tdirection direction);

	ITerrainCallback* pTerrainObj; ///<Points to terrain callback object.

};



class CShellIterator  {
public:
	CShellIterator(CShell* pShell);
	CSuperChunk2* SC();
	CShellIterator operator++();
	CShellIterator operator++(int);

	CSuperChunk2 operator*();
	CSuperChunk2* operator->();

	bool finished();

	glm::i32vec3 getIndex();

//protected:
	CSuperChunk2* pSC;
	glm::i32vec3 index;
	glm::i32vec3 max;
	CShell* pShell
;



};

/** An iterator that only returns SCs not overlaid by an 
	inner shell. */
class COuterSCIterator : public CShellIterator {
public:
	COuterSCIterator(CShell* pShell);
	COuterSCIterator  operator++();
	COuterSCIterator operator++(int);
private:
	TBoxVolume innerBounds;
};

/** An iterator that only returns the outermost layer of SCs in the given direction. */
class CFaceIterator : public CShellIterator {
public:
	CFaceIterator(CShell* pShell, Tdirection face);
	CFaceIterator  operator++();
	CFaceIterator operator++(int);
	glm::i32vec3 getIndex();
//private:
	int pseudoX, pseudoY, pseudoZ;
	int pseudoZValue;
};

glm::i32vec3 getSCat(glm::vec3& pos);
