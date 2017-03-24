#include "superChunk.h"
#include <glm/gtc/matrix_transform.hpp>	
#include <glm/mat4x4.hpp> // glm::mat4
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
	removeOutgoingChunks = freeToRemove;
	overlapDir = none;
	overlapCount = 0;
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

void CSuperChunk::setSamplePos(vec3 pos) {
	nwSamplePos = pos;
}


/** Create chunks for this superchunk where they are intersected by the isosurface. Each chunk is put on the list for
	skinning*/
void CSuperChunk::createChunks() {
	//first, rule out this superChunk altogether if possible
	if (terrain->superChunkIsEmpty(nwSamplePos,LoD))
		return;

	//genColour = vec4(col::randHue(),1);

	vec3 samplePos = nwSamplePos; 
	Chunk* chunk;
	for (int x=0;x<sizeInChunks.x;x++) {
		for (int y=0;y<sizeInChunks.y;y++) {
			for (int z=0;z<sizeInChunks.z;z++) {
				samplePos = nwSamplePos + (vec3(x,y,z) * (float) cubesPerChunkEdge * LoDscale);
				if (terrain->chunkExists(samplePos,LoD)) { 
					chunk = createChunk(i32vec3(x,y,z));
					chunkList.push_back(chunk);
					CSkinningOrder order;
					order.chunk = chunk;
					order.parentSC = NULL;
					terrain->toSkin.push_back(order);
				}
			}
		} 
	}
}


extern int uniTag = 0;
/** Return a pointer to a chunk that has been positioned at the given chunk position and
	initialised for skinning.*/
Chunk* CSuperChunk::createChunk(i32vec3& gridPosition) {
	vec3 samplePos = nwSamplePos + vec3(gridPosition) * (cubesPerChunkEdge * LoDscale);
	Chunk* newChunk = terrain->getFreeChunk();
	newChunk->setPos(nwWorldPos + (vec3(gridPosition)*chunkSize));
	newChunk->cubeSize = cubeSize;
	newChunk->setSamplePos(samplePos);
	newChunk->colour = genColour;
	newChunk->LoD = LoD;
	newChunk->scIndex = gridPosition;
	newChunk->tag = uniTag++;
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
	else if (adj(outFace)->LoD > LoD)  { //we're outscrolling chunks into a superChunk of lower detail
		;
	} else if (adj(outFace)->LoD < LoD) { //we're outscrolling chunks into a superChunk of higher detail
		flagFaceForRemoval(outFace);  //
	}

	if (adj(inFace) == NULL)	{	//if inface direction doesn't lead to another superchunk
			addFace(inFace);
	}
	else if (adj(inFace)->LoD > LoD) { //we're inscrolling chunks from a superChunk of lower detail
		//NB this stage now handled by ;
		
	}
	else if  (adj(inFace)->LoD < LoD) { //we're inscrolling chunks from a superChunk of higher detail
		addFace(inFace);
	}
	else 
		acquireChunks(inFace);
		
	//assign a random colour to this new layer
	genColour = vec4(col::randHue(),1);
}



/** Move all existing chunks one chunk along in the given direction in worldspace.*/
void CSuperChunk::shiftChunksBy1(vec3 tVec) {  
	vec3 shiftDist = tVec;
	for (int c=0;c<chunkList.size();c++)
		chunkList[c]->translate(-shiftDist);
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
	i32vec3 facePos(0); Chunk* newChunk;

	int xAxis = getXaxis(faceDir);
	int yAxis = getYaxis(faceDir);

	Tdirection xStart,xEnd,yStart,yEnd;
	xStart = getXstart(faceDir); xEnd = flipDir(xStart);
	yStart = getYstart(faceDir); yEnd = flipDir(yStart);

	facePos.z = firstEmptyLayer(faceDir);
	for (facePos.x=outerLayer(xStart);facePos.x<=outerLayer(xEnd);facePos.x++) {
		for (facePos.y=outerLayer(yStart);facePos.y<=outerLayer(yEnd);facePos.y++) {
			i32vec3 realPos = rotateByDir(facePos,faceDir);
			vec3 samplePos = nwSamplePos + (vec3(realPos) * (float)cubesPerChunkEdge * LoDscale);
			if (terrain->chunkExists(samplePos,LoD)) {	
				newChunk = createChunk(realPos); 
				CSkinningOrder order;
				order.chunk = newChunk;
				order.parentSC = this;
				chunkList.push_back(newChunk);
				terrain->toSkin.push_back(order);
				chunksToSkin++;
			}

		}
	}
	extendBoundary(faceDir);//because we've added a layer of chunks to this face

	if (!adj(faceDir)) //catch SCs at very edge of terrain. 
		return;

	if (chunksToSkin) {
		adj(faceDir)->removeOutgoingChunks = waitToRemove;
		adj(faceDir)->adj(getXdir(faceDir))->removeOutgoingChunks = waitToRemove;
		adj(faceDir)->adj(getYdir(faceDir))->removeOutgoingChunks = waitToRemove;
		adj(faceDir)->adj(getYdir(faceDir))->adj(getXdir(faceDir))->removeOutgoingChunks = waitToRemove;
		overlapDir = faceDir;
	}
	else {
		adj(faceDir)->removeOutgoingChunks = freeToRemove;
		adj(faceDir)->adj(getXdir(faceDir))->removeOutgoingChunks = freeToRemove;
		adj(faceDir)->adj(getYdir(faceDir))->removeOutgoingChunks = freeToRemove;
		adj(faceDir)->adj(getYdir(faceDir))->adj(getXdir(faceDir))->removeOutgoingChunks = freeToRemove;
		overlapDir = none;
	}

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
	if (this == dbgSC)
		int b = 0;
}

