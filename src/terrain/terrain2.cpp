#include "terrain2.h"

#include <iostream>
#include <numeric>      // std::iota

#include <glm/gtc/matrix_transform.hpp>	

#include "..\\3DEngine\src\utils\log.h"

using glm::vec3;
using glm::mat4;
using glm::i32vec3;

CTerrain2::CTerrain2()   {
	viewpoint = vec3(0);
	chunkOrigin = mat4(1);
	chunksToSkinPerFrame = 10;
	oldViewpoint = vec3(0);
	CSuperChunk2::pTerrainObj = this;;
};

/** Specify the size of the storage space for chunk vertices. This will silenty resize if more
	storage is required. */
void CTerrain2::setInitialChunkStorageSize(int bytes) {
	multiBuf.setSize(bytes);
}

/** Specify how chunk vertexes are stored, for use when drawing them. */
void CTerrain2::setChunkVertLayout(std::initializer_list<int> attribs) {
	multiBuf.storeLayout(attribs);
}

/** Create the central shell, establishing some variables for the terrain as a whole. */
void CTerrain2::createCentralShell(float LoD1cubeSize, int numChunkCubes, int numSCchunks, int numShellSCs)  {
	this->LoD1cubeSize = LoD1cubeSize;
	this->numChunkCubes = numChunkCubes;
	this->numSCchunks = numSCchunks;
	LoD1chunkSize = numChunkCubes * LoD1cubeSize;

	addShell(numShellSCs);
}

/** Add an outer shell to the existing shells, consisting of extent layers of SCs. */
void CTerrain2::addShell(int extent) {	
	int LoD = 1 << shells.size(); //1 2 4 8 etc
	float chunkSize = LoD1chunkSize * (LoD);

	if (shells.size() > 0)
		extent += shells.back().shellSCs;
	shells.push_back({ LoD, chunkSize, numSCchunks, extent });
	shells.back().pTerrain = this; //TO DO: scrap!
	shells.back().setTerrainCallbackObj(this);
	shells.back().shellNo = shells.size()-1;
	shells.back().initSuperChunks();
	shells.back().initChunkExtentToMinimum();
}


/** Respond to the player moving by the given vector. If the  
	cumulative viewpoint displacement > 1 chunk, advance the terrain .*/
void CTerrain2::onPlayerMove(glm::vec3 & move) {
	vec3 oldPosInChunks = glm::floor(viewpoint / LoD1chunkSize);
	viewpoint += move;
	vec3 newPosInChunks = glm::floor(viewpoint / LoD1chunkSize);

	i32vec3 terrainAdvanceInChunks = newPosInChunks - oldPosInChunks;
	Tdirection advanceDirection = vecToDir(terrainAdvanceInChunks);

	if (advanceDirection != none) {	
		for (auto & shell : shells) {
			if (shell.advanceShellTerrain(advanceDirection) == false)
				break;
		}
		removeShellOverlaps(advanceDirection);
	}
}


/** Fill all shells with chunks where they are intersected by the terrain. */
void CTerrain2::createTerrain() {
	initialiseChunkPool(approxChunksRequired);
	for (auto& shell : shells) {
		shell.initChunkExtentToMinimum();
		shell.createTerrain();
	}
}


/** Centre the terrain at the give position in sample space. */
void CTerrain2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

/** Set the number of world units per sample unit. */
void CTerrain2::setWorldScale(float scale) {
	worldToSampleScale = scale;
}

void CTerrain2::setCallbackApp(ITerrainAppCallback * pApp) {
	pCallbackApp = pApp;
}


/** Called every frame, used to maintain terrain in realtime. */
void CTerrain2::update(float dT) {
	for (int x = 0; x < chunksToSkinPerFrame; x++) {
		//are there any chunks in the queue? Mesh the next one.
		if (chunksToMesh.empty())
			return;

			int id = chunksToMesh.front();
			chunksToMesh.pop();
			if (chunks[id].status == chToSkin )
				pCallbackApp->createChunkMesh(chunks[id]);	
	}
}



/** Return the superchunk at this position in worldspace containing terrain, or NULL. */
TSCident CTerrain2::getSC(const glm::vec3& pos) {
	for (auto shell : shells) {
		i32vec3 scIndex = shell.getSCat(pos);
		if (any(lessThan(scIndex, i32vec3 (0))))
			continue;
		 CSuperChunk2* sc =  shell.scArray.element(scIndex.x, scIndex.y, scIndex.z);
		if (!sc->isEmpty)
			return {sc->shellNo,scIndex};
	}
	return { -1,glm::i32vec3(-1) };
}
//TO DO: weird pointer corruption if I return sc directly to terrainPhysObj2, as if shell sc vector gone out of scope
//FIX or replace when I sort out physics



