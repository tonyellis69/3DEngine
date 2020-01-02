#include "shell.h"

#include "terrain2.h"
#include "..\colour.h"
#include "superChunk2.h"

#include "..\3DEngine\src\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	

#include <iostream>



using namespace glm;

CShell::CShell(int LoD, float chunkSize, int SCchunks, int shellSCs) :
	LoD(LoD), chunkSize(chunkSize), SCchunks(SCchunks), shellSCs(shellSCs) {

	SCsize = SCchunks * chunkSize;
	worldSpaceSize = shellSCs * SCsize;
	worldSpacePos = vec3(0);
	minimumChunkExtent = (float(shellSCs-1) / 2.0f) * SCchunks;
	playerChunkPos = i32vec3(0);
	

	scArray.setSize(shellSCs, shellSCs, shellSCs);

	shellColour = vec4(col::randHue(), 1);

	//SCs don't have chunks yet, so we can safely say:
	for (int face = 0; face < 6; face++)
		faceLayerFull[face] = false;
	scrolls = 0;
}


/** Respond to the player advancing in the given direction.*/
void CShell::playerAdvance(Tdirection direction) {
	//Either: (1) there is sufficient terrain in this direction and no need to supply more,
	//or (2) there's room for more terrain in the outer SC so add to sc, or
	//(3) the SCs need to be scrolled  to provide more space, and then added to.
	//Also, update the player's positionHint in the shell.
	i32vec3 travelVector = dirToVec(direction);
	playerChunkPos += travelVector;
	liveLog << "\nAdvance shell " << shellNo << ":";
	if (getPlayerChunkExtent(direction) >= minimumChunkExtent) {
		//1) sufficient terrain in direction, so nothing else to do
		liveLog << " suffient terrain, returning.";
		return;
	}
	
	//got here

	//still here? More terrain is needed in the given direction
	//Is there room for more chunks in the final SC in the given direction?
	if (!faceLayerFull[direction]) {
		
		//(2) add chunks to the face layer SCs in this direction!;
		addToFaceLayer(direction);
		liveLog << " adding to face layer.";
	
	}
	else {
		//(3) scroll the SCs, then add a layer 
		//NB Think of scroll direction (scrollDir) as the direction of rotation of the conveyor belt of terrain

		Tdirection scrollDir = flipDir(direction);
		liveLog << " scrolling.";

		scrolls++;

		if (shellNo != 0)
			pTerrain->recentreShellsAfterScroll(*this, direction);

		if (shellNo == 0) {
			pTerrain->scrollSampleSpace(scrollDir, scSampleStep);
		}

		scroll(scrollDir);

		/*if (shellNo == 0) {
			shellColour = vec4(col::randHue(), 1);
			pTerrain->shells[0].shellColour = vec4(0, 0, 1, 1.0f);
		}*/
		
		vec3 move = dirToVec(scrollDir) * SCsize;
		if (shellNo == 0) {
			pTerrain->chunkOrigin = glm::translate(pTerrain->chunkOrigin, move);
			liveLog << "\nchunkOrigin moved to " << vec3(pTerrain->chunkOrigin[3]);
		}

		//move enclosing shells back to ensure terrain still lines up
		pTerrain->alignOuterShellsWithScroll(*this, scrollDir);

		//now that we've scrolled, return to the default layout of one final double-layer of chunks
		addToFaceLayer(direction);

		removeScrolledOutChunks(scrollDir);
		//because we've removed scrolled out chunks, the rear inner face of the enclosing shell 
		//needs to add some to cover that area of terrain
		if (shellNo < pTerrain->shells.size() - 1) {
			//if (scrolls == 2)
			//	pTerrain->shells[shellNo + 1].shellColour = vec4(1, 1, 1, 1.0f);
			pTerrain->shells[shellNo + 1].addInnerFaceChunks2(scrollDir);
		}

		if (shellNo == 0)
			pTerrain->pCallbackApp->onTerrainScroll(move);
	}
	
	//didn't get here
	//The containing shell also needs to respond to the player advancing across the terrain:
	if (shellNo < pTerrain->shells.size() - 1) {
		 pTerrain->shells[shellNo + 1].playerAdvance(direction);
	}

	
}

