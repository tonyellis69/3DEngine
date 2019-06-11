#include "terrain2.h"

#include <iostream>
#include <numeric>      // std::iota


using namespace glm;



/** Create the central shell, establishing some variables. */
void CTerrain2::createLoD1shell(float _LoD1cubeSize, int _chunkCubes, int _SCchunks, int _shellSCs)  {
	LoD1cubeSize = _LoD1cubeSize;
	chunkCubes = _chunkCubes;
	SCchunks = _SCchunks;

	LoD1chunkSize = chunkCubes * LoD1cubeSize;
	shells.push_back({1, LoD1chunkSize, SCchunks, _shellSCs });
	shells.back().pTerrain = this;
	shells.back().shellNo = 0;
	shells.back().initSuperChunks();
	initialiseChunks(100);
}

/** Add an outer shell to the existing shells, its size adjusted by extent. */
void CTerrain2::addShell(int extent) {	
	int newShellLoD = shells.back().LoD * 2;
	float newShellChunkSize = shells.back().chunkSize * 2;
	int newShellSCs = shells.back().shellSCs + extent; // was (extent * 2);
	shells.push_back({ newShellLoD, newShellChunkSize, SCchunks, newShellSCs });
	shells.back().shellNo = shells.size() -1;
	shells.back().pTerrain = this;
	shells.back().initSuperChunks();
	//shells.back().innerBounds = getInnerBounds(shells.back().shellNo);
	shells.back().calculateInnerBounds();
}

/** Get the worldspace size of the given shell. */
float CTerrain2::getShellSize(unsigned int shellNo) {
	return shells[shellNo].worldSpaceSize;
}

/** Respond to the player moving by the given vector from their current position. */
void CTerrain2::playerWalk(glm::vec3 & move) {
	//if offset > 1 chunk, advance terrain.
	playerOffset += move;
	Tdirection inDirection = none;

	//are we now more than a chunk width from the terrain centre?
	if (playerOffset.x > LoD1chunkSize) {
		inDirection = east;
		playerOffset.x = mod(playerOffset.x, LoD1chunkSize);
	}
	else if (-playerOffset.x > LoD1chunkSize) {
		inDirection = west;
		playerOffset.x = mod(playerOffset.x, -LoD1chunkSize);
	}
	else if (-playerOffset.z > LoD1chunkSize) {
		inDirection = north;
		playerOffset.z = mod(playerOffset.z, -LoD1chunkSize);
	}
	else if (playerOffset.z > LoD1chunkSize) {
		inDirection = south;
		playerOffset.z = mod(playerOffset.z, LoD1chunkSize);
	}
	else if (playerOffset.y > LoD1chunkSize) {
		inDirection = up;
		playerOffset.y = mod(playerOffset.y, LoD1chunkSize);
	}
	else if (-playerOffset.y > LoD1chunkSize) {
		inDirection = down;
		playerOffset.y = mod(playerOffset.y, -LoD1chunkSize);
	}

	if (inDirection != none) {
		shells[0].playerAdvance(inDirection);
		removeChunkOverlaps(inDirection);
	}
}

/** Iterate through the shells, removing any chunks now overlapped by chunks of an inner shell. */
void CTerrain2::removeChunkOverlaps(Tdirection inDirection) {
	for (unsigned int shell = 1; shell < shells.size(); shell++) {
		shells[shell].calculateInnerBounds();
		shells[shell].removeEncroachedOnChunks2(inDirection);
	}
}

/** Fill all shells with chunks where they are intersected by the terrain. */
void CTerrain2::fillShells() {
	for (auto& shell : shells) {
		//how many chunk layers from the shell origin to fill:
		int chunkExtent = ((shell.shellSCs - 1) / 2) * SCchunks;
		//TO DO: find the best way to calculate this!

		shell.initChunkExtent();
		shell.fillEntire();

		//TO DO: this is adding chunks then removing them again! Find a more 
		//elegant way to do this!
		//maybe pass inner shell's getNWchunkSpaceExtent etc result to createAllChunks
		//via findAllSCchunks
		if (shell.shellNo > 0) {
			for (int dir = north; dir <= down; dir++)
				shell.removeEncroachedOnChunks2((Tdirection)dir);
		}
	}
}

/** Move the enclosing shells of this shell one SC length in the given direction. This is called to keep the outer terrain 
	aligned with a shell that has just scrolled. */
void CTerrain2::displaceOuterShells(const CShell & sender, Tdirection moveDirection) {
	vec3 move = dirToVec(moveDirection) * shells[sender.shellNo].SCsize;
	for (unsigned int outerShell = sender.shellNo +1; outerShell < shells.size(); outerShell++) {
		shells[outerShell].worldSpacePos += move;
	}
}


/** Move this shell and any enclosing shells one SC length in the given direction. This is called to keep their
	terrain in line with any inner shells after scrolling. It also prevents the shell from drifting away from shell 0.*/
void CTerrain2::returnShellAndOuterShells(const CShell & sender, Tdirection moveDirection) {
	vec3 move = dirToVec(moveDirection) * sender.SCsize;
	for (int shell = sender.shellNo; shell < shells.size(); shell++) {
		shells[shell].worldSpacePos += move;
	}
}


/** Centre the terrain at the give positionHint in sample space. */
void CTerrain2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

/** Set the number of world units per sample unit. */
void CTerrain2::setWorldScale(float scale) {
	worldToSampleScale = scale;
}

void CTerrain2::setCallbackApp(ITerrainCallback * pApp) {
	pCallbackApp = pApp;
}


/** Move the terrain's position in sample space. */
void CTerrain2::scrollSampleSpace(Tdirection scrollDir, float shift) {
	vec3 vec = dirToVec(flipDir(scrollDir)) * shift;
	sampleSpacePos += vec;
}

/** Prepare all chunks for use. */
void CTerrain2::initialiseChunks(int numChunks) {
	chunks.resize(numChunks);
	freeChunks.resize(numChunks);
	inUseChunks.clear();
	iota(begin(freeChunks), end(freeChunks), 0);
}

/** Return a chunk initialised with the given index. */
int CTerrain2::createChunk(glm::i32vec3& index) {
	int id = getFreeChunk();
	chunks[id].index = index;
	return id;
}

/** Return the id of a free chunk, creating more chunks if necessary. */
int CTerrain2::getFreeChunk() {
	int id;
	if (freeChunks.size() > 0) {
		id = freeChunks.back();
		freeChunks.pop_back();
		inUseChunks.push_back(id);
		return id;
	}

	int chunkInc = chunks.size() / 10;
	freeChunks.resize(chunkInc-1);
	id = chunks.size();
	iota(begin(freeChunks), end(freeChunks), id+1);
	chunks.resize(chunks.size() + chunkInc);
	inUseChunks.push_back(id);
	return id;
}


