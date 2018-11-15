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
}

/** Add an outer shell to the existing shells, its size adjusted by extent. */
void CTerrain2::addShell(int extent) {	
	int newShellLoD = shells.back().LoD * 2;
	float newShellChunkSize = shells.back().chunkSize * 2;
	int newShellSCs = shells.back().shellSCs + extent; // was (extent * 2);
	shells.push_back({ newShellLoD, newShellChunkSize, SCchunks, newShellSCs });
	shells.back().pTerrain = this;
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
	else if (playerOffset.z > LoD1chunkSize) {
		inDirection = north;
		playerOffset.z = mod(playerOffset.z, LoD1chunkSize);
	}
	else if (-playerOffset.z > LoD1chunkSize) {
		inDirection = south;
		playerOffset.z = mod(playerOffset.z, -LoD1chunkSize);
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
	}

}

/** Fill all shells with chunks where they are intersected by the terrain. */
void CTerrain2::fillShells() {
	//TO DO: iterate through all shells
	//for now, just do the LoD1shell

	//how many chunk layers from the shell origin to fill:
	int chunkExtent = ((shells[0].shellSCs - 1) / 2) * SCchunks;
	//TO DO: find the best way to calculate this!

	shells[0].fillEntire(chunkExtent);
}

/** Move all shells but the sender in the given direction. This is called to keep the terrain 
	aligned with a shell that has just scrolled. */
void CTerrain2::worldMove(const CShell & sender, Tdirection moveDirection) {
	//TO DO
	//Move all shells but the sender one SC width in the move direction
	//NB this is analogous to the shell playerAdvance method, in that it might result in adding more 
	//terrain or scrolling - it's just that instead of responding to the player moving within a shell
	//wer're responding to a *shell* moving within a larger shell. Try to keep this in mind and 
	//exploit it.
}
