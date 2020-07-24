#include "terrain2.h"

#include <iostream>
#include <numeric>      // std::iota

#include <glm/gtc/matrix_transform.hpp>	

#include "..\\3DEngine\src\utils\log.h"

#include  "physics2/collisions.h"

using glm::vec3;
using glm::mat4;
using glm::i32vec3;

CTerrain2::CTerrain2()   {
	viewpoint = vec3(0);
	chunkOrigin = mat4(1);
	chunksToSkinPerFrame = 10;
	oldViewpoint = vec3(0);
	CSuperChunk2::pTerrainObj = this;

	chunkDataCache.setMultiBuf(&multiBuf);
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

	if (advanceDirection == down) {
			return;
	}

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
	pTerrainApp = pApp;
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
				pTerrainApp->createChunkMesh(chunks[id]);	
	}
}



int CTerrain2::storeChunkMesh(CBuf& src, int size) {
	return multiBuf.copyBuf(src, size);
}

void CTerrain2::freeChunkMesh(int addr) {
	multiBuf.freeBlock(addr);
	chunkDataCache.freeBuf(addr);
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
	return pTerrainApp;
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
void CTerrain2::prepShellForScroll(int shellNo, Tdirection direction) {
	if (shellNo == 0) {
		Tdirection scrollDir = flipDir(direction);
		scrollSampleSpace(scrollDir, shells[0].scSampleStep);

		vec3 move = dirToVec(scrollDir) * shells[0].SCsize;
		chunkOrigin = glm::translate(chunkOrigin, move);

		
		scrollViewpoint(scrollDir);
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
		pTerrainApp->deleteChunkMesh(chunks[id]);

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
	pTerrainApp->onTerrainScroll(move);
	viewpoint += move;
	oldViewpoint += move;
}

/**	If we've removed scrolled-out chunks from this shell following a scroll, the rear
	inner face of the enclosing shell now needs to add chunks to cover that area of terrain. */
void CTerrain2::rebuildOuterShellInnerFace(int shellNo, Tdirection scrollOutFace) {
	if (shellNo < shells.size() - 1) {
		shells[shellNo + 1].addInnerFaceChunks2(scrollOutFace);
	}
}

glm::i32vec3 CTerrain2::getChunkIndex(int chunkId) {
	return chunks[chunkId].index;
}


TAaBB CTerrain2::calcAABB() {
	glm:vec3 BBextent = glm::vec3(getShellSize(0) * 0.5f);
	TAaBB AaBb = { -BBextent, BBextent };
	glm::mat4 worldMatrix = glm::translate(glm::mat4(), shells[0].worldSpacePos);
	//TO DO: don't need matrix if we just use shell 0  - see how we go
	glm::vec4 AABBmin = worldMatrix * glm::vec4(-BBextent, 1);
	glm::vec4 AABBmax = worldMatrix * glm::vec4(BBextent, 1);
	return { glm::vec3(AABBmin), glm::vec3(AABBmax) };
}

Contact CTerrain2::checkCollision(CPhysObj2* objB) {
	Contact contact;
	if (this < objB) {
		contact.objA = this;
		contact.objB = objB;
	}
	else {
		contact.objB = this;
		contact.objA = objB;
	}

	glm::vec3 baseVertB = objB->calcBaseVertPos();
	TAaBB objAbb = calcAABB();

	glm::vec3 corners[] = { { -20,0,20}, {20,0,20}, {20,0,-20}, {-20,0,-20} };

	float penetration = 0;
	TChunkTriBuf2* chunkData;
	if (objAbb.clips(baseVertB)) { //we're inside shell 0

		for (int corner = 0; corner < 4; corner++) {
			chunkData = getShell0ChunkDataAt(baseVertB + corners[corner]);
			if (chunkData == NULL) {
				//check the chunk above in case we dropped straight through
				glm::vec3 chunkAbove = baseVertB + corners[corner] + glm::vec3(0, LoD1chunkSize, 0);
				TChunkTriBuf2* chunkAboveData = getShell0ChunkDataAt(chunkAbove);
				if (chunkAboveData == NULL) {
					//return contact;
					continue;
				}
				chunkData = chunkAboveData;
			}

			baseVertB = baseVertB - glm::vec3(chunkOrigin[3]); //move to chunk worldspace position

			penetration = checkChunkCollision(baseVertB + corners[corner], chunkData);
			if (penetration > 0) {
				contact.normal = glm::vec3(0, 1, 0);
				contact.numPoints = 1;
				contact.points[0] = { baseVertB + corners[corner], penetration };
				return contact; //for now, exit on first contact
			}

		}

		//got here? Then we've made no contact
		//liveLog << "\nNo contact!";
		
	}

	return contact;
}

TChunkTriBuf2* CTerrain2::getShell0ChunkDataAt(glm::vec3& pos) {
	glm::i32vec3 scIndex = shells[0].getSCat(pos);
	CSuperChunk2* sc = shells[0].scArray.element(scIndex.x, scIndex.y, scIndex.z);
	if (sc->isEmpty)
		return NULL;

	float SCsize = shells[0].SCsize;

	glm::vec3 scCornerOrigin = vec3(scIndex) * SCsize;
	scCornerOrigin -= shells[0].worldSpaceSize * 0.5; // make relative to centre of shell
	scCornerOrigin += shells[0].worldSpacePos; //and then relative to shell's worldspace position

	glm::vec3 SCorigin = scCornerOrigin + (SCsize * 0.5f); //move orgin to centre of SC
	float topHeight = SCorigin.y + (SCsize * 0.5);

	//find chunk at baseVertB - if any
	glm::vec3 pointInSC = pos - scCornerOrigin;
	glm::i32vec3 chunkIndex = glm::i32vec3(pointInSC) / glm::i32vec3(shells[0].chunkSize);


	//get the id for the chunk at this index position
	int chunkAddr = -1;
	for (auto localChunk : sc->scChunks) {
		if (chunks[localChunk].index == chunkIndex) {
			chunkAddr = chunks[localChunk].bufId;
			break;
		}
	}
		 
	if (chunkAddr == -1)
		return NULL;



	return chunkDataCache.getData(chunkAddr);
}

/** Return the contact, if any, between an upright line segment ending at the base vector,
and the triangles in the chunk data. */
float CTerrain2::checkChunkCollision(glm::vec3& baseVector, TChunkTriBuf2* chunkData) {

	float penetration = 0;
	//check for intersection
	glm::vec3 segTop = baseVector + glm::vec3(0, LoD1chunkSize, 0);
	float u, v, w, t;
	int vertNo = 0;

	for (int tri = 0; tri < chunkData->noTris; tri++) {
		bool intersect = triSegmentIntersection(segTop, baseVector, chunkData->buf[vertNo].v, chunkData->buf[vertNo + 1].v, chunkData->buf[vertNo + 2].v, u, v, w, t);

		if (intersect) {
			glm::vec3 intersectionPoint = (chunkData->buf[vertNo].v * u) + (chunkData->buf[vertNo + 1].v * v) + (chunkData->buf[vertNo + 2].v * w);
			float contactDistance = glm::length(segTop - intersectionPoint);

			/*vec3 a = chunkData->buf[vertNo + 1].v - chunkData->buf[vertNo].v;
			vec3 b = chunkData->buf[vertNo + 2].v - chunkData->buf[vertNo].v;
			vec3 triNorm = normalize(cross(a, b));
			vec3 contactDir = triNorm;*/
		
			penetration = glm::length(intersectionPoint - baseVector);
			break;
		}

		vertNo += 3;
	}

	return penetration;
}
