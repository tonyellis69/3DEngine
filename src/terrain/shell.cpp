#include "shell.h"

#include "terrain2.h"
#include "..\colour.h"
#include "superChunk2.h"

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


	//SCs don't have chunks yet, so we can safely say:
	for (int face = 0; face < 6; face++)
		faceLayerFull[face] = false;

}


/** Respond to the player advancing in the given direction.*/
void CShell::playerAdvance(Tdirection direction) {
	//Either: (1) there is sufficient terrain in this direction and no need to supply more,
	//or (2) there's room for more terrain in the outer SC so add to it, or
	//(3) the SCs need to be scrolled  to provide more space, and then added to.
	
	//Also, update the player's position in the shell.
	i32vec3 travelVector = dirToVec(direction);
	playerChunkPos += travelVector;

	std::cerr << "\nAdvance shell " << shellNo << ":";
	if (getPlayerChunkExtent(direction) >= minimumChunkExtent) {
		//1) sufficient terrain in direction, so nothing else to do
		std::cerr << " suffient terrain, returning.";
		return;
	}

	//still here? More terrain is needed in the given direction
	//Is there room for more chunks in the final SC in the given direction?
	if (!faceLayerFull[direction]) {
		//(2) add chunks to the face layer SCs in this direction!
		addToFaceLayer(direction);
		std::cerr << " adding to face layer.";
	}
	else {
		//(3) scroll the SCs, then add a layer 
		//NB Think of scroll direction as the direction of rotation of the conveyor belt of terrain
		Tdirection scrollDir = flipDir(direction);
		std::cerr << " scrolling.";
		//TO DO: will step always be LoD1? check
		pTerrain->scrollSampleSpace(scrollDir, scSampleStep);
		scroll(scrollDir);
		addToFaceLayer(direction);

		if (shellNo != 0)
			pTerrain->returnShellAndOuterShells(*this, direction);
	}

	//The containing shell has been encroached upon to the tune of one layer of its chunks, so tell it to handle this.
	/////////////////////////////////////////////
	if (shellNo < pTerrain->shells.size()-1)
		pTerrain->shells[shellNo + 1].advance(direction);

}

/** Handle a request to advance this shell in the given direction in its LoD terrain space. */
void CShell::advance(Tdirection direction) {
	//as well as removing its encroached-on chunks, this shell has to  introduce more
	//of its LoD terrain, which may result in a scroll
	playerAdvance(direction);
}

/** Return the number of chunks of terrain lying in the given direction from the player position. */
int CShell::getPlayerChunkExtent(Tdirection direction) {
	vec3 directionVector = dirToVec(direction);
	int totallExtent = chunkExtent[direction];
	int playerTravel = dot(vec3(playerChunkPos), directionVector);
	return totallExtent - playerTravel;
}

/** Fill the SCs with chunks where they are intersected by terrain, as far as chunkExtent from the origin .*/
void CShell::fillEntire(int chunkExtent) {
	//set shell chunk extent
	for (int direction = 0; direction < 6; direction++)
		this->chunkExtent[direction] = chunkExtent;

	//TO DO: fill SCs with chunks if they intersect terrain:
	findAllSCintersections();
}

/** Extend the terrain by two layers of chunks in the given direction. Two because we have to replace entire
	chunks of the enclosing shell, which will be twice the size.*/
void CShell::addToFaceLayer(Tdirection direction) {
	//TO DO: add two layers od chunks to all the SCs in the face layer of this shell.

	chunkExtent[direction] += 2;
	int maxChunkExtent = (shellSCs * SCchunks) / 2;
	if (chunkExtent[direction] >= maxChunkExtent)
		faceLayerFull[direction] = true;
}

/** Rotate the SC ordering, moving them one step in the given direction, and moving the outgoing layer
	of SCs to become the new 'in' face. */
void CShell::scroll(Tdirection scrollDirection) {
	Tdirection inDirection = flipDir(scrollDirection);
	i32vec3 rotateVec = dirToVec(inDirection);
	scArray.rotate(rotateVec);

	//The scrolled-out SCs have lost their chunks and maybe gained new ones, so reset them
	resampleFace(inDirection);

	//we've just lost a SC's worth of chunks in the 'in' direction, so:
	chunkExtent[inDirection] -= SCchunks;
	faceLayerFull[inDirection] = false;

	i32vec3 scrollVec = dirToVec(scrollDirection);
	//move player position in shell back by one SC's worth of chunks
	playerChunkPos += scrollVec * SCchunks;

	//move enclosing shells back to ensure terrain still lines up
	pTerrain->displaceOuterShells(*this, scrollDirection);
}



/** Any SC setup required when a shell is created. */
void CShell::initSuperChunks() {
	scArray.element(0, 0, 0).setCallbackApp(pTerrain->pCallbackApp);
	scSampleStep = SCsize / pTerrain->worldToSampleScale;
	CShellIterator scIter = getIterator();
	while (!scIter.finished()) {
		scIter->isEmpty = true;
		scIter->colour = vec4(col::randHue(), 1);
		scIter->origIndex = scIter.getIndex();
	
		scIter->setSampleSize(scSampleStep);

		vec3 sampleSpacePosition = calcSCsampleSpacePosition(scIter.getIndex());
		scIter->setSampleSpacePosition(sampleSpacePosition);
		scIter++;
	}
}

/** Return the point in sample space of the bottom NW corner of the superchunk at this index position. */
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
	//TO DO: only check outer SCs
	COuterSCIterator it = getOuterSCiterator();
	while (!it.finished()) {
		if (shellNo == 0)
			std::cerr << "\n checking SC at " << it.getIndex().x << " " << it.getIndex().y << " "
			<< it.getIndex().z << " , samplePos"
			
			<< it->sampleSpacePos.x << " " << it->sampleSpacePos.y << " "
			<< it->sampleSpacePos.z << ", coverage " << it->sampleSize;
		it->checkForIntersection();
		it++;
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

TShellInnerBounds & CShell::getInnerBounds() {
	return pTerrain->getInnerBounds(shellNo);
}

/**	Check if the SCs in this face intersect with terrain (and request the chunks if they do). */
void CShell::resampleFace(Tdirection face) {
	//return;
	CFaceIterator faceIter = getFaceIterator(face);

	//iterate through all the SCs in the face
	while (!faceIter.finished()) {
		//clear the old intersection data
		faceIter->isEmpty = true;
		vec3 sampleSpacePosition = calcSCsampleSpacePosition(faceIter.getIndex());
		faceIter->setSampleSpacePosition(sampleSpacePosition);
		if (shellNo == 0)
			std::cerr << "\n checking SC at " << faceIter.getIndex().x << " " << faceIter.getIndex().y << " "
			<< faceIter.getIndex().z << " , samplePos"
			
			<< faceIter->sampleSpacePos.x << " " << faceIter->sampleSpacePos.y << " "
			<< faceIter->sampleSpacePos.z << ", coverage " << faceIter->sampleSize;
		faceIter->checkForIntersection();

		faceIter++;
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