/** Return the number of chunks of terrain lying in the given direction from the player positionHint. */
int CShell::getPlayerChunkExtent(Tdirection direction) {
	vec3 directionVector = dirToVec(direction);
	int totallExtent = chunkExtent[direction];
	int playerTravel = dot(vec3(playerChunkPos), directionVector);
	return totallExtent - playerTravel;
}

/** Fill the SCs with chunks where they are intersected by terrain, as far as chunkExtent from the origin .*/
void CShell::fillEntire() {
	calculateInnerBounds();
	//findAllSCchunks();
	fillAllUnclippedSCs();
}


/** Set chunks to initially extend as far as the minimum chunk extent in all directions. The extent
	will change as the terrain advances in a particular direction. */
void CShell::initChunkExtent() {
	for (int direction = 0; direction < 6; direction++)
		this->chunkExtent[direction] = minimumChunkExtent;
}


/** Extend the terrain by two layers of chunks in the given direction. Two because we have to replace entire
	chunks of the enclosing shell, which will be twice the size.*/
void CShell::addToFaceLayer(Tdirection direction) {
	chunkExtent[direction] += 2;
	int maxChunkExtent = (shellSCs * SCchunks) / 2;
	if (chunkExtent[direction] >= maxChunkExtent)
		faceLayerFull[direction] = true;

	//add the new chunks

	addChunksToFaceSCs2(direction);

}

/** Rotate the SC ordering, moving them one step in the given direction, and moving the outgoing layer
	of SCs to become the new 'in' face. */
void CShell::scroll(Tdirection scrollDirection) {
	Tdirection inDirection = flipDir(scrollDirection);
	i32vec3 rotateVec = dirToVec(inDirection);
	scArray.rotate(rotateVec);

	//reset the chunk extent in this direction
	chunkExtent[inDirection] = minimumChunkExtent-2; 
	chunkExtent[scrollDirection] = minimumChunkExtent;

	faceLayerFull[inDirection] = false;

	//The scrolled-out-and-back-in SCs have lost their chunks and occupy a new area of 
	//sample space, so resample them
	reinitialiseFaceSCs(inDirection);

	//SCs now entirely within the chunk volume of the inner shell need to be cleared: 
	if (shellNo > 0)
		reinitialiseInnerSCs();

	i32vec3 scrollVec = dirToVec(scrollDirection);
	//move player position in shell back by one SC's worth of chunks
	playerChunkPos += scrollVec * SCchunks;
}



/** Any SC setup required when a shell is created. */
void CShell::initSuperChunks() {
	scArray.element(0, 0, 0)->setCallbackApp(pTerrain->pCallbackApp);
	scSampleStep = SCsize / pTerrain->worldToSampleScale;
	CShellIterator scIter = getIterator();
	while (!scIter.finished()) {
		scIter->isEmpty = true;
	
		scIter->colour = shellColour;
		scIter->colour =  vec4(col::randHue(), 1);

		scIter->origIndex = scIter.getIndex();
	
		scIter->SCchunks = SCchunks;
		scIter->setSampleSize(scSampleStep);

		vec3 sampleSpacePosition = calcSCsampleSpacePosition(scIter.getIndex());
		scIter->setSampleSpacePosition(sampleSpacePosition);

		scIter->pTerrain = pTerrain;

		scIter->shellNo = shellNo;


		scIter++;
	}

	initChunkExtent();
}

/** Return the point in sample space of the bottom NW corner of the superchunk at this index positionHint. */
vec3 CShell::calcSCsampleSpacePosition(i32vec3& scIndex) {
	//find worldspace displacement of SC from terrain centre 
	vec3 scPos = vec3(scIndex) * SCsize;
	scPos -= worldSpaceSize * 0.5f;
	scPos += worldSpacePos;
	//convert into a displacement in sample space
	vec3 sampleSpacePosition = scPos / pTerrain->worldToSampleScale;
	sampleSpacePosition = pTerrain->sampleSpacePos + sampleSpacePosition;
	return sampleSpacePosition;
}

