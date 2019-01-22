#include "shell.h"

#include "terrain2.h"
#include "..\colour.h"

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
		Tdirection scrollDir = flipDir(direction);
		std::cerr << " scrolling.";
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
	findSCintersections();
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
	vec3 sampleSpacePosition;
	for (int x = 0; x < shellSCs; x++) {
		for (int y = 0; y < shellSCs; y++) {
			for (int z = 0; z < shellSCs; z++) {
				scArray.element(x, y, z).isEmpty = true;
				scArray.element(x, y, z).colour = vec4(col::randHue(),1);
				scArray.element(x, y, z).origIndex = vec3(x,y,z);
				//scArray.element(x, y, z).setSampleSize(SCsize / pTerrain->worldToSampleScale);

				//temp!
				float scSampleStep = SCsize / pTerrain->worldToSampleScale;
				float lodScale = scSampleStep / 16;
			//	std::cerr << "\n#LoD " << this->LoD << " SC sampleStep " << scSampleStep << " LoDscale " << lodScale;

				scArray.element(x, y, z).setSampleSize(lodScale);

				//find worldspace displacement of SC from terrain centre 
				vec3 scPos = vec3(x, y, z) * SCsize;
				//////////////////////////////scPos += SCsize * 0.5f; TO DO temporary only
				scPos -= worldSpaceSize * 0.5f;
				scPos += worldSpacePos;
				//convert into a displacement in sample space
				sampleSpacePosition = scPos / pTerrain->worldToSampleScale;
				//should end up with around 5 .3 5 fo (0,0,0)
				sampleSpacePosition = pTerrain->sampleSpacePos + sampleSpacePosition;
				scArray.element(x, y, z).setSampleSpacePosition(sampleSpacePosition);
			}
		}
	}
}

/** Ask all superchunks to check if they are intersected by terrain. */
void CShell::findSCintersections() {
	for (int x = 0; x < shellSCs; x++) {
		for (int y = 0; y < shellSCs; y++) {
			for (int z = 0; z < shellSCs; z++) {
				//TO DO: exclude SCs entirely within child shell
					scArray.element(x, y, z).checkForIntersection();
			}
		}
	}
}
