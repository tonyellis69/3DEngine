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
	//We have to question the outer face of SCs in that direction
	//The shell needs a face status state, faceLayerFull. We ask this
	if (!faceLayerFull[direction]) {
		//add chunks to the face layer SCs in this direction
		//needs an addToFaceLayer method!

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
void CShell::fill(int chunkExtent) {
	//set shell chunk extent
	for (int direction = 0; direction < 6; direction++)
		this->chunkExtent[direction] = chunkExtent;

	//TO DO: fill SCs with chunks if they intersect terrain:

}