/** Fill all non-empty superchunks with chunks. */
void CShell::findAllSCchunks() {
	COuterSCIterator sc = getOuterSCiterator();
	CBoxVolume chunkVol = getChunkVolume2();
	while (!sc.finished()) {
		sc->checkForIntersection();
		if (!sc->isEmpty) {

			//clip sc to this shell's chunk volume
			//use the unit vol to tell the SC where to put chunks
			vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f);
			CBoxVolume SCvol;
			vec3 pos = vec3(sc.getIndex()) * SCsize - shellOrigin;
			SCvol.set(pos, pos + vec3(SCsize));
			if (chunkVol.isClippedBy(SCvol)) {
				sc->addChunks(SCvol);
			}


		}
		sc++;
	}
}

/** Fill SCs with chunks where they are intersected by terrain within this shell's chunk extent,
	but not where they are overlapped by the chunk extent of the inner shell, if any. */
void CShell::fillAllUnclippedSCs() {
	COuterSCIterator sc = getOuterSCiterator();
	CBoxVolume chunkVol = getChunkVolume2();
	CBoxVolume innerVol;
	CBoxVolume SCvol;
	CBoxVolume innerTest;
	while (!sc.finished()) {
		vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f);
		
	//	if (shellNo == 2 && sc.getIndex() == i32vec3(2, 3, 2))
	//		int b = 0;

		vec3 pos = vec3(sc.getIndex()) * SCsize - shellOrigin;
		SCvol.set(pos, pos + vec3(SCsize));
		innerTest = SCvol;
		if (chunkVol.isClippedBy(SCvol)) { //SC is within our chunk extent
			if (shellNo > 0) {
				
				innerVol = pTerrain->shells[shellNo - 1].getChunkVolume2();
				if (!innerVol.doesNotEntirelyEnvelop(innerTest)) {
					sc++;
					continue;
				}
				//still here? SC is in our zone of interest
				sc->checkForIntersection();
				if (!sc->isEmpty)
					sc->addChunksBetween(SCvol, innerTest);
			}
			else {
				sc->checkForIntersection();
				if (!sc->isEmpty)
					sc->addChunks(SCvol);
			}
		}
		sc++;
	}
}



CShellIterator  CShell::getIterator() {
	return CShellIterator(this);
}

COuterSCIterator  CShell::getOuterSCiterator() {
	return COuterSCIterator(this);
}

CFaceIterator  CShell::getFaceIterator(Tdirection face) {
	return CFaceIterator(this, face);
}

TBoxVolume  CShell::getInnerBounds() {
	return innerBounds;
}

/** Empty the superchunks in this face of any chunks, and reinitialise them ready to 
	contain new chunks. This function is called after a scroll, when a new layer of SCs
	has become the face layer. */
void CShell::reinitialiseFaceSCs(Tdirection face) {
	CFaceIterator faceIter = getFaceIterator(face);

	//iterate through all the SCs in the face
	while (!faceIter.finished()) {
		//clear the old intersection data
		faceIter->isEmpty = true;
		faceIter->clearChunks();
		//for (auto& chunk : faceIter->scChunks)
		//	faceIter->createChunk(faceIter.getIndex());
		vec3 sampleSpacePosition = calcSCsampleSpacePosition(faceIter.getIndex());
		faceIter->setSampleSpacePosition(sampleSpacePosition);
		faceIter->checkForIntersection();
		faceIter++;
	}
}