/** Shrink the recogised position for outermost chunks in the face direction. */
void CSuperChunk::shrinkBoundary(Tdirection face) {
	
	if ((face == north)||(face == west) || (face == down))
		faceBoundary[face] += 1;
	else
		faceBoundary[face] -= 1;
	if (this == dbgSC)
		int b = 0;
}

/** Return the position of the first layer beyond the outer, face layer of chunks in the given direction. */
int CSuperChunk::firstEmptyLayer(Tdirection face) {
	return faceBoundary[face] + getSign(face);
}

/** Return the position of the outermost layer of chunks in the face direction. */
int CSuperChunk:: outerLayer(Tdirection face) {
	return faceBoundary[face];
}

void CSuperChunk::flagFaceForRemoval(Tdirection face) {

//	removeFace(face);
//	return;
	//iterate through face chunks, adding them to removal list
/*	int zAxis = getAxis(face);
	int zPos = outerLayer(face);
	for (size_t chunk=0;chunk < chunkList.size();chunk++) {
		if (chunkList[chunk]->scIndex[zAxis] == zPos)  {
			CReplaceChunk replaceChunk;
			replaceChunk.oldChunk = chunkList[chunk];
			replaceChunk.nwCorner = chunkList[chunk]->getPos();
			replaceChunk.opCorner = replaceChunk.nwCorner + chunkSize;
			replaceChunk.parentSC = this;
			terrain->replaceChunks.push_back(replaceChunk);

		}
	} */

	if (chunkList.size() > 0) {
		removeOutgoingChunks = waitToRemove;
		//overlapCount = 4;

	}
	else {
		removeOutgoingChunks = freeToRemove;
		shrinkBoundary(face);
	}


//	shrinkBoundary(face);
}

/** If a surface intersects the volume at the given sample position, create a chunk for it. */
bool CSuperChunk::createChunkAsRequired(i32vec3& pos, vec3& samplePos, CSuperChunk* replaceSC) {
	Chunk* newChunk = NULL;
	if (terrain->chunkExists(samplePos,LoD)) {	
		newChunk = createChunk(pos); 
		chunkList.push_back(newChunk);
		CSkinningOrder order;
		order.chunk = newChunk;
		order.parentSC = replaceSC;
		terrain->toSkin.push_back(order);
		return true;
	}
	vec3 worldPos = nwWorldPos + (vec3(pos)*chunkSize);
	//terrain->replacementCheck(worldPos,newChunk, replaceSC);
	
	terrain->handleSmallerChunkOverlap(worldPos, this->LoD - 1);
	return false;
}

void CSuperChunk::removeChunk(Chunk* chunk) {
	for (size_t chk=0;chk<chunkList.size();chk++)
		if (chunkList[chk] == chunk) {
			terrain->freeChunk(chunk);
			chunkList.erase(chunkList.begin() + chk);
			break;
		}
}

/** Remove any chunks now outscrolled and replaced.*/
void CSuperChunk::removeOutscrolledChunks(Tdirection faceDir) {
	if (this == dbgSC)
		 int h = 0;
	removeFace(faceDir);
	removeFace(faceDir);

}

CSuperChunk::~CSuperChunk() {
	for (size_t c=0;c<chunkList.size();c++) {
		delete chunkList[c];
	}
}






