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
	overlapDir = none;
	overlapCount = 4;
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
	setOverlapDir(none);
	//first, rule out this superChunk altogether if possible
	if (terrain->superChunkIsEmpty(nwSamplePos,LoD))
		return;

	for (int x=0;x<sizeInChunks.x;x++) {
		for (int y=0;y<sizeInChunks.y;y++) {
			for (int z=0;z<sizeInChunks.z;z++) {
				createChunk(i32vec3(x,y,z));
			}
		} 
	}
}


extern int uniTag = 0;
/** Return a pointer to a chunk that has been positioned at the given chunk position and initialised for skinning.*/
Chunk* CSuperChunk::createChunk(i32vec3& gridPosition) {
	vec3 samplePos = nwSamplePos + vec3(gridPosition) * (cubesPerChunkEdge * LoDscale);
	Chunk* newChunk = terrain->getFreeChunk();
	newChunk->terrainPos = vec3(nwWorldPos + (vec3(gridPosition)*chunkSize) - vec3(terrain->chunkOrigin[3]));
	newChunk->cubeSize = cubeSize;
	newChunk->setSamplePos(samplePos);
	newChunk->colour = genColour;
	newChunk->LoD = LoD;
	newChunk->scIndex = gridPosition;
	newChunk->tag = uniTag++;

	newChunk->setCreatorSC(this);
	chunkList.push_back(newChunk);
	chunksToSkin++;
	terrain->toSkin.push_back(newChunk);
	return newChunk;
}


