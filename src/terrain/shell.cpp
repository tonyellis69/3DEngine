#include "shell.h"

#include "terrain2.h"
#include "..\colour.h"
#include "superChunk2.h"

#include "..\3DEngine\src\utils\log.h"

#include <iostream>



using namespace glm;

CShell::CShell(int LoD, float chunkSize, int SCchunks, int shellSCs) :
	LoD(LoD), chunkSize(chunkSize), SCchunks(SCchunks), shellSCs(shellSCs) {

	SCsize = SCchunks * chunkSize;
	worldSpaceSize = shellSCs * SCsize;
	worldSpacePos = vec3(0);
	minimumChunkExtent = (shellSCs / 2) * SCchunks;
	playerChunkPos = i32vec3(0);
	

	scArray.setSize(shellSCs, shellSCs, shellSCs);

	//initSuperChunks();
	shellColour = vec4(col::randHue(), 1);

	//SCs don't have chunks yet, so we can safely say:
	for (int face = 0; face < 6; face++)
		faceLayerFull[face] = false;
	
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

	//still here? More terrain is needed in the given direction
	//Is there room for more chunks in the final SC in the given direction?
	if (!faceLayerFull[direction]) {
		//(2) add chunks to the face layer SCs in this direction!
		addToFaceLayer(direction);
		liveLog << " adding to face layer.";
	}
	else {
		//(3) scroll the SCs, then add a layer 
		//NB Think of scroll direction as the direction of rotation of the conveyor belt of terrain
		Tdirection scrollDir = flipDir(direction);
		liveLog << " scrolling.";

		if (shellNo != 0)
			pTerrain->returnShellAndOuterShells(*this, direction);

		if (shellNo == 0) {
			pTerrain->scrollSampleSpace(scrollDir, scSampleStep);
		}
		scroll(scrollDir);

		//move enclosing shells back to ensure terrain still lines up
		pTerrain->displaceOuterShells(*this, scrollDir);


		setSCchunkBoundaries();
		addToFaceLayer(direction);
		removeScrolledOutChunks(scrollDir);
		if(shellNo < pTerrain->shells.size() - 1) 
			pTerrain->shells[shellNo + 1].addInnerFaceChunks(scrollDir);

		//if (shellNo < pTerrain->shells.size() - 1) {
		//	pTerrain->shells[shellNo + 1].handleInnerShellAdvance(direction);
		//}


		//move enclosing shells back to ensure terrain still lines up
		//pTerrain->displaceOuterShells(*this, scrollDir);
		//return;
	}

	//The containing shell has been encroached upon to the tune of one layer of its chunks, so tell sc to handle this.
	/////////////////////////////////////////////
	if (shellNo < pTerrain->shells.size() - 1) {
		 pTerrain->shells[shellNo + 1].handleInnerShellAdvance(direction);
	}
}

/** Respond to the inner shell advancing, by removing encroached-upon chunks and performing an advance of our own. */
void CShell::handleInnerShellAdvance(Tdirection direction) {
	playerAdvance(direction);
	removeEncroachedOnChunks(direction);
	//as well as removing its encroached-on chunks, this shell has to  introduce more
	//of its LoD terrain, which may result in a scroll
	////////////////////////playerAdvance(direction);
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
	findAllSCintersections();
	findAllSCchunks();
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

	//extend SC chunk boundaries accordingly
	setSCchunkBoundaries();

	//add the new chunks
	addChunksToFaceSCs( direction);

}

/** Rotate the SC ordering, moving them one step in the given direction, and moving the outgoing layer
	of SCs to become the new 'in' face. */
void CShell::scroll(Tdirection scrollDirection) {
	Tdirection inDirection = flipDir(scrollDirection);
	i32vec3 rotateVec = dirToVec(inDirection);
	scArray.rotate(rotateVec);

	//reset the chunk extent in this direction
	chunkExtent[inDirection] -= SCchunks; // minimumChunkExtent;
	//TO DO: can we use initChunkExtent here rather than this 
	//It's more elegant

	//setSCchunkBoundaries(); //TO DO any reason can't do this here rather than below?
	faceLayerFull[inDirection] = false;

	//The scrolled-out-and-back-in SCs have lost their chunks and occupy a new area of 
	//sample space, so resample them
	reinitialiseFaceSCs(inDirection);

	//The new back face SCs have to partially overlap the outer shell, so we need to 
	//remove the outer layers of chunks	
	setSCchunkBoundaries();  //TO DO: see above
	

	i32vec3 scrollVec = dirToVec(scrollDirection);
	//move player position in shell back by one SC's worth of chunks
	playerChunkPos += scrollVec * SCchunks;

	//move enclosing shells back to ensure terrain still lines up
	//pTerrain->displaceOuterShells(*this, scrollDirection);
}