/** Return a pointer to a buffer of chunk triangles for the given position, if any. */
void CTerrain2::getTris(CSuperChunk2* sc, const glm::vec3& pos, TChunkVert2*& buf, unsigned int& noTris) {
	return;
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
//TO DO: another one to FIX or replace when I sort out physics and what's actually required


int CTerrain2::storeChunkMesh(CBuf& src, int size) {
	return multiBuf.copyBuf(src, size);
}

void CTerrain2::freeChunkMesh(int addr) {
	multiBuf.freeBlock(addr);
}

unsigned int CTerrain2::getChunkMeshVAO() {
	return multiBuf.getVAO();
}

/** Sets the position of the viewer of this terrain, relative to its origin. */
void CTerrain2::setViewpoint(glm::vec3& pos) {
	viewpoint = pos; 
	oldViewpoint = pos;
}

ITerrainAppCallback* CTerrain2::getTerrainApp(){
	return pCallbackApp;
}

/** Return the worldspace size of the given shell. */
float CTerrain2::getShellSize(unsigned int shellNo) {
	return shells[shellNo].worldSpaceSize;
}

//////////Private functions

/** Prepare all chunks for use. */
void CTerrain2::initialiseChunkPool(int numChunks) {
	chunks.resize(numChunks);
	freeChunks.resize(numChunks);
	iota(begin(freeChunks), end(freeChunks), 0);
}

/** Iterate through the shells, removing any chunks now overlapped by chunks of an inner shell. */
void CTerrain2::removeShellOverlaps(Tdirection inDirection) {
	for (unsigned int shell = 1; shell < shells.size(); shell++) {
		shells[shell].removeOverlappedInnerFaceChunks(inDirection);
	}
}

/** Move the enclosing shells of the given shell one SC length in the given direction. This is called to keep the outer terrain
	aligned with a shell that has just scrolled. */
void CTerrain2::realignOuterShells(int shellNo, Tdirection moveDirection) {
	vec3 move = dirToVec(moveDirection) * shells[shellNo].SCsize;
	for (unsigned int outerShell = shellNo + 1; outerShell < shells.size(); outerShell++) {
		shells[outerShell].worldSpacePos += move;
	}
}

/** Move this shell and any enclosing shells one SC length in the given direction. This is called to keep their
	terrain in line with any inner shells after scrolling. It also prevents the shell from drifting away from shell 0.*/
void CTerrain2::recentreOnInnerShells(int shellNo, Tdirection moveDirection) {
	vec3 move = dirToVec(moveDirection) * shells[shellNo].SCsize;
	for (int shell = shellNo; shell < shells.size(); shell++) {
		shells[shell].worldSpacePos += move;
	}
}

/** Prepare for the given shell to scroll. Shells 1 and above must be moved, so that after scrolling their terrain still
	lines up with their inner shells. Shell 0 doesn't move, keeping the whole model centred, instead sample space is s
	scrolled. */
void CTerrain2::shellPreScrollUpdate(int shellNo, Tdirection direction) {
	if (shellNo == 0) {
		Tdirection scrollDir = flipDir(direction);
		scrollSampleSpace(scrollDir, shells[0].scSampleStep);

		vec3 move = dirToVec(scrollDir) * shells[0].SCsize;
		chunkOrigin = glm::translate(chunkOrigin, move);
		liveLog << "\nchunkOrigin moved to " << vec3(chunkOrigin[3]);
	}
	else
		recentreOnInnerShells(shellNo, direction);
}

/** Move the terrain's position in sample space. */
void CTerrain2::scrollSampleSpace(Tdirection scrollDir, float shift) {
	vec3 vec = dirToVec(flipDir(scrollDir)) * shift;
	sampleSpacePos += vec;
}

/** Return a chunk initialised to the region of sample space given by its index. */
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

/** Return the id of a chunk free for use, creating more chunks if necessary. */
int CTerrain2::getFreeChunk() {
	int id;
	if (freeChunks.size() > 0) {
		id = freeChunks.back();
		freeChunks.pop_back();
		//inUseChunks.push_back(id);
		return id;
	}

	int chunkInc = std::max((int)chunks.size() / 10, 1);
	freeChunks.resize(chunkInc - 1);
	id = chunks.size();
	iota(begin(freeChunks), end(freeChunks), id + 1);
	chunks.resize(chunks.size() + chunkInc);
	//inUseChunks.push_back(id);
	return id;
}

/** Take this chunk out of service, returning it to the free pile. */
void CTerrain2::removeChunk(int id) {
	if (chunks[id].status == chSkinned)
		pCallbackApp->deleteChunkMesh(chunks[id]);

	chunks[id].status = chFree;
	freeChunks.push_back(id);
}

/** Return the NW bottom corner position of this SC in worldspace. */
glm::vec3 CTerrain2::getSCworldPos(int shellNo, const glm::i32vec3& origIndex) {
	i32vec3 rotatedIndex = shells[shellNo].getInvRotatedIndex(origIndex);;
	return (shells[shellNo].worldSpacePos + vec3(rotatedIndex) * shells[shellNo].SCsize) -
		vec3(shells[shellNo].shellSCs * shells[shellNo].SCsize * 0.5f);
}

/** Update the viewpioint position in response to a scroll, so that the view remains the same. */
void CTerrain2::scrollViewpoint(Tdirection scrollDir) {
	glm::vec3& move = dirToVec(scrollDir) * shells[0].SCsize;
	pCallbackApp->onTerrainScroll(move);
	viewpoint += move;
	oldViewpoint += move;
}

/**	If we've removed scrolled-out chunks from this shell following a scroll, the rear
	inner face of the enclosing shell now needs to add chunks to cover that area of terrain. */
void CTerrain2::rebuildOuterShell(int shellNo, Tdirection scrollOutFace) {
	if (shellNo < shells.size() - 1) {
		shells[shellNo + 1].addInnerFaceChunks2(scrollOutFace);
	}
}

glm::i32vec3 CTerrain2::getChunkIndex(int chunkId) {
	return chunks[chunkId].index;
}
