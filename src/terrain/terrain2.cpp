#include "terrain2.h"

#include <iostream>
#include <numeric>      // std::iota

#include <glm/gtc/matrix_transform.hpp>	

#include "..\\3DEngine\src\utils\log.h"

#include  "physics2/collisions.h"

int tmpCount2;

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

	pRenderer = &CRenderer::getInstance();
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

/** We're removing this shell's chunks because they have scrolled out,
	so for each one tell the enclosing shell to create a new, lower-LOD chunk
	in the same area. */

void CTerrain2::overwriteInnerShellChunks(int shellNo, Tdirection scrollOutdir) {
	for (auto chunk : scrolledOutChunksToDelete) {
		if (shellNo < shells.size() - 1) {
			//find location of chunk in worldspace 
			//fnd the enclosing shell SC at that point
			//find the SC chunk index for that point
			//ask shell to create a chunk there
			CShell& innerShell = shells[shellNo];
			Chunk2& chunkData = chunks[chunk.second];
			glm::vec3 chunkPos = glm::vec3(chunkData.index) * innerShell.chunkSize;

			glm::vec3 scOffset = glm::vec3(chunk.first) * innerShell.SCsize;

			//proof of concept bodge
			//if (chunk.first.z == 0)
			//	scOffset.z = 5 * innerShell.SCsize;
			//to do this for real, pass scroll direction, use to find plane
			int scrollAxis = getAxis(scrollOutdir);
			if (scrollOutdir == south || scrollOutdir == east || scrollOutdir == up) {
				if (chunk.first[scrollAxis] == 0)
					 scOffset[scrollAxis] = innerShell.shellSCs * innerShell.SCsize;

			}
			else {
				if (chunk.first[scrollAxis] == innerShell.shellSCs - 1) {
					scOffset[scrollAxis] = 0 - innerShell.SCsize; //this is the fail!
					//removeChunk(chunk.second);
					//continue; //functionally the same
				}
	

			}


			chunkPos += scOffset;
			glm::vec3 shellOrigin = (vec3(innerShell.worldSpaceSize) * 0.5f) - innerShell.worldSpacePos;
			chunkPos -= shellOrigin;

			//find SC of enclosing shell at this point
			glm::i32vec3 outerSCindex = shells[shellNo + 1].getSCat(chunkPos);




			//find outer SC chunk index at this point 
			float outerSCsize = shells[shellNo + 1].SCsize;
			glm::vec3 scCornerOrigin = vec3(outerSCindex) * outerSCsize;
			scCornerOrigin -= shells[shellNo + 1].worldSpaceSize * 0.5; // make relative to centre of shell
			scCornerOrigin += shells[shellNo + 1].worldSpacePos; //and then relative to shell's worldspace position
			

			glm::vec3 pointInSC = chunkPos - scCornerOrigin;
			glm::i32vec3 chunkIndex = glm::i32vec3(pointInSC) / glm::i32vec3(shells[shellNo+1].chunkSize);
			chunkIndex = glm::abs(chunkIndex);

			//is there already a chunk there?
			CSuperChunk2* outerSC = shells[shellNo + 1].scArray.element(outerSCindex.x, outerSCindex.y, outerSCindex.z);
			bool chunkFound = false;
			for (auto localChunk : outerSC->scChunks) {
				if (chunks[localChunk].index == chunkIndex) {
					chunkFound = true;
					break;
				}
			}

			if (!chunkFound) {
				 outerSC->createChunk(chunkIndex);
			}

			outerSC->isEmpty = false;;


		}

		removeChunk(chunk.second);
	}
}