/** Any SC setup required when a shell is created. */
void CShell::initSuperChunks() {
	scArray.element(0, 0, 0).setCallbackApp(pTerrain->pCallbackApp);
	scSampleStep = SCsize / pTerrain->worldToSampleScale;
	CShellIterator scIter = getIterator();
	while (!scIter.finished()) {
		scIter->isEmpty = true;
		//scIter->colour = vec4(col::randHue(), 1);
		scIter->colour = shellColour;
		scIter->origIndex = scIter.getIndex();
	
		scIter->SCchunks = SCchunks;
		scIter->nwChunkStart = i32vec3(0);
		scIter->seChunkEnd = i32vec3(SCchunks);

		scIter->setSampleSize(scSampleStep);

		vec3 sampleSpacePosition = calcSCsampleSpacePosition(scIter.getIndex());
		scIter->setSampleSpacePosition(sampleSpacePosition);



		scIter++;
	}

	initChunkExtent();
	setSCchunkBoundaries();
}

/** Return the point in sample space of the bottom NW corner of the superchunk at this index positionHint. */
vec3& CShell::calcSCsampleSpacePosition(i32vec3& scIndex) {
	//find worldspace displacement of SC from terrain centre 
	vec3 scPos = vec3(scIndex) * SCsize;
	scPos -= worldSpaceSize * 0.5f;
	scPos += worldSpacePos;
	//convert into a displacement in sample space
	vec3 sampleSpacePosition = scPos / pTerrain->worldToSampleScale;
	sampleSpacePosition = pTerrain->sampleSpacePos + sampleSpacePosition;
	return sampleSpacePosition;
}

/** Ask all superchunks to check if they are intersected by terrain. */
void CShell::findAllSCintersections() {
	COuterSCIterator sc = getOuterSCiterator();
	while (!sc.finished()) {
		sc->checkForIntersection();
		sc++;
	}
}

/** Fill all non-empty superchunks with chunks. */
void CShell::findAllSCchunks() {
	COuterSCIterator sc = getOuterSCiterator();
	while (!sc.finished()) {
		if (!sc->isEmpty)
			sc->createAllChunks();
		sc++;
	}
}



CShellIterator & CShell::getIterator() {
	return CShellIterator(this);
}

COuterSCIterator & CShell::getOuterSCiterator() {
	return COuterSCIterator(this);
}

CFaceIterator & CShell::getFaceIterator(Tdirection face) {
	return CFaceIterator(this, face);
}

/** TO DO: pretty sure this only has to be calculated once then cached. Investigate! */
TBoxVolume & CShell::getInnerBounds() {
	//return pTerrain->getInnerBounds(shellNo);
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
		vec3 sampleSpacePosition = calcSCsampleSpacePosition(faceIter.getIndex());
		faceIter->setSampleSpacePosition(sampleSpacePosition);
		faceIter->checkForIntersection();

		faceIter->nwChunkStart = i32vec3(0);
		faceIter->seChunkEnd = i32vec3(SCchunks);

		faceIter++;
	}
}

/** Set the chunk index boundaries - if any - for each SC, beyond which we don't try to place chunks. For
	most SCs there are none, but for the inner and outer SCs we want to prevent creating chunks where 
	the chunks of another shell take over. */ 
void CShell::setSCchunkBoundaries() {
	//i32vec3 nwChunkExtent = getNWchunkSpaceExtent();
	//i32vec3 seChunkExtent = getSEchunkSpaceExtent();
	TBoxVolume chunkVol = getChunkVolume();
	//TO DO: maybe replace chunkExtent altogether with these

	//(1)Find the origin of the chunk extents' -n - n volume in SC's 0 - n chunk coordinate space
	i32vec3 chunkExtentVolumeOrig = i32vec3(shellSCs * SCchunks) / 2;

	//use it to find the corners of the volume in SC chunk space
	i32vec3 nwClip = chunkExtentVolumeOrig + chunkVol.bl;
	i32vec3 seClip = chunkExtentVolumeOrig + chunkVol.tr - i32vec3(1); //because we go 0 - n-1 not 0 - n.

	//(2)For each SC, determine its position in chunk space...
	CShellIterator scIter = getIterator();
	while (!scIter.finished()) {
		i32vec3 scNWchunkSpacePos = scIter.getIndex() * SCchunks; 
		i32vec3 scSEchunkSpacePos = scNWchunkSpacePos + i32vec3(SCchunks - 1);
		//...clip that position against this shell's current chunk extents:

		//is the SC outside the chunk extents entirely? Then flatten its chunk start and end to nothing
		if (nwClip.x > scSEchunkSpacePos.x || nwClip.y > scSEchunkSpacePos.y || nwClip.z > scSEchunkSpacePos.z ) {
			scIter->nwChunkStart = i32vec3(SCchunks - 1);
			scIter->seChunkEnd = i32vec3(SCchunks - 1);
			scIter++; 
			continue;
		}
		if (seClip.x < scNWchunkSpacePos.x || seClip.y < scNWchunkSpacePos.y || seClip.z <scNWchunkSpacePos.z ) {
			scIter->nwChunkStart = i32vec3(0);
			scIter->seChunkEnd = i32vec3(0);
			scIter++;
			continue;
		}


		scIter->nwChunkStart = glm::max(scNWchunkSpacePos, nwClip) % i32vec3(SCchunks);
		//we want the min of the sc se pos and the se clip vol
		scIter->seChunkEnd = glm::min(scSEchunkSpacePos, seClip);
		scIter->seChunkEnd = scIter->seChunkEnd % i32vec3(SCchunks);

		scIter++;
	}
}

