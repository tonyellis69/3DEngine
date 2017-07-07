#include "superChunk.h"
#include "colour.h"
#include <iostream> //for cerr
#include <algorithm>

#include "chunk.h"

using namespace glm;
using namespace vBuf;
using namespace std;



CSuperChunk::CSuperChunk()  {
	nSChunk = eSChunk = sSChunk = wSChunk = uSChunk = dSChunk = NULL;
	LoD = 0;
	overlapCount = 0;
	chunksToSkin = 0;
}

/** Set the dimensions and building block sizes of this superChunk. */
void CSuperChunk::setSizes(i32vec3& _sizeInChunks,int _cubesPerChunkEdge, float _cubeSize) {
	cubesPerChunkEdge = _cubesPerChunkEdge;
	cubeSize = _cubeSize;
	chunkSize = cubesPerChunkEdge * cubeSize;

	//set the extents of the superChunk
	sizeInChunks = _sizeInChunks;
	faceBoundary[north] = faceBoundary[west] = faceBoundary[down] = 0;
	faceBoundary[east] = sizeInChunks.x-1;
	faceBoundary[south] = sizeInChunks.z-1;
	faceBoundary[up] = sizeInChunks.y-1;

}

/** Set the position of the northwest bottom corner of this superChunk in sample space. */
void CSuperChunk::setSamplePos(vec3& pos) {
	nwSamplePos = pos;
}


/** Create all the chunks for this superchunk where they are intersected by the isosurface. Each chunk is put on the list for
	skinning*/
void CSuperChunk::createAllChunks() {
	//first, rule out this superChunk altogether if possible
	//if (terrain->superChunkIsEmpty(nwSamplePos,LoD))
	//	return;
	//TO DO: faulty! Needs a look at

	for (int x=0;x<sizeInChunks.x;x++) {
		for (int y=0;y<sizeInChunks.y;y++) {
			for (int z=0;z<sizeInChunks.z;z++) {
				createChunk(i32vec3(x,y,z),none);
			}
		} 
	}
}


extern int uniTag = 0;
/** Return a pointer to a chunk that has been positioned at the given chunk position and initialised for skinning.*/
Chunk* CSuperChunk::createChunk(i32vec3& gridPosition, Tdirection overlap) {
	vec3 samplePos = nwSamplePos + vec3(gridPosition) * (cubesPerChunkEdge * LoDscale) * terrain->sampleScale;
	Chunk* newChunk = terrain->getFreeChunk();
	vec3 layerPos = terrain->layers[layerNo].nwLayerPos;
	newChunk->terrainPos = layerPos + nwWorldPos + (vec3(gridPosition)*chunkSize) - vec3(terrain->chunkOrigin[3]);
	newChunk->cubeSize = cubeSize;
	newChunk->setSamplePos(samplePos);
	newChunk->colour = genColour;
	newChunk->LoD = LoD;
	newChunk->scIndex = gridPosition;
	newChunk->tag = uniTag++;

	newChunk->overlapDir = overlap;
	newChunk->setCreatorSC(this);
	chunkList.push_back(newChunk);
	terrain->toSkin.push_back(newChunk);
	newChunk->status = chToSkin;
	return newChunk;
}


/** Scroll by one chunk in the given direction. */
void CSuperChunk::scroll(i32vec3& scrollVec) {
	//when we scroll, superchunks stay where they are but their sample point changes
	nwSamplePos += vec3(scrollVec) * (float)cubesPerChunkEdge * LoDscale * terrain->sampleScale;
	
	//find which face is being scrolled out, and the opposite 'in' face receiving a new layer of nodes.
	//NB if scrolling in new terrain from north, dir is -1
	Tdirection outFace,inFace;
	inFace = vecToDir(scrollVec);
	outFace = flipDir(inFace);
	
	if (adj(outFace) == NULL) //if outface direction doesn't lead to another superchunk
		removeFace(outFace);
	 else if (adj(outFace)->LoD < LoD) { //we're outscrolling chunks into a superChunk of higher detail
		 ;// shrinkIfEmpty(outFace);  //
	}

	if (adj(inFace) == NULL || adj(inFace)->LoD < LoD) { //we're inscrolling chunks from outside terrain or a superChunk of higher detail
		addFace(inFace);
	} 
	else if (adj(inFace)->LoD == LoD)
		acquireChunks(inFace);
		
	//assign a random colour to this new layer
	genColour = vec4(col::randHue(),1);
}



/** Remove the out layer of chunks from this superChunk corresponding to the given face. */
void CSuperChunk::removeFace(Tdirection faceDir) {
	Chunk* chunk;
	initFaceWalk(faceDir);
	while (faceWalkNextChunk(chunk)) {
		terrain->prepareToFree(chunk);
	}

	retractChunkSpace(faceDir); 
}





