#include "shell.h"

using namespace glm;

CShell::CShell(int LoD, float chunkSize, int SCchunks, int shellSCs) :
	LoD(LoD), chunkSize(chunkSize), SCchunks(SCchunks), shellSCs(shellSCs) {

	SCsize = SCchunks * chunkSize;
	worldSpaceSize = shellSCs * SCsize;
	worldSpacePos = vec3(0);
	minimumChunkExtent = (shellSCs / 2) * SCchunks;
	playerChunkPos = i32vec3(0);

	//SCs don't chunks yet, so we can safely say:
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

	if (getPlayerChunkExtent(direction) >= minimumChunkExtent) {
		//1) sufficient terrain in direction, so nothing else to do
		return;
	}

	//still here? More terrain is needed in the given direction
	//Is there room for more chunks in the final SC in the given direction?
	if (!faceLayerFull[direction]) {
		//(2) add chunks to the face layer SCs in this direction!
		addToFaceLayer(direction);
	}
	else {
		//(3) scroll the SCs, then add a layer 
		Tdirection scrollDir = flipDir(direction);
		scroll(scrollDir);
		addToFaceLayer(direction);
		requestWorldAdvance(scrollDir);
	}

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

}

/** Extend the terrain by two layers of chunks in the given direction. Two because we have to replace entire
	chunks of the enclosing shell, which will be twice the size.*/
void CShell::addToFaceLayer(Tdirection direction) {
	//TO DO: add two layers od chunks to all the SCs in the face layer of this shell.

	chunkExtent[direction] += 2;
}

/** Rotate the SC ordering, moving them one step in the given direction, and moving the outgoing layer
	of SCs to become the new 'in' face. */
void CShell::scroll(Tdirection scrollDirection) {
	//TO DO: scroll SCs
	//we've just lost a SC's worth of chunks in the 'in' direction, so:
	chunkExtent[flipDir(scrollDirection)] -= SCchunks;

	//move player position in shell back by one SC's worth of chunks
	i32vec3 scrollVec = dirToVec(scrollDirection);
	playerChunkPos += scrollVec * SCchunks;
}

/** Ask the other shells of the world to move in the given direction. This is done to ensure that the 
	terrain of this shell still lines up with the rest after a scroll. */
void CShell::requestWorldAdvance(Tdirection scrollDirection){
	//TO DO@
	//message the parent terrain, asking it to move/advance the other shells by 1 LoD1 SC width.
}