/** Iterate through the SCs of this face, and add more chunks where needed. */
void CShell::addChunksToFaceSCs2(Tdirection face) {
	CBoxVolume chunkVolume = getChunkVolume2();
	CBoxVolume SCvol;
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f) - worldSpacePos;
	CFaceIterator faceIter = getFaceIterator(face);
	while (!faceIter.finished()) {
		if (faceIter->isEmpty == false) {
			//clip SC to chunk volume
			//add any new chunks
			vec3 pos = vec3(faceIter.getIndex()) * SCsize - shellOrigin;
			SCvol.set(pos, pos + vec3(SCsize));
			
			//didn't get here
			if (chunkVolume.isClippedBy(SCvol)) {
				
				
				faceIter->addChunks(SCvol);

			}
			
		}
		faceIter++;
	}
}



/** For the given inner face, find and remove any chunks overlapped by the
	chunks of the inner shell. */
void CShell::removeEncroachedOnChunks2(Tdirection face) {
	//1. Find the inner face SC layer
	TBoxVolume innerFace = innerBounds;
	int axis = getAxis(face);

	if (face == north || face == west || face == down)
		innerFace.tr[axis] = innerFace.bl[axis] + 1;
	else
		innerFace.bl[axis] = innerFace.tr[axis] - 1;


	CBoxVolume innerChunkVol = pTerrain->shells[shellNo - 1].getChunkVolume2();
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f) - worldSpacePos;

	CBoxVolume SCvol;
	for (int x = innerFace.bl.x; x <= innerFace.tr.x; x++) {
		for (int y = innerFace.bl.y; y <= innerFace.tr.y; y++) {
			for (int z = innerFace.bl.z; z <= innerFace.tr.z; z++) {
				vec3 pos = vec3(x, y, z) * SCsize - shellOrigin;
				SCvol.set(pos, pos + vec3(SCsize));
				if (innerChunkVol.isClippedBy(SCvol)) {
					scArray.element(x, y, z)->clearChunks(SCvol);
				}
			}
		}
	}
}





/** Return the volume describing the intersection of the chunks of the given inner face
	and inner shell.*/
TBoxVolume CShell::calcInnerFaceSCVolume(Tdirection face) {
	TBoxVolume innerFace = innerBounds;

	int axis = getAxis(face);
	if (face == north || face == west || face == down) {
		innerFace.bl[axis] = innerBounds.bl[axis];
		innerFace.tr[axis] = innerBounds.bl[axis];
	}
	else {
		innerFace.bl[axis] = innerBounds.tr[axis];
		innerFace.tr[axis] = innerBounds.tr[axis];
	}
	return innerFace;
}

/** Find the innermost superchunks that we are interested in, before the chunks of the inner shell take over. */
void CShell::calculateInnerBounds() {
	innerBounds = { i32vec3(0),i32vec3(0) };
	if (shellNo == 0)
		return;

	CBoxVolume innerShellChunks = pTerrain->shells[shellNo - 1].getChunkVolume2();
	vec3 avoidSCBoundary(0.5f);
	innerBounds.bl = getSCat(innerShellChunks.bl - avoidSCBoundary);
	innerBounds.tr = getSCat(innerShellChunks.tr + avoidSCBoundary);
}

/** Return the index of the superchunk at this worldspace position. */
/*
 glm::i32vec3 CShell::getSCat(const glm::vec3& pos) {
	// if (pos == vec3{-0.500000000 , -722.461060 ,-0.500000000 })
	//	 int v = 0;
	 vec3 halfSize(worldSpaceSize * 0.5f);
	vec3 shellOrigin = halfSize - worldSpacePos;
	vec3 scPos = pos + shellOrigin; //pos is now 0,0,0 - n,n,n
	
	//check if pos falls entirely outside shell. If so, return -1-1-1
	if (any(greaterThan(scPos, halfSize)) ||
		any(lessThan(scPos,vec3(0))) ) {
		return i32vec3(-1);
	}

	i32vec3 result = i32vec3(scPos / vec3(SCsize));
	//if (result.y == -2)
	//	int b = 0;

	return result;
}
 */
 glm::i32vec3 CShell::getSCat(const glm::vec3& pos) {
	 vec3 scPos;
	 vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f) - worldSpacePos;
	 scPos = pos + shellOrigin; //pos is now 0,0,0 - n,n,n
	 return i32vec3(scPos / vec3(SCsize));
 }