void CTerrain2::loadShaders() {
	//load chunk shader
	char* chunkFeedbackStrs[2];
	chunkFeedbackStrs[0] = "gl_Position";
	chunkFeedbackStrs[1] = "normal";
	chunkShader = pRenderer->createShader(pRenderer->dataPath + "chunk", chunkFeedbackStrs, 2);
	chunkShader->setType(userShader);

	//Upload data texture for chunk shader
	triTableTex = pRenderer->createDataTexture(intTex, 16, 256, &triTable);
	pRenderer->setShader(chunkShader);
	hChunkCubeSize = chunkShader->getUniformHandle("cubeSize");
	hChunkLoDscale = chunkShader->getUniformHandle("LoDscale");
	hChunkSamplePos = chunkShader->getUniformHandle("samplePos");
	hChunkTriTable = chunkShader->getUniformHandle("triTableTex");
	hChunkTerrainPos = chunkShader->getUniformHandle("terrainPos");
	hSamplesPerCube = chunkShader->getUniformHandle("samplesPerCube");
	hChunkTerrainTexture = chunkShader->getUniformHandle("terrainTexture");
	chunkShader->setShaderValue(hChunkTriTable, *triTableTex);

	//load chunkCheck shader
	chunkCheckShader = pRenderer->createShader(pRenderer->dataPath + "chunkCheck");
	pRenderer->setShader(chunkCheckShader);
	hNWsamplePos = chunkCheckShader->getUniformHandle("nwSamplePos");
	hLoDscale = chunkCheckShader->getUniformHandle("LoDscale");
	hTerrainTexture = chunkCheckShader->getUniformHandle("terrainTexture");

	//load te point finding shader
	char* fPointFeedbackStrs[1];
	fPointFeedbackStrs[0] = "newPoint";
	findPointHeightShader = pRenderer->createShader(pRenderer->dataPath + "findPointHeight", fPointFeedbackStrs, 1);
	hCurrentY = findPointHeightShader->getUniformHandle("currentY");
	hChunkSamplePosition = findPointHeightShader->getUniformHandle("chunkSamplePosition");
	hFPHSampleScale = findPointHeightShader->getUniformHandle("sampleScale");
	hChunkLocaliser = findPointHeightShader->getUniformHandle("chunkLocaliser");
	hFPHterrainTexture = findPointHeightShader->getUniformHandle("terrainTexture");

	//TO DO: temporarily commented out while I try to remove dependency on soil.h
	//grassTex = pRenderer->textureManager.getTexture(pRenderer->dataPath + "grassPack.dds");

	
	//load terrain surface point shader


	char* pointFeedbackStrs[1];
	pointFeedbackStrs[0] = "result";
	terrainPointShader = pRenderer->createShader(pRenderer->dataPath + "terrainPoint", pointFeedbackStrs, 1);
	terrainPointShader->setType(userShader);
	pRenderer->setShader(terrainPointShader);
	hPointSampleBase = terrainPointShader->getUniformHandle("sampleBase");
	hPointOffsetScale = terrainPointShader->getUniformHandle("offsetScale");
	hPSTerrainTexture = terrainPointShader->getUniformHandle("terrainTexture");

	//load heightpoint culling shader
	char* feedbackStrs[1];
	feedbackStrs[0] = "gl_Position";
	cullPointsShader = pRenderer->createShader(pRenderer->dataPath + "cullPoints", feedbackStrs, 1);
	cullPointsShader->setType(userShader);
	pRenderer->setShader(cullPointsShader);
	hSampleOffset = cullPointsShader->getUniformHandle("sampleOffset");
	hSampleScale = cullPointsShader->getUniformHandle("sampleScale");
	hMode = cullPointsShader->getUniformHandle("mode");

	wireBoxShader = pRenderer->createShader(pRenderer->dataPath + "wireBox");
	wireBoxShader->setType(userShader);
	hBoxColour = wireBoxShader->getUniformHandle("colour");
	hBoxMVP = wireBoxShader->getUniformHandle("mvpMatrix");
}

float CTerrain2::findTerrainHeight(glm::vec3& basePos) {
	pRenderer->setShader(terrainPointShader);
	float offsetScale = 1 / worldUnitsPerSampleUnit;
	terrainPointShader->setShaderValue(hPointOffsetScale, offsetScale);

	pRenderer->attachTexture(0, tmpTerrainMap.handle);
	terrainPointShader->setShaderValue(hPSTerrainTexture, 0);

	vec3 startPos = basePos;
	CBaseBuf* heightResultsBuf = pRenderer->createBuffer();
	heightResultsBuf->setSize(sizeof(float) * findHeightVerts);

	float* heightResults = new float[findHeightVerts];
	float terrainHeight = 0;; const float MCvertexTest = 0.5f;

	for (int step = 0; step < 100; step++) {
		terrainPointShader->setShaderValue(hPointSampleBase, startPos);
		pRenderer->getGeometryFeedback(heightFinderBuf, drawPoints, (CBuf&)*heightResultsBuf, drawPoints);

		heightResultsBuf->getData((unsigned char*)heightResults, sizeof(float) * findHeightVerts);
		for (int r = 0; r < findHeightVerts; r++) {
			if (heightResults[r] < MCvertexTest)
				terrainHeight = startPos.y + (r * offsetScale);
		}
		startPos.y += findHeightVerts * offsetScale;
	}
	delete heightResults;
	return terrainHeight;
}