/** Scroll by one chunk in the given direction. */
void CSuperChunk::scroll(i32vec3& scrollVec) {
	//when we scroll, superchunks stay where they are but their sample point changes
	nwSamplePos += vec3(scrollVec) * (float)cubesPerChunkEdge * LoDscale;
	
	//find which face is being scrolled out, and the opposite 'in' face receiving a new layer of nodes.
	//NB if scrolling in new terrain from north, dir is -1
	Tdirection outFace,inFace;
	inFace = vecToDir(scrollVec);
	outFace = flipDir(inFace);
	
	if (adj(outFace) == NULL) //if outface direction doesn't lead to another superchunk
		removeFace(outFace);
	 else if (adj(outFace)->LoD < LoD) { //we're outscrolling chunks into a superChunk of higher detail
		shrinkIfEmpty(outFace);  //
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
	int zAxis = getAxis(faceDir);
	int zPos = outerLayer(faceDir);
	vector<Chunk*>::iterator it;
	for (it = chunkList.begin();it != chunkList.end();) {
		if ((*it)->scIndex[zAxis] == zPos) {
			terrain->freeChunk(*it);
			it = chunkList.erase(it);	
		}
		else
			++it;
	}
	shrinkBoundary(faceDir); ///////////////////////////////////////////
}


/**	Add a layer of new chunks to the face indicated. */
void CSuperChunk::addFace(Tdirection faceDir) {
	chunksToSkin = 0;
	//for every potenitial chunk position in the 'in' face, check for a chunk.
	i32vec3 facePos(0); 

	int xAxis = getXaxis(faceDir);
	int yAxis = getYaxis(faceDir);

	Tdirection xStart,xEnd,yStart,yEnd;
	xStart = getXstart(faceDir); xEnd = flipDir(xStart);
	yStart = getYstart(faceDir); yEnd = flipDir(yStart);

	facePos.z = firstEmptyLayer(faceDir);
	for (facePos.x=outerLayer(xStart);facePos.x<=outerLayer(xEnd);facePos.x++) {
		for (facePos.y=outerLayer(yStart);facePos.y<=outerLayer(yEnd);facePos.y++) {
			i32vec3 realPos = rotateByDir(facePos,faceDir);
			createChunk(realPos);
		}
	}
	extendBoundary(faceDir);//because we've added a layer of chunks to this face

	if (adj(faceDir)) //catch SCs at very edge of terrain. 
		setOverlapDir(faceDir);
	else
		setOverlapDir(none);
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
	for (it = giver->chunkList.begin();it != giver->chunkList.end();) {
		if (((*it)->scIndex[zAxis] == outFaceZ)/*||( giver->sizeInChunks[zAxis] == 1 )*/) {
			(*it)->scIndex[zAxis] = inFaceZ;
			chunkList.push_back((*it));
			it = giver->chunkList.erase(it);	
		}
		else
			++it;
		}	
	extendBoundary(inFace);
	giver->shrinkBoundary(outFace);
 }


/** Extend the recogised position for outermost chunks in the face direction. */
void CSuperChunk::extendBoundary(Tdirection face) {
	if ((face == north)||(face == west) || (face == down))
		faceBoundary[face] -= 1;
	else
		faceBoundary[face] += 1;
}

/** Shrink the recogised position for outermost chunks in the face direction. */
void CSuperChunk::shrinkBoundary(Tdirection face) {
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

/** Ensure empty SCs have their boundary reshrunk - this happens automatically for SCs with chunkst to scroll. */
//TO DO: bit of a fudge, maybe ensure all shrinkage happens in the same place?
void CSuperChunk::shrinkIfEmpty(Tdirection face) {
	if (chunkList.size() == 0) { 
		shrinkBoundary(face);
	}
}

/** Remove the given chunk from this superChunk's control. */
void CSuperChunk::removeChunk(Chunk* chunk) {
	for (size_t chk=0;chk<chunkList.size();chk++)
		if (chunkList[chk] == chunk) {
			terrain->freeChunk(chunk);
			chunkList.erase(chunkList.begin() + chk);
			break;
		}
}

/** Remove the two layers of chunks now outscrolled and replaced by an L2 scroll.*/
void CSuperChunk::removeOutscrolledChunks(Tdirection faceDir) {
	removeFace(faceDir);
	removeFace(faceDir);
}

/** Set the direction in which the chunks this superchunk potentially overlap the chunks of another superChunk, ie, at layer borders during scrolling. */
void CSuperChunk::setOverlapDir(Tdirection dir) {
	overlapDir = dir;
}

/** Create two new layers of chunks on the given face. This happens when the superChunk has scrolled and we want to replace the lower-LoD chunks of the 
	adjacent next-layer superChunk, which are currently intruding into our space. */
void CSuperChunk::addTwoIncomingLayers(Tdirection faceDir, Tdirection xStart, Tdirection yStart) {
	int zAxis = getAxis(faceDir);
	extendBoundary(faceDir); extendBoundary(faceDir);
	sizeInChunks[zAxis] += 1;
	chunksToSkin = 0;

	int zStart = outerLayer(faceDir); int zEnd = zStart + 1;
	if ((faceDir == south) || (faceDir == east) || (faceDir == up)) {
		zEnd = outerLayer(faceDir); zStart = zEnd - 1;
	}

	i32vec3 facePos;
	Tdirection xEnd = flipDir(xStart);
	Tdirection yEnd = flipDir(yStart);
	for (facePos.z = zStart; facePos.z <= zEnd; facePos.z++) {
		//for each potential chunk space
		for (facePos.x = outerLayer(xStart); facePos.x <= outerLayer(xEnd); facePos.x++) {
			for (facePos.y = outerLayer(yStart); facePos.y <= outerLayer(yEnd); facePos.y++) {
				i32vec3 realPos = rotateByDir(facePos, faceDir);
				createChunk(realPos);
			}
		}
	}
	setOverlapDir(faceDir);
}

/**	Attempt to create a block of terrain geometry for this chunk. If the terrain surface doesn't pass through it, it's empty and we can discard it.
	When all the chunks of this SC have been processed, we deal with any neighbour SC that may now be overlapped. */
void CSuperChunk::skinChunk(Chunk * chunk) {
	terrain->createChunkMesh(*chunk);
	if (chunk->id == 0)
		removeChunk(chunk);
	else
		chunk->live = true;


	//do the overlapped-neighbour notification as needed
	chunksToSkin--;
	if (chunksToSkin == 0 && overlapDir != none) {
		CSuperChunk* overlappedNeighbour = adj(overlapDir);
		Tdirection faceDir = flipDir(overlapDir);
		if (overlappedNeighbour->LoD > LoD) //we're a smaller SC partially overlapping a larger
			overlappedNeighbour->incrementOverlap(faceDir);
		else { //we're a larger SC overlapping four smaller ones
			overlappedNeighbour->removeOutscrolledChunks(faceDir);
			overlappedNeighbour->adj(getXdir(overlapDir))->removeOutscrolledChunks(faceDir); 
			overlappedNeighbour->adj(getYdir(overlapDir))->removeOutscrolledChunks(faceDir);
			overlappedNeighbour->adj(getYdir(overlapDir))->adj(getXdir(overlapDir))->removeOutscrolledChunks(faceDir);
		}
	}
	if (chunksToSkin == 0)
		overlapDir = none;
	
}

/** Increase the tally of how many small superChunks now potentially overlap this one with their chunks. When the count is four, we know the scrolled-out
	chunks of this SC have now been replaced and can be removed. */
void CSuperChunk::incrementOverlap(Tdirection faceDir) {
	overlapCount--;
	if (overlapCount == 0) {
		removeFace(faceDir);
		overlapCount = 4;
	}
}


CSuperChunk::~CSuperChunk() {
	for (size_t c=0;c<chunkList.size();c++) {
		delete chunkList[c];
	}
}