/** Return the volume of this shell's chunk extent in worldspace, counting from a central origin. */
CBoxVolume CShell::getChunkVolume2() {
	CBoxVolume chunkVol;
	chunkVol.bl = vec3(-chunkExtent[west], -chunkExtent[down], -chunkExtent[north]) * chunkSize;
	chunkVol.tr = vec3(chunkExtent[east], chunkExtent[up], chunkExtent[south]) * chunkSize;
	chunkVol.bl += worldSpacePos;
	chunkVol.tr += worldSpacePos;
	return chunkVol;
}

/** Find any SCs now entirely within the inner chunk volume and clear them,
	so they're empty when they scroll back into use.*/
void CShell::reinitialiseInnerSCs() {
	calculateInnerBounds();
	TBoxVolume innerFace = innerBounds;

	for (int x = innerFace.bl.x + 1; x < innerFace.tr.x; x++) {
		for (int y = innerFace.bl.y + 1; y < innerFace.tr.y; y++) {
			for (int z = innerFace.bl.z + 1; z < innerFace.tr.z; z++) {
				scArray.element(x, y, z)->clearChunks();
			}
		}
	}
}

/** Convert the given index to its rotated equivalent. */
glm::i32vec3 CShell::getRotatedIndex(const glm::i32vec3& origIndex) {
	return scArray.getRotatedIndex(origIndex);
}

glm::i32vec3 CShell::getInvRotatedIndex(const glm::i32vec3& origIndex) {
	return scArray.getInvRotatedIndex(origIndex);
}



/** Notify SCs on the outface to remove any chunks that have now scrolled beyond the 
	chunk extent. */
void CShell::removeScrolledOutChunks(Tdirection outface) {
	int overlap = 2;
	CFaceIterator faceIter = getFaceIterator(outface);
	while (!faceIter.finished()) {
		if (faceIter->isEmpty == false) {
			faceIter->clearScrolledOutChunks(outface, overlap);
		}
		faceIter++;
	}

}

/** Add chunks to an inner face, where they had previously been displaced by the chunks of the
	inner shell, before it scrolled. */
void CShell::addInnerFaceChunks2(Tdirection face) {
	calculateInnerBounds();
	//1. Find the inner face SC layer
	TBoxVolume innerFace = innerBounds;


	//Check we don't go beyond our own chunk extent:
	int axis = getAxis(face);
	if (face == north || face == west || face == down) {
		innerFace.tr[axis] = innerFace.bl[axis];
		if (innerFace.bl[axis] > 1)
			innerFace.bl[axis] = innerFace.bl[axis] - 1;
	}
	else {
		innerFace.bl[axis] = innerFace.tr[axis];
		if (innerFace.tr[axis] < shellSCs - 2)
			innerFace.tr[axis] = innerFace.tr[axis] + 1;
	}

	CBoxVolume innerChunkVol = pTerrain->shells[shellNo - 1].getChunkVolume2();
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f);
	CBoxVolume SCvol;
	for (int x = innerFace.bl.x; x <= innerFace.tr.x; x++) {
		for (int y = innerFace.bl.y; y <= innerFace.tr.y; y++) {
			for (int z = innerFace.bl.z; z <= innerFace.tr.z; z++) {
				vec3 pos = vec3(x, y, z) * SCsize - shellOrigin +worldSpacePos;
				SCvol.set(pos, pos + vec3(SCsize));

				if (innerChunkVol.doesNotEntirelyEnvelop(SCvol)) {
					scArray.element(x, y, z)->addChunksOutside(SCvol);
				}
			}
		}
	}

}

CShellIterator::CShellIterator(CShell * pShell) {
	this->pShell = pShell;
	index = i32vec3(0);
	max = i32vec3(pShell->shellSCs);
	pSC = pShell->scArray.element(0, 0, 0);
}

CSuperChunk2* CShellIterator::SC() {
	return pSC;
}

