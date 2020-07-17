#include "shell.h"

#include "terrain2.h"
#include "..\colour.h"
#include "superChunk2.h"

#include "..\3DEngine\src\utils\log.h"

#include <glm/gtc/matrix_transform.hpp>	

#include <iostream>

using namespace glm;

/** Construct a shell of the given proportions and initialise it. */
CShell::CShell(int LoD, float chunkSize, int numSCchunks, int shellSCs) :
	LoD(LoD), chunkSize(chunkSize), numSCchunks(numSCchunks), shellSCs(shellSCs) {

	SCsize = numSCchunks * chunkSize;
	worldSpaceSize = shellSCs * SCsize;
	minRequiredChunkExtent = (float(shellSCs-1) / 2.0f) * numSCchunks; //chunks must always extend this far
	maxChunkExtent = (shellSCs * numSCchunks) / 2;
	scArray.setSize(shellSCs, shellSCs, shellSCs);

	init();
}

void CShell::init() {
	worldSpacePos = vec3(0);
	for (int face = 0; face < 6; face++)
		faceLayerFull[face] = false;
}

void CShell::setTerrainCallbackObj(ITerrain* obj) {
	pTerrainObj = obj;

}

/** Update the terrain at the far end of this shell in the given direction, as
	determined by how close the viewpoint has got to it. Either (1) there is 
	still a permissable depth of terrain for this level of detail, so return false, or
	(2) there's not, but (a) there's room for more chunks in the outlying face of SCs,
	so add them, or (b), we scroll, shifting the viewpoint back and creating a new,
	empty face layer of SCs into which we add chunks.
		(a) and (b) both require that we add or remove chunks where this shell
		overlaps inner and outer shells.*/
bool CShell::advanceShellTerrain(Tdirection direction) {
	liveLog << "\nAdvance shell " << shellNo << ":";

	//1
	if (chunkExtentFromViewpoint(direction) >= minRequiredChunkExtent) {
		liveLog << " sufficent terrain, returning.";
		return false ;
	}
	
	//2a
	if (!faceLayerFull[direction]) {
		addChunksToFaceLayer(direction); 
		liveLog << " adding to face layer.";
	}
	else { //2b
		liveLog << " scrolling.";
		scroll(direction);
	}

	return true;
}

/** Fill this shell with chunks where it is intersected by terrain.*/
void CShell::createTerrain() {
	fillSCsToChunkExtent();
}

/** Perform the SC setup required when a shell is created. */
void CShell::initSuperChunks() {
	CSuperChunk2::pTerrainApp = pTerrainObj->getTerrainApp();
	scSampleStep = SCsize / pTerrain->worldToSampleScale;
	CShellIterator scIter = getIterator();
	while (!scIter.finished()) {
		scIter->setBasics(shellNo, scIter.getIndex(), numSCchunks);
		scIter->setSampleInfo(scSampleStep, calcSCsampleSpacePosition(scIter.getIndex()) );
		scIter++;
	}
}

/** Set chunks to initially extend as far as the minimum chunk extent in all directions. The extent
	will change as the terrain advances in a particular direction. */
void CShell::initChunkExtentToMinimum() {
	for (int direction = 0; direction < 6; direction++)
		this->chunkExtent[direction] = minRequiredChunkExtent;
}

/** For the given inner face, find and remove any chunks overlapped by the
	chunks of the inner shell following a scroll. */
void CShell::removeOverlappedInnerFaceChunks(Tdirection face) {
	TBoxVolume innerFaceVolume = findOverlappedInnerFaceSCs(face);
	removeOverlappedChunksInVolume(innerFaceVolume);
}

/** Add chunks to this inner face, to fill the gap left by the chunks of the
	inner shell that were removed when it scrolled. */
void CShell::addInnerFaceChunks2(Tdirection face) {
	TBoxVolume innerFaceVolume = findOverlappedInnerFaceSCsmultiLayer(face);	
	addChunksAtOrOutsideInnerShell(innerFaceVolume);
}


/** Return the volume of superchunks of this shell that is overlapped by the chunks of its inner shell. 
	This is also the volume defined by the inner faces of this shell. */
