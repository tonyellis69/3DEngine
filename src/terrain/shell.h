#pragma once

#include "glm/glm.hpp"

#include "..\direction.h"
#include "scArray.h"



class CShellIterator;
class COuterSCIterator;
class CFaceIterator;
class CTerrain2;
class CShell {
public:
	CShell(int LoD, float chunkSize, int SCsize, int shellSCs);
	void playerAdvance(Tdirection direction);
	void handleInnerShellAdvance(Tdirection direction);
	int getPlayerChunkExtent(Tdirection direction);
	void fillEntire();
	void initChunkExtent();
	void addToFaceLayer(Tdirection direction);
	void scroll(Tdirection direction);
	void initSuperChunks();
	glm::vec3 & calcSCsampleSpacePosition(glm::i32vec3& scIndex);
	void findAllSCintersections();
	void findAllSCchunks();
	CShellIterator& getIterator();
	COuterSCIterator& getOuterSCiterator();
	CFaceIterator& getFaceIterator(Tdirection face);
	TBoxVolume& getInnerBounds();
	void reinitialiseFaceSCs(Tdirection face);
	void setSCchunkBoundaries();
	void addChunksToFaceSCs(Tdirection direction);
	void removeEncroachedOnChunks(Tdirection face);
	TBoxVolume& transformToLocalSpace(TBoxVolume& innerShellChunkVolume, glm::i32vec3& offset);
	void removeScrolledOutChunks(Tdirection face);
	void addInnerFaceChunks(Tdirection innerFace);

	TBoxVolume calcInnerFaceSCVolume(Tdirection face);

	TBoxVolume getChunkVolume();

	int LoD; //<1=highest, then 2,4,8, etc
	int SCchunks; //<SC size in chunks.
	int shellSCs; //<shell size in superChunks.

	float chunkSize; //<In worldspace.
	float SCsize; //<In worlsspace
	float worldSpaceSize; 
	glm::vec3 worldSpacePos;
	int minimumChunkExtent; //<Terrain must always extend this far from player.
	glm::i32vec3 playerChunkPos;  //<Player positionHint in chunks, relative to origin

	int chunkExtent[6]; //<How far chunks extend from origin in each direction
	bool faceLayerFull[6]; //<True if there's no room to add chunks to this face layer of SCs

	CTerrain2* pTerrain; ///<Pointer to parent terrain object
	unsigned int shellNo;

	CSCarry scArray;
	float scSampleStep;

	glm::vec4 shellColour; ///!!!!!!!!!!!temp!

	TBoxVolume innerBounds;
};


class CShellIterator  {
public:
	CShellIterator(CShell* pShell);
	CSuperChunk2* SC();
	CShellIterator& operator++();
	CShellIterator operator++(int);

	CSuperChunk2& operator*();
	CSuperChunk2* operator->();

	bool finished();

	glm::i32vec3& getIndex();

protected:
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
	COuterSCIterator & operator++();
	COuterSCIterator operator++(int);
private:
	TBoxVolume innerBounds;
};

/** An iterator that only returns the outermost layer of SCs in the given direction. */
class CFaceIterator : public CShellIterator {
public:
	CFaceIterator(CShell* pShell, Tdirection face);
	CFaceIterator & operator++();
	CFaceIterator operator++(int);
	glm::i32vec3 getIndex();
private:
	int pseudoX, pseudoY, pseudoZ;
	int pseudoZValue;
};