void CTerrain2::initChunkShell() {
	float vertsPerEdge = numChunkCubes + 1;
	shellTotalVerts = std::pow(vertsPerEdge, 3) - std::pow(vertsPerEdge - 2, 3);
	vec3* shell = new vec3[shellTotalVerts];
	int v = 0;
	for (int y = 0; y < vertsPerEdge; y++) {
		for (int x = 0; x < vertsPerEdge; x++) {
			shell[v++] = vec3(x, y, 0);
			shell[v++] = vec3(x, y, numChunkCubes);
		}
		for (int z = 1; z < numChunkCubes; z++) {
			shell[v++] = vec3(0, y, z);
			shell[v++] = vec3(numChunkCubes, y, z);
		}
	}

	for (int x = 1; x < numChunkCubes; x++) {
		for (int z = 1; z < numChunkCubes; z++) {
			shell[v++] = vec3(x, 0, z);
			shell[v++] = vec3(x, numChunkCubes, z);
		}
	}

	chunkShell = new CRenderModel();

	CRenderMaterial* material = new CRenderMaterial();
	chunkShell->setMaterial(*material);
	chunkShell->setDrawMode(drawPoints);
	chunkShell->storeVertexes(shell, sizeof(vec3), v);
	chunkShell->storeLayout(3, 0, 0, 0);
	delete[] shell;
	chunkShell->getMaterial()->setShader(chunkCheckShader);
}

/** Initialise a 3D grid of points to represent the cubes of a chunk in drawing. */
void CTerrain2::initChunkGrid(int cubesPerChunkEdge) {
	int vertsPerEdge = cubesPerChunkEdge + 1;
	int noVerts = vertsPerEdge * vertsPerEdge * vertsPerEdge;
	vec3* shaderChunkVerts = new vec3[noVerts];
	int i = 0;
	for (float y = 0; y < vertsPerEdge; y++) {
		for (float z = 0; z < vertsPerEdge; z++) {
			for (float x = 0; x < vertsPerEdge; x++) {
				shaderChunkVerts[i++] = vec3(x, y, z);
			}
		}
	}


	//create an index that creates a bottom square for each cube in the grid
	int noIndices = cubesPerChunkEdge * cubesPerChunkEdge * cubesPerChunkEdge * 5;

	unsigned short layer = vertsPerEdge * vertsPerEdge;
	unsigned int* index = new unsigned int[noIndices];
	i = 0;
	unsigned short vertNo = 0;
	do {
		index[i++] = vertNo;
		index[i++] = vertNo + 1;
		index[i++] = vertNo + 1 + layer;
		index[i++] = vertNo + layer;


		index[i++] = 65535; //signals the end of this line sequence

		vertNo++;
		if (((vertNo + 1) % vertsPerEdge) == 0)
			vertNo++;
		if ((vertNo % layer) == (vertsPerEdge * (vertsPerEdge - 1)))
			vertNo += vertsPerEdge;

	} while (i < noIndices);

	//shaderChunkGrid = Engine.createModel();
	{shaderChunkGrid = new CRenderModel();
	//CMaterial* material = pRenderer. createMaterial();

	CRenderMaterial* material = new CRenderMaterial();
	material->setShader(pRenderer->phongShader);
	//materialList.push_back(material);
	shaderChunkGrid->setMaterial(*material);
	//modelList.push_back(model); 
	}

	shaderChunkGrid->setDrawMode(drawLinesAdjacency);



	shaderChunkGrid->storeVertexes(shaderChunkVerts, sizeof(vec3), noVerts);
	shaderChunkGrid->storeIndex(index, noIndices);
	shaderChunkGrid->storeLayout(3, 0, 0, 0);

	delete[] shaderChunkVerts;
	delete[] index;
}

/** Initialise shader and buffer required to run a terrain height-finding query. */
void CTerrain2::initHeightFinder() {


	//create verts
	vec3* v = new vec3[findHeightVerts];
	for (int x = 0; x < findHeightVerts; x++)
		v[x] = vec3(0, x, 0);

	heightFinderBuf.storeVertexes(v, sizeof(vec3) * findHeightVerts, findHeightVerts);
	heightFinderBuf.storeLayout(3, 0, 0, 0);
	delete v;
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
	if (shellNo <  shells.size() - 1) {
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

	float playerHalfWidth = 2.5f;//TEMP should import this!

	glm::vec3 corners[] = { { -playerHalfWidth,0,playerHalfWidth}, {playerHalfWidth,0,playerHalfWidth}, {playerHalfWidth,0,-playerHalfWidth}, {-playerHalfWidth,0,-playerHalfWidth} };

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