TBoxVolume CShell::findInnerFacesSCvolume() {
	TBoxVolume innerVol = { i32vec3(0),i32vec3(0) };
	if (shellNo == 0)
		return innerVol;

	CBoxVolume innerShellChunks = pTerrain->shells[shellNo - 1].calcWorldSpaceChunkExtent();
	vec3 avoidSCBoundary(0.5f);
	innerVol.bl = getSCat(innerShellChunks.bl - avoidSCBoundary);
	innerVol.tr = getSCat(innerShellChunks.tr + avoidSCBoundary);
	return innerVol;
}

glm::i32vec3 CShell::getSCat(const glm::vec3& pos) {
	vec3 scPos;
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f) - worldSpacePos;
	scPos = pos + shellOrigin; //pos is now 0,0,0 - n,n,n
	return i32vec3(scPos / vec3(SCsize));
}

glm::i32vec3 CShell::getInvRotatedIndex(const glm::i32vec3& origIndex) {
	return scArray.getInvRotatedIndex(origIndex);
}

COuterSCIterator  CShell::getOuterSCiterator() {
	return COuterSCIterator(this);
}



/////public-private divide 

/** Return the number of chunks of terrain lying in the given direction from the viewpoint. */
int CShell::chunkExtentFromViewpoint(Tdirection direction) {
	vec3 viewpointDisplacementInChunks = (pTerrain->viewpoint - worldSpacePos) / chunkSize;

	vec3 directionVector = dirToVec(direction);
	int displacementInDirection = dot(viewpointDisplacementInChunks, directionVector);
	return chunkExtent[direction] - displacementInDirection;
}

/** Extend the terrain by two layers of chunks in the given direction. Two because
	we have to replace entire chunks of the enclosing shell, which will be twice the size.*/
void CShell::addChunksToFaceLayer(Tdirection direction) {
	chunkExtent[direction] += 2;
	if (chunkExtent[direction] >= maxChunkExtent)
		faceLayerFull[direction] = true;

	addChunksToFaceSCs(direction);
}


/** Scroll the superChunks away from the given face to create space for new terrain there,
	then fill that space with terrain. At the opposite, outgoing face, remove the outermost 
	layers of chunks, returning the shell to its initial chunk extent in all directions, 
	ready for the cycle to begin again. Because this creates a gap between the outgoing
	face and the inner face of the outer shell, we notify the outer shell to rebuild itself. */
void CShell::scroll(Tdirection direction) {
	pTerrainObj->prepShellForScroll(shellNo, direction);

	Tdirection scrollDir = flipDir(direction);
	rotateSCs(scrollDir); 

	//move enclosing shells back to ensure terrain still lines up
	pTerrainObj->realignOuterShells(shellNo, scrollDir);

	//now that we've scrolled, return to the default layout of one final double-layer of chunks all around.
	addChunksToFaceLayer(direction);
	removeOutfaceChunks(scrollDir);

	//because we've removed scrolled out chunks, the rear inner face of the enclosing shell 
	//needs to add some to cover that area of terrain
	pTerrainObj->rebuildOuterShellInnerFace(shellNo, scrollDir); 
}


/** Rotate the SC ordering, moving the layers one step in the given direction, and moving the outgoing layer
	of SCs to become the new 'in' face. */
void CShell::rotateSCs(Tdirection scrollDirection) {
	Tdirection inDirection = flipDir(scrollDirection);
	i32vec3 rotateVec = dirToVec(inDirection);
	scArray.rotate(rotateVec);

	//reset the chunk extent in this direction
	chunkExtent[inDirection] = minRequiredChunkExtent-2; //-2 to create space for the chunks that will be added to this face
	chunkExtent[scrollDirection] = minRequiredChunkExtent;
	faceLayerFull[inDirection] = false;

	//The scrolled-out-and-back-in SCs have lost their chunks and occupy a new area of 
	//sample space, so resample them
	reinitialiseFaceSCs(inDirection);

	//SCs now entirely within the chunk volume of the inner shell need to be cleared of chunks: 
	if (shellNo > 0)
		clearInnerSCs();
}


/** Return the point in sample space of the bottom NW corner of the superchunk at this index position. */
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


/** Fill SCs with chunks where they are intersected by terrain within this shell's chunk extent,
	but not where they are overlapped by the chunk extent of an inner shell. */
