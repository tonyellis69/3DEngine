#include "terrain2.h"

#include <iostream>
#include <numeric>      // std::iota

#include "..\\3DEngine\src\utils\log.h"

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
	initialiseChunks(2000); //2000  //2164
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

		shell.initChunkExtent();
		shell.fillEntire();
	}
}

/** Move the enclosing shells of this shell one SC length in the given direction. This is called to keep the outer terrain 
	aligned with a shell that has just scrolled. */
void CTerrain2::alignOuterShellsWithScroll(const CShell & sender, Tdirection moveDirection) {
	vec3 move = dirToVec(moveDirection) * shells[sender.shellNo].SCsize;
	for (unsigned int outerShell = sender.shellNo +1; outerShell < shells.size(); outerShell++) {
		shells[outerShell].worldSpacePos += move;
	}
}


/** Move this shell and any enclosing shells one SC length in the given direction. This is called to keep their
	terrain in line with any inner shells after scrolling. It also prevents the shell from drifting away from shell 0.*/
void CTerrain2::recentreShellsAfterScroll(const CShell & sender, Tdirection moveDirection) {
	vec3 move = dirToVec(moveDirection) * sender.SCsize;
	for (int shell = sender.shellNo; shell < shells.size(); shell++) {
		shells[shell].worldSpacePos += move;
		//if (shell == 0)
		//	liveLog << "\nshell 0 moved to " << shells[shell].worldSpacePos;
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
	//inUseChunks.clear();
	iota(begin(freeChunks), end(freeChunks), 0);
}

/** Return a chunk initialised with the given index. */
int CTerrain2::createChunk(glm::i32vec3& index, glm::vec3& sampleCorner, int shellNo, glm::vec3& terrainPos) {
	int id = getFreeChunk();
	chunks[id].index = index;
	chunks[id].cubeSize = LoD1cubeSize * pow(2, shellNo);
	chunks[id].LoD = shells[shellNo].LoD;
	chunks[id].setSample(sampleCorner, shells[shellNo].scSampleStep);
	chunks[id].terrainPos = terrainPos;
	chunks[id].status = chToSkin;
	chunksToMesh.push(id);
	return id;
}

/** Return the id of a free chunk, creating more chunks if necessary. */
int CTerrain2::getFreeChunk() {
	int id;
	if (freeChunks.size() > 0) {
		id = freeChunks.back();
		freeChunks.pop_back();
		//inUseChunks.push_back(id);
		return id;
	}

	int chunkInc = std::max((int)chunks.size() / 10,1);
	freeChunks.resize(chunkInc-1);
	id = chunks.size();
	iota(begin(freeChunks), end(freeChunks), id+1);
	chunks.resize(chunks.size() + chunkInc);
	//inUseChunks.push_back(id);
	return id;
}

/** Move this chunk  to the free pile. */
void CTerrain2::removeChunk(int id) {
	if (chunks[id].status == chSkinned)
		pCallbackApp->deleteChunkMesh(chunks[id]);

	chunks[id].status = chFree; 
	freeChunks.push_back(id);
}

/** Called every frame, used to maintain terrain in realtime. */
void CTerrain2::update(float dT) {

	//TO DO: base number on time available, not const value!
	unsigned int tmp = 10;// chunksToMesh.size(); //10
	for (int x = 0; x < tmp; x++) {  
		//are there any chunks in the queue? Mesh the next one.
		if (chunksToMesh.empty())
			return;

			int id = chunksToMesh.front();
			chunksToMesh.pop();
			if (chunks[id].status == chToSkin )
				pCallbackApp->createChunkMesh(chunks[id]);

		
	}
}

/** Return the NW bottom corner position of this SC in worldspace. */
glm::vec3 CTerrain2::getSCpos(int shellNo, const glm::i32vec3& origIndex) {
	i32vec3 rotatedIndex = shells[shellNo].getRotatedIndex(origIndex);
	return (shells[shellNo].worldSpacePos + vec3(rotatedIndex) * shells[shellNo].SCsize) -
		vec3(shells[shellNo].shellSCs * shells[shellNo].SCsize * 0.5f);
}

glm::vec3 CTerrain2::getSCpos2(int shellNo, const glm::i32vec3& origIndex) {
	i32vec3 rotatedIndex = shells[shellNo].getInvRotatedIndex(origIndex);;
	return (shells[shellNo].worldSpacePos + vec3(rotatedIndex) * shells[shellNo].SCsize) -
		vec3(shells[shellNo].shellSCs * shells[shellNo].SCsize * 0.5f);
}

/** Return the superchunk at this position in worldspace containing terrain, or NULL. */
CSuperChunk2* CTerrain2::getSC(const glm::vec3& pos) {
	for (auto shell : shells) {
		i32vec3 scIndex = shell.getSCat(pos);
		if (any(lessThan(scIndex, i32vec3 (0))))
			continue;
		CSuperChunk2* sc =  &shell.scArray.element(scIndex.x, scIndex.y, scIndex.z);
		if (!sc->isEmpty)
			return sc;
	}
	return NULL;
}


/** Return a pointer to a buffer of chunk triangles for the given position, if any. */
void CTerrain2::getTris(CSuperChunk2* sc, const glm::vec3& pos, TChunkVert2*& buf, unsigned int& noTris) {
	noTris = 0;
	float chunkSize = shells[sc->shellNo].chunkSize;
	//get chunk at this position
	int chunkId = -1;
	for (auto chunk : sc->scChunks) {
		if (all(greaterThanEqual(chunks[chunk].terrainPos, pos)) && 
				all(lessThanEqual(chunks[chunk].terrainPos, pos + vec3(chunkSize)))) {
			chunkId = chunks[chunk].bufId;
			break;
		}
	}

	if (chunkId == -1)
		return;


	//Use callback to getChunkTris? Or give terrain2 a pointer to the multibuf?
	//prob better to do the former, separating terrain model from mesh data 
	//it's a virtual call but can always optimise later

	

	//check if we already have this chunk in the cache
	for (int cacheNo = 0; cacheNo < chunkTriCacheSize2; cacheNo++) {
		if (cachedChunkTris[cacheNo].id == chunkId) {
			noTris = cachedChunkTris[cacheNo].noTris;
			buf = cachedChunkTris[cacheNo].buf;
			return;
		}
	}
	
	//no? Let's go retrieve it then
	unsigned int size = pCallbackApp->getChunkTrisCallback(chunkId, cachedChunkTris[freeChunkTriCache].buf);
	
	noTris = cachedChunkTris[freeChunkTriCache].noTris = size / (sizeof(TChunkVert2) * 3);
	cachedChunkTris[freeChunkTriCache].id = chunkId;
	buf = cachedChunkTris[freeChunkTriCache].buf;


	freeChunkTriCache++;
	if (freeChunkTriCache >= chunkTriCacheSize2)
		freeChunkTriCache = 0;
	
}