/** Iterate through the SCs of this face, and add more chunks where needed. */
void CShell::addChunksToFaceSCs(Tdirection face) {
	CFaceIterator faceIter = getFaceIterator(face);
	while (!faceIter.finished()) {
		if (faceIter->isEmpty == false) {
			faceIter->addFaceChunks(face);

		}
		faceIter++;
	}

}

/** Find and remove any chunks overlapped by the chunks of the inner shell. */
void CShell::removeEncroachedOnChunks(Tdirection face) {
	sysLog << "\n\nshell " << shellNo << " removing on face " << face;

	CShell* pInnerShell = &pTerrain->shells[shellNo - 1];
	TBoxVolume innerShellChunkVolume = pInnerShell->getChunkVolume();

	//convert inner shell chunk extent (expressed -n to +n) to this shell's 0 - n coordinate syste
	i32vec3 shellOffset = i32vec3((pInnerShell->worldSpacePos - worldSpacePos) / pInnerShell->chunkSize);
	innerShellChunkVolume = transformToLocalSpace(innerShellChunkVolume,shellOffset);

	TBoxVolume innerFace = calcInnerFaceSCVolume(face);
	TBoxVolume chunkClippingVolume;
	for (int x = innerFace.bl.x; x <= innerFace.tr.x; x++) {
		for (int y = innerFace.bl.y; y <= innerFace.tr.y; y++) {
			for (int z = innerFace.bl.z; z <= innerFace.tr.z; z++) {
				//find position of SC's chunks within inner shell's chunkspace
				i32vec3 SCchunkStart = i32vec3(x, y, z) * SCchunks; // -SCchunkspaceOrigin;
				SCchunkStart *= 2; //because these chunks are twice the size of the inner chunks

				//clip it to the inner shell's chunk volume
				chunkClippingVolume.bl = max(innerShellChunkVolume.bl, SCchunkStart);
				chunkClippingVolume.tr = min(innerShellChunkVolume.tr, (SCchunkStart + i32vec3(SCchunks-1) * 2));

				//convert to chunk indices
				chunkClippingVolume.bl /= 2;
				chunkClippingVolume.bl %= SCchunks;

				chunkClippingVolume.tr /= 2;
				chunkClippingVolume.tr %= SCchunks;

				scArray.element(x, y, z).clearOverlappedChunks(chunkClippingVolume);
				sysLog << "\nSC " << i32vec3(x, y, z) << " told to cull from " << chunkClippingVolume.bl
					<< " to " << chunkClippingVolume.tr << " inclusuve.";

			}
		}
	}
}

/** Transform this chunk volume, with its central axis, to the local chunk space of this shell, where
	the origin is in the bottom left front corner.*/