CShellIterator  CShellIterator::operator++() {
	if (pSC != NULL) {
		index.z++;
		if (index.z == max.z) {
			index.z = 0;
			index.y++;
			if (index.y == max.y) {
				index.y = 0;
				index.x++;
				if (index.x == max.x) {
					pSC = NULL;
					return *this;
				}
			}

		}
		pSC = pShell->scArray.element(index.x, index.y, index.z);
	}
	return *this;

}

CShellIterator CShellIterator::operator++(int) {
	CShellIterator tmp(*this); 
	operator++(); 
	return tmp; 
}

CSuperChunk2  CShellIterator::operator*() {
	return *pSC;
}

CSuperChunk2 * CShellIterator::operator->() {
	return pSC;
}

bool CShellIterator::finished() {
	return pSC == NULL;
}

glm::i32vec3  CShellIterator::getIndex() {
	return index;
}



COuterSCIterator::COuterSCIterator(CShell * pShell) : CShellIterator(pShell) {
	innerBounds = pShell->getInnerBounds();
}


COuterSCIterator  COuterSCIterator::operator++() {
	if (pSC != NULL) {
		if (index.z == innerBounds.bl.z && ( index.y > innerBounds.bl.y && index.y < innerBounds.tr.y ) 
			&& (index.x > innerBounds.bl.x && index.x < innerBounds.tr.x) )
			index.z = innerBounds.tr.z;
		else
			index.z++;
		if (index.z == max.z) {
			index.z = 0;
			if (index.y == innerBounds.bl.y && (index.z > innerBounds.bl.z && index.z < innerBounds.tr.z)
				&& (index.x > innerBounds.bl.x && index.x < innerBounds.tr.x))
				index.y = innerBounds.tr.y;
			else
				index.y++;
			if (index.y == max.y) {
				index.y = 0;
				if (index.x == innerBounds.bl.x && (index.y > innerBounds.bl.y && index.y < innerBounds.tr.y)
					&& (index.z > innerBounds.bl.z && index.z < innerBounds.tr.z))
					index.x = innerBounds.tr.x;
				else
					index.x++;
				if (index.x == max.x) {
					pSC = NULL;
					return *this;
				}
			}

		}
		pSC = pShell->scArray.element(index.x, index.y, index.z);
	}
	return *this;

}

COuterSCIterator COuterSCIterator::operator++(int) {
	COuterSCIterator tmp(*this);
	operator++();
	return tmp;
}


CFaceIterator::CFaceIterator(CShell * pShell, Tdirection face) : CShellIterator(pShell) {
	pseudoX = getXaxis(face);
	pseudoY = getYaxis(face);
	pseudoZ = getAxis(face);

	

	if (face == south || face == east || face == up)
		pseudoZValue = pShell->shellSCs - 1;
	else
		pseudoZValue = 0;

	index = i32vec3(0); //because index is relative

	i32vec3 element;
	element[pseudoX] = 0;
	element[pseudoY] = 0;
	element[pseudoZ] = pseudoZValue;


	pSC = pShell->scArray.element(element.x, element.y, element.z);
}

CFaceIterator  CFaceIterator::operator++() {
	i32vec3 element;
	element[pseudoZ] = pseudoZValue;

	if (pSC != NULL) {
		index.y++;
		if (index.y == max.y) {
			index.y = 0;
			index.x++;
			if (index.x == max.x) {
				pSC = NULL;
				return *this;
			}
		}

		element[pseudoX] = index.x;
		element[pseudoY] = index.y;

		pSC = pShell->scArray.element(element.x, element.y, element.z);
	}
	return *this;
}

CFaceIterator CFaceIterator::operator++(int) {
	CFaceIterator tmp(*this);
	operator++();
	return tmp;
}

glm::i32vec3  CFaceIterator::getIndex() {
	i32vec3 element;
	element[pseudoX] = index.x;
	element[pseudoY] = index.y;
	element[pseudoZ] = pseudoZValue;// index.z;
	return element;
}
