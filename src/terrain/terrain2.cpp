#include "terrain2.h"

#include <iostream>

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
		std::cerr << "\nMovement!";
		shells[0].playerAdvance(inDirection);
	}

}

/** Fill all shells with chunks where they are intersected by the terrain. */
void CTerrain2::fillShells() {
	//TO DO: iterate through all shells
	//for now, just do the LoD1shell

	for (auto& shell : shells) {
		//how many chunk layers from the shell origin to fill:
		int chunkExtent = ((shell.shellSCs - 1) / 2) * SCchunks;
		//TO DO: find the best way to calculate this!

		shell.fillEntire(chunkExtent);
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

/** Go through all superchunks, asking them to check if they intersect terrain. */
void CTerrain2::findSCintersections() {

}

/** Centre the terrain at the give position in sample space. */
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