TBoxVolume& CShell::transformToLocalSpace(TBoxVolume& innerShellChunkVolume, i32vec3& offset) {
	i32vec3 SCchunkspaceOrigin = i32vec3(SCchunks * shellSCs);
	innerShellChunkVolume.bl = SCchunkspaceOrigin + offset + innerShellChunkVolume.bl;
	innerShellChunkVolume.tr = SCchunkspaceOrigin + offset + innerShellChunkVolume.tr - i32vec3(1); //because we go 0 - n-1 not 0 - n.;
	return innerShellChunkVolume;
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

/** Return the volume of this shell's chunk extent,measured in chunks counting from a central origin. */
TBoxVolume CShell::getChunkVolume() {
	TBoxVolume chunkVol;
	chunkVol.bl = i32vec3(-chunkExtent[west], -chunkExtent[down], -chunkExtent[north]);
	chunkVol.tr = i32vec3(chunkExtent[east], chunkExtent[up], chunkExtent[south]);
	return chunkVol;
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
void CShell::addInnerFaceChunks(Tdirection face) {
	sysLog << "\nAdd innerFaceChunks called on shell " << shellNo << " face " << face;
	//calculate how the chunk volume of the inner shell would have overlapped this shell before it 
	//scrolled. This is the space to fill.
	//find inner chunk volume
	if (shellNo == 1) {
		
		int b = 0;

	}

	CShell* pInnerShell = &pTerrain->shells[shellNo - 1];
	TBoxVolume innerShellChunkVolume = pInnerShell->getChunkVolume();

	//displace by previous overlap
	i32vec3 shellOffset = i32vec3((pInnerShell->worldSpacePos - worldSpacePos) / pInnerShell->chunkSize);
	innerShellChunkVolume = transformToLocalSpace(innerShellChunkVolume, shellOffset);

	//clip to SCs
	TBoxVolume innerFace = calcInnerFaceSCVolume(face);
	TBoxVolume chunkClippingVolume;
	for (int x = innerFace.bl.x; x <= innerFace.tr.x; x++) {
		for (int y = innerFace.bl.y; y <= innerFace.tr.y; y++) {
			for (int z = innerFace.bl.z; z <= innerFace.tr.z; z++) {
				//first, check if we can discard this SC altogether
				if (scArray.element(x, y, z).isEmpty) { //scrolled-in flanking SCs won't necessarily be empty
					scArray.element(x, y, z).checkForIntersection();
					if (scArray.element(x, y, z).isEmpty)
						continue;
				}

				//find position of SC's chunks within inner shell's chunkspace
				i32vec3 SCchunkStart = i32vec3(x, y, z) * SCchunks; // -SCchunkspaceOrigin;
				SCchunkStart *= 2; //because these chunks are twice the size of the inner chunks
				i32vec3 SCchunkEnd = SCchunkStart + i32vec3(SCchunks - 1) * 2;

				//clip it to the inner shell's chunk volume
				chunkClippingVolume.bl = max(innerShellChunkVolume.bl, SCchunkStart);
				chunkClippingVolume.tr = min(innerShellChunkVolume.tr, SCchunkEnd); 

					//convert to chunk indices
				chunkClippingVolume.bl /= 2;
				chunkClippingVolume.bl %= SCchunks;

				chunkClippingVolume.tr /= 2;
				chunkClippingVolume.tr %= SCchunks;

				//check if the SCs chunks are outside the clipping volume entirely
				//if so, collapse the axis to ensure chunks are still added
				if (any(greaterThan(SCchunkStart, innerShellChunkVolume.tr)) ||
					any(lessThan(SCchunkEnd, innerShellChunkVolume.bl))) {
					int axis = getAxis(face);
					chunkClippingVolume.bl[axis] = -1;
					chunkClippingVolume.tr[axis] = -1;
				}
				
				sysLog << "\nSC " << i32vec3(x, y, z) << " told to add outside " << chunkClippingVolume.bl
					<< " to " << chunkClippingVolume.tr << " inclusive.";
				scArray.element(x, y, z).restoreClippedChunks(chunkClippingVolume);

			}
		}
	}
}



CShellIterator::CShellIterator(CShell * pShell) {
	this->pShell = pShell;
	index = i32vec3(0);
	max = i32vec3(pShell->shellSCs);
	pSC = &pShell->scArray.element(0, 0, 0);
}

CSuperChunk2* CShellIterator::SC() {
	return pSC;
}

CShellIterator & CShellIterator::operator++() {
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
		pSC = &pShell->scArray.element(index.x, index.y, index.z);
	}
	return *this;

}

CShellIterator CShellIterator::operator++(int) {
	CShellIterator tmp(*this); 
	operator++(); 
	return tmp; 
}

CSuperChunk2 & CShellIterator::operator*() {
	return *pSC;
}

CSuperChunk2 * CShellIterator::operator->() {
	return pSC;
}

bool CShellIterator::finished() {
	return pSC == NULL;
}

glm::i32vec3 & CShellIterator::getIndex() {
	return index;
}



COuterSCIterator::COuterSCIterator(CShell * pShell) : CShellIterator(pShell) {
	innerBounds = pShell->getInnerBounds();
}


COuterSCIterator & COuterSCIterator::operator++() {
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
		pSC = &pShell->scArray.element(index.x, index.y, index.z);
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

	pSC = &pShell->scArray.element(element.x, element.y, element.z);
}

CFaceIterator & CFaceIterator::operator++() {
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

		pSC = &pShell->scArray.element(element.x, element.y, element.z);
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