void CShell::fillSCsToChunkExtent() {
	COuterSCIterator sc = getOuterSCiterator();
	CBoxVolume thisShellChunkVol = calcWorldSpaceChunkExtent();
	CBoxVolume innerShellChunkVol;
	CBoxVolume thisShellSCvol;

	bool isOverlapping; CBoxVolume SCoverlapVolume;
	for (auto sc = getOuterSCiterator(); !sc.finished(); sc++) {
		CBoxVolume SCnonOverlappedVol;
		vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f);
		vec3 pos = vec3(sc.getIndex()) * SCsize - shellOrigin;
		thisShellSCvol.set(pos, pos + vec3(SCsize));
		std::tie(isOverlapping, SCoverlapVolume) = thisShellChunkVol.findOverlap(thisShellSCvol);
		if (isOverlapping) {
			if (shellNo > 0) {
				innerShellChunkVol = pTerrain->shells[shellNo - 1].calcWorldSpaceChunkExtent();
				bool notEntirelyEnveloped;
				std::tie(notEntirelyEnveloped, SCnonOverlappedVol) = innerShellChunkVol.findNotOrPartiallyOverlapped(thisShellSCvol);
				if (!notEntirelyEnveloped) {
					continue;
				}
			}
			//still here? SC is at least partially in the region of shell that can have chunks
			sc->checkForTerrainIntersection();
			if (!sc->isEmpty)
				sc->addChunksBetween(SCoverlapVolume, SCnonOverlappedVol);

		}
	}
}

CShellIterator  CShell::getIterator() {
	return CShellIterator(this);
}

CFaceIterator  CShell::getFaceIterator(Tdirection face) {
	return CFaceIterator(this, face);
}


/** Empty the superchunks in this face of any chunks, and reinitialise them ready to 
	contain new chunks. This function is called after a scroll, when a new layer of SCs
	has become the face layer. */
void CShell::reinitialiseFaceSCs(Tdirection face) {
	CFaceIterator faceIter = getFaceIterator(face);
	while (!faceIter.finished()) {
	
		faceIter->isEmpty = true;
		faceIter->clearChunks();

		vec3 sampleSpacePosition = calcSCsampleSpacePosition(faceIter.getIndex());
		faceIter->setSampleSpacePosition(sampleSpacePosition);
		faceIter->checkForTerrainIntersection();
		faceIter++;
	}
}


/** Iterate through the SCs of this face, and add more chunks where they are 
	overlapped by the chunk extent. */
void CShell::addChunksToFaceSCs(Tdirection face) {
	CBoxVolume chunkVolume = calcWorldSpaceChunkExtent();
	bool overlapping;  CBoxVolume overlapVolume;
	CBoxVolume currentSCvolume;
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f) - worldSpacePos;

	for (auto faceSC = getFaceIterator(face); !faceSC.finished(); faceSC++) {
		if (!faceSC->isEmpty) {
			vec3 SCpos = vec3(faceSC.getIndex()) * SCsize - shellOrigin;
			currentSCvolume.set(SCpos, SCpos + vec3(SCsize));		
			std::tie(overlapping, overlapVolume) = chunkVolume.findOverlap(currentSCvolume);
			if (overlapping) {
				faceSC->addChunks(overlapVolume);
			}
		}
	}
}


/** Return the volume of this shell's chunk extent in worldspace, counting from a central origin. */
CBoxVolume CShell::calcWorldSpaceChunkExtent() {
	CBoxVolume chunkVol;
	chunkVol.bl = vec3(-chunkExtent[west], -chunkExtent[down], -chunkExtent[north]) * chunkSize;
	chunkVol.tr = vec3(chunkExtent[east], chunkExtent[up], chunkExtent[south]) * chunkSize;
	chunkVol.bl += worldSpacePos;
	chunkVol.tr += worldSpacePos;
	return chunkVol;
}

/** Find any SCs entirely within the chunk extent of the inner shell, and clear them,
	so they're empty when they scroll back into use.*/
void CShell::clearInnerSCs() {
	TBoxVolume innerFace = findInnerFacesSCvolume();

	for (int x = innerFace.bl.x + 1; x < innerFace.tr.x; x++) {
		for (int y = innerFace.bl.y + 1; y < innerFace.tr.y; y++) {
			for (int z = innerFace.bl.z + 1; z < innerFace.tr.z; z++) {
				scArray.element(x, y, z)->clearChunks();
			}
		}
	}
}

/** Convert this original SC index positon to its rotated equivalent. */
glm::i32vec3 CShell::getRotatedIndex(const glm::i32vec3& origIndex) {
	return scArray.getRotatedIndex(origIndex);
}

