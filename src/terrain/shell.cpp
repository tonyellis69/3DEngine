#include "shell.h"

using namespace glm;

CShell::CShell(int LoD, float chunkSize, int SCchunks, int shellSCs) :
	LoD(LoD), chunkSize(chunkSize), SCchunks(SCchunks), shellSCs(shellSCs) {

	SCsize = SCchunks * chunkSize;
	worldSpaceSize = shellSCs * SCsize;
	worldSpacePos = vec3(0);
	minimumChunkExtent = (shellSCs / 2) * SCchunks;
	playerChunkPos = i32vec3(0);
}


/** Respond to the player advancing in the given direction.*/
void CShell::advancePlayer(Tdirection direction) {
	//Either: (1) there is sufficient terrain in this direction and no need to supply more,
	//or (2) there's room for more terrain in the outer SC so add to it, or
	//(3) the SCs need to be scrolled  to provide more space, and then added to.
	//Also, update the player's position in the shell.

	if (chunkExtent(direction) >= minimumChunkExtent)
		return;
}

/** Return the number of chunks of terrain lying in the given direction from the player position. */
int CShell::chunkExtent(Tdirection direction) {
	return 0;
}