/**	Add a layer of new chunks to the face indicated. */
void CSuperChunk::addFace(Tdirection faceDir) {
	chunksToSkin = 0;

	Tdirection overlap;
	if (adj(faceDir)) //catch SCs at very edge of terrain. 
		overlap = faceDir;
	else
		overlap = none;

	int chunkCount = 0;
	i32vec3 index;
	initFaceWalk(faceDir);
	while (faceWalkNextSpace(index)) {
		createChunk(index, overlap);
		chunkCount++;
	}

	extendChunkSpace(faceDir);//because we've added a layer of chunks to this face

	if (overlap == none)
		return;
	Tdirection overlapedFace = flipDir(faceDir);
	adj(faceDir)->raiseOverlapCount(chunkCount, overlapedFace);
}

void CSuperChunk::initFaceWalk(Tdirection faceDir) {
	Tdirection xStart, xEnd, yStart, yEnd;
	xStart = getXstart(faceDir); xEnd = flipDir(xStart);
	yStart = getYstart(faceDir); yEnd = flipDir(yStart);

	faceWalkStart.x = outerLayer(xStart); faceWalkStart.y = outerLayer(yStart);
	faceWalkEnd.x = outerLayer(xEnd); faceWalkEnd.y = outerLayer(yEnd);
	faceWalkDir = faceDir;
	faceWalkPos = faceWalkStart;
	faceWalkPos.z = firstEmptyLayer(faceWalkDir);
	faceWalkIt = chunkList.begin();
}

bool CSuperChunk::faceWalkNextSpace(i32vec3& index) {
	index = rotateByDir(faceWalkPos, faceWalkDir);
	if (faceWalkPos.y > faceWalkEnd.y)
		return false;
	faceWalkPos.x++;
	if (faceWalkPos.x > faceWalkEnd.x) {
		faceWalkPos.x = faceWalkStart.x;
		faceWalkPos.y++;
	}
	return true;
}

bool CSuperChunk::faceWalkNextChunk(Chunk* &chunk) {
	if (faceWalkIt == chunkList.end()) 
		return false;
	int zAxis = getAxis(faceWalkDir);
	int zPos = outerLayer(faceWalkDir);
	while ((*faceWalkIt)->scIndex[zAxis] != zPos) {
		faceWalkIt++;
		if (faceWalkIt == chunkList.end())
			return false;
	}
	chunk = *faceWalkIt;
	faceWalkIt = chunkList.erase(faceWalkIt);
	return true;
}




/** Return the adjacent superchunk in the given direction, if any.*/
CSuperChunk*& CSuperChunk::adj(const Tdirection dir) {
	switch (dir) {
	case (north) : return nSChunk;
	case (south) : return sSChunk;
	case (east) : return eSChunk;
	case (west) : return wSChunk;
	case (up) : return uSChunk;
	case (down) : return dSChunk;
	}
}

/** Return the adjacent superchunk in the opposite direction to the one given.*/
 CSuperChunk*& CSuperChunk::op(const Tdirection dir) {
	switch (dir) {
	case (north) : return sSChunk;
	case (south) : return nSChunk;
	case (east) : return wSChunk;
	case (west) : return eSChunk;
	case (up) : return dSChunk;
	case (down) : return uSChunk;
	}
 }

 /** Acquire as a new face layer in the inFace direction, the face chunks (if any), of the neighbouring superchunk
	in that direction.*/
 void CSuperChunk::acquireChunks(Tdirection inFace) {

	 Tdirection outFace = flipDir(inFace);

	 //get a pointer to the adjacent superchunk
	 CSuperChunk* giver = adj(inFace);

	 int zAxis = getAxis(inFace);
	 int outFaceZ = giver->outerLayer(outFace);
	 int inFaceZ = firstEmptyLayer(inFace);

		 //for each chunk in the giver's out face...
		 vector<Chunk*>::iterator it;
		 for (it = giver->chunkList.begin(); it != giver->chunkList.end();) {
			 if (((*it)->scIndex[zAxis] == outFaceZ) && (*it)->status != chRemoveOnAlert) {
				 (*it)->scIndex[zAxis] = inFaceZ;
				 chunkList.push_back((*it));
				 it = giver->chunkList.erase(it);
			 }
			 else
				 ++it;
		 }
		 extendChunkSpace(inFace);
		 giver->retractChunkSpace(outFace);
 }


/** Extend the recogised position for outermost chunks in the face direction. */
void CSuperChunk::extendChunkSpace(Tdirection face) {
	if ((face == north)||(face == west) || (face == down))
		faceBoundary[face] -= 1;
	else
		faceBoundary[face] += 1;
}