/** Return the volume encompassing the SCs of the given inner face. */
TBoxVolume CShell::findOverlappedInnerFaceSCs(Tdirection innerFace) {
	TBoxVolume innerSCvol = findInnerFacesSCvolume();
	int axis = getAxis(innerFace);
	if (innerFace == north || innerFace == west || innerFace == down)
		innerSCvol.tr[axis] = innerSCvol.bl[axis] + 1;
	else
		innerSCvol.bl[axis] = innerSCvol.tr[axis] - 1;
	return innerSCvol;
}

/** For each SC of this shell, remove its chunks where they are overlapped by the volume. */
void CShell::removeOverlappedChunksInVolume(TBoxVolume innerVol) {
	CBoxVolume innerShellChunkVol = pTerrain->shells[shellNo - 1].calcWorldSpaceChunkExtent();
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f) - worldSpacePos;

	CBoxVolume SCvol;
	bool isOverlapping; CBoxVolume overlapVolume;
	for (int x = innerVol.bl.x; x <= innerVol.tr.x; x++) {
		for (int y = innerVol.bl.y; y <= innerVol.tr.y; y++) {
			for (int z = innerVol.bl.z; z <= innerVol.tr.z; z++) {
				vec3 pos = vec3(x, y, z) * SCsize - shellOrigin;
				SCvol.set(pos, pos + vec3(SCsize));
				std::tie(isOverlapping, overlapVolume) = innerShellChunkVol.findOverlap(SCvol);
				if (isOverlapping) {
					scArray.element(x, y, z)->clearChunks(overlapVolume);
				}
			}
		}
	}

}

/** Return the volume encompassing the SCs of tje given inner face, incorporating two
	layer of SCs where that face is sufficiently deep withing this shell.
	Two layers ensures we provide a large enough search area for adding inner face
	chunks after the inner shell scrolled. */
TBoxVolume CShell::findOverlappedInnerFaceSCsmultiLayer(Tdirection face) {
	TBoxVolume innerVol = findInnerFacesSCvolume();

	int axis = getAxis(face);

	if (face == north || face == west || face == down) {
		innerVol.tr[axis] = innerVol.bl[axis];
		innerVol.bl[axis] = max(0, innerVol.bl[axis] - 1);
	}
	else {
		innerVol.bl[axis] = innerVol.tr[axis];
		innerVol.tr[axis] = min(shellSCs - 1, innerVol.tr[axis] + 1);
	}

	return innerVol;
}

/** Add chunks to every SC in the given volume that is *outside* or only
	partially overlapped by the chunk extent of the inner shell. 
	This is used to add chunks to an inner face to fill the gap left when the chunks of an
	scrolling inner were removed.*/
void CShell::addChunksAtOrOutsideInnerShell(TBoxVolume volume) {
	CBoxVolume innerChunkVol = pTerrain->shells[shellNo - 1].calcWorldSpaceChunkExtent();
	vec3 shellOrigin = (vec3(worldSpaceSize) * 0.5f);
	CBoxVolume thisShellSCvol;
	for (int x = volume.bl.x; x <= volume.tr.x; x++) {
		for (int y = volume.bl.y; y <= volume.tr.y; y++) {
			for (int z = volume.bl.z; z <= volume.tr.z; z++) {
				vec3 pos = vec3(x, y, z) * SCsize - shellOrigin + worldSpacePos;
				thisShellSCvol.set(pos, pos + vec3(SCsize));
				bool notEntirelyEnveloped; CBoxVolume SCnonOverlappedVol;
				std::tie(notEntirelyEnveloped, SCnonOverlappedVol) = innerChunkVol.findNotOrPartiallyOverlapped(thisShellSCvol);
				if (notEntirelyEnveloped) {
					scArray.element(x, y, z)->addChunksOutside(SCnonOverlappedVol);;
				}
			}
		}
	}
}


/** We've scrolled, so notify SCs on the new outface to remove all but their 
	innnermost two layers of chunks, as the rest of this terrain will now 
	be provided by the enclosing shell. */
void CShell::removeOutfaceChunks(Tdirection outface) {
	int layersToKeep = 2;
	CFaceIterator faceIter = getFaceIterator(outface);
	while (!faceIter.finished()) {
		if (faceIter->isEmpty == false) {
			faceIter->clearOuterLayerChunks(outface, layersToKeep);
		}
		faceIter++;
	}
	faceLayerFull[outface] = false;
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
	//innerBounds = pShell->getInnerBounds();
	innerBounds = pShell->findInnerFacesSCvolume();
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