extern CSuperChunk* dbgSC;

/** Shrink the recogised position for outermost chunks in the face direction. */
void CSuperChunk::retractChunkSpace(Tdirection face) {

	if ((face == north)||(face == west) || (face == down))
		faceBoundary[face] += 1;
	else
		faceBoundary[face] -= 1;
}

/** Return the position of the first layer beyond the outer, face layer of chunks in the given direction. */
int CSuperChunk::firstEmptyLayer(Tdirection face) {
	return faceBoundary[face] + getSign(face);
}

/** Return the position of the outermost layer of chunks in the face direction. */
int CSuperChunk:: outerLayer(Tdirection face) {
	return faceBoundary[face];
}



/** Find and remove the given chunk from this superChunk's control. */
void CSuperChunk::removeChunk(Chunk* chunk) {
	for (size_t chk=0;chk<chunkList.size();chk++)
		if (chunkList[chk] == chunk) {
			terrain->prepareToFree(chunk);
			chunkList.erase(chunkList.begin() + chk);
			break;
		}
}

/** Remove the two layers of chunks now outscrolled and replaced by an L2 scroll.*/
void CSuperChunk::removeOutscrolledChunks(Tdirection faceDir) {
	removeFace(faceDir);
	removeFace(faceDir);
}



/** Create two new layers of chunks on the given face. This happens when the superChunk has scrolled and we want to replace the lower-LoD chunks of the 
	adjacent next-layer superChunk, which are currently intruding into our space. */
void CSuperChunk::addTwoIncomingLayers(Tdirection faceDir, Tdirection xStart, Tdirection yStart) {

	int chunkCount = 0;

	i32vec3 facePos;
	initFaceWalk(faceDir);
	while (faceWalkNextSpace(facePos)) {
		createChunk(facePos, faceDir);
		chunkCount++;
	}
	extendChunkSpace(faceDir);

	initFaceWalk(faceDir);
	while (faceWalkNextSpace(facePos)) {
		createChunk(facePos, faceDir);
		chunkCount++;
	}
	extendChunkSpace(faceDir);
	//sizeInChunks[zAxis] += 1;

	adj(faceDir)->raiseOverlapCount(chunkCount, flipDir(faceDir));
}

/**	Attempt to create a block of terrain geometry for this chunk. If the terrain surface doesn't pass through it, it's empty and we can discard it.
	When all the chunks of this SC have been processed, we deal with any neighbour SC that may now be overlapped. */
void CSuperChunk::skinChunk(Chunk * chunk) {
	if (chunk->status == chToSkin)
		terrain->createChunkMesh(*chunk);
	//else
	//	chunk->id = 0;
	if (chunk->id == 0)
		removeChunk(chunk);
//	else
//		chunk->live = true;


	//do the overlapped-neighbour notification as needed
	if (chunk->overlapDir != none) {
		adj(chunk->overlapDir)->overlapAlert(chunk->overlapDir);


	}
	
}



void CSuperChunk::removeAllChunks() {
	for (int c = 0; c < chunkList.size(); c++) {
		int id = chunkList[c]->id;
		if (id)
			terrain->multiBuf.deleteBlock(id);
	}
	
}

void CSuperChunk::raiseOverlapCount(int chunks, Tdirection faceDir) {
	if (overlapCount == 0) {
		int zAxis = getAxis(faceDir);
		int zPos = outerLayer(faceDir);
		vector<Chunk*>::iterator it;
		for (int ch = 0; ch < chunkList.size(); ch++) {
			if (chunkList[ch]->scIndex[zAxis] == zPos) {
				chunkList[ch]->status = chRemoveOnAlert;
			}
		}
	}

	overlapCount += chunks;
}

void CSuperChunk::overlapAlert(Tdirection overlap) {
	overlapCount--;
	if (overlapCount == 0) { //any overlaping SC is now fully drawn - this SC is fully overlapped
		Tdirection overlapDir = flipDir(overlap);
		int overlapLoD = adj(overlapDir)->LoD;
		if (overlapLoD < LoD) //we're a large SC overlapped by smaller SCs
			//	removeTaggedFaceChunks(overlapDir);
			removeFace(overlapDir);
		else { //we're one of 4 smaller SCs overlapped by a larger
			removeOutscrolledChunks(overlapDir);
			adj(getXdir(overlapDir))->removeOutscrolledChunks(overlapDir);
			adj(getYdir(overlapDir))->removeOutscrolledChunks(overlapDir);
			adj(getYdir(overlapDir))->adj(getXdir(overlapDir))->removeOutscrolledChunks(overlapDir);
		}
	}
}


CSuperChunk::~CSuperChunk() {
	for (size_t c=0;c<chunkList.size();c++) {
		delete chunkList[c];
	}
}






