#include "terrain.h"
#include <iostream> //for cerr
#include <algorithm> //for find

#include <glm/gtc/matrix_transform.hpp>	
//#include <glm/gtc/matrix_access.hpp>

using namespace glm;
using namespace vBuf;
using namespace std;

#include "watch.h"

using namespace watch;

#include "colour.h"



 int totalbufsize = 0;
 int totalchunks = 1;

CTerrain::CTerrain() : CModel() {
	pRenderer = &CRenderer::getInstance();
	totalTris = 0; 
	chunkOrigin = glm::translate(glm::mat4(1), getPos());
	scrollTriggerPoint = vec3(0);
	chunkOriginInt = i32vec3(0);
	chunkProcessDelay = 0;
	//sampleScale = 0.002f;

	//cachedChunkTris.buf = (TChunkVert*) new char[160000];
	freeChunkTriCache = 0;

	worldUnitsPerSampleUnit = 2560; //assuming a 5120m terrain volume. Adjust if it ends up bigger

}

void CTerrain::reserveBuf(unsigned int elementsUsed) {
		multiBuf.reserve(elementsUsed);
}

/** Set the dimensions and building block sizes of this terrain. */
void CTerrain::setSizes(int _chunksPerSChunkEdge, int _cubesPerChunkEdge, float _cubeSize) {
	chunksPerSChunkEdge = _chunksPerSChunkEdge;
	LoD1cubeSize = _cubeSize;
	cubesPerChunkEdge = _cubesPerChunkEdge;
}

/** Create a series of nested shells made of superchunks. */
void CTerrain::createShells(float terrainSize, float centreShellSize, int steps) {
	TShellCalcData calcData;
	int numShells = calcTotalShells((int)terrainSize,centreShellSize, steps, calcData);

	// we should now have enough info to create the SC shells
	float LoD1SCsize = LoD1cubeSize * cubesPerChunkEdge * chunksPerSChunkEdge; 
	T3dArray array3d1, array3d2;
	T3dArray* outerArray = &array3d1;
	T3dArray* innerArray = &array3d2;

	shells.resize(numShells);
	CTerrainLayer::LoD1chunkSize = LoD1cubeSize * cubesPerChunkEdge;
	worldSize = vec3(calcData.shellSize.front());

	int currentLoD = numShells;

	//create shells, working in from the outside
	vec3 nwLayerPos = vec3(calcData.shellSize.front()) * -0.5f;

	float shellThickness = 0;
	int shellThicknessInSCs = 0;
	float prevShellSize = calcData.shellSize.front();
	float prevSCsize = calcData.shellSuperchunkSize;
	float prevLoDscale = 0;
	vec3 samplesPerPrevSC(0);


	for (int shellNo = 0; shellNo < numShells; shellNo++) {
		float LoDscale = pow(2, numShells - (shellNo + 1));
		resize3dArray(*outerArray, i32vec3(calcData.superChunksPerShellEdge[shellNo]));
		shells[shellNo].nwLayerPos = vec3(calcData.shellSize[shellNo]) * -0.5f;
		shells[shellNo].cubeSize = LoD1cubeSize  *LoDscale;;
		shells[shellNo].LoD = currentLoD;
		shells[shellNo].scSize = LoD1SCsize * LoDscale;
		shells[shellNo].SCsampleStep = shells[shellNo].scSize / worldUnitsPerSampleUnit;
		shells[shellNo].chunkSampleStep = (shells[shellNo].cubeSize * cubesPerChunkEdge) / worldUnitsPerSampleUnit;
		shellThickness = (prevShellSize - calcData.shellSize[shellNo]) / 2;
		shellThicknessInSCs = int(shellThickness / prevSCsize);

		
		createSuperChunks(*outerArray, shells[shellNo].superChunks);
		initSuperChunks(*outerArray, shellNo,  i32vec3(chunksPerSChunkEdge));
		connectSuperChunks(*outerArray);
		findLayerFaces(*outerArray, shellNo);

		currentLoD = currentLoD - 1;

		if (shellNo > 0) { //stitch this layer into previous, outer layer
			shellThickness = abs(shells[shellNo - 1].nwLayerPos.x - shells[shellNo].nwLayerPos.x);
			shellThicknessInSCs = int(shellThickness / prevSCsize);// layerSCsize[shellNo - 1];
		
			insertLayer(*innerArray, *outerArray, shellThicknessInSCs);

			//remove redundant superChunks from outer layer
			hollowLayer(*innerArray, shellNo - 1, shellThicknessInSCs);
		}
			
		//maker this layer the next outer layer
		if (innerArray == &array3d1) {
			innerArray = &array3d2; outerArray = &array3d1;
		} else {
			innerArray = &array3d1; outerArray = &array3d2;
		}

		prevShellSize = calcData.shellSize[shellNo];
		prevSCsize = LoD1SCsize * LoDscale;
	    prevLoDscale = LoDscale;

		samplesPerPrevSC = vec3(shells[shellNo].scSize / worldUnitsPerSampleUnit);
	}
}

int CTerrain::calcTotalShells(int terrainSize, float centreShellSize, int numSteps, TShellCalcData& calcData) {
	float currentShellSize = centreShellSize; 
	float currentShellCubeSize = LoD1cubeSize; 
	float LoD1SCsize = currentShellCubeSize * cubesPerChunkEdge * chunksPerSChunkEdge; //s

	calcData.shellSize = { currentShellSize * 2 }; //s
	calcData.superChunksPerShellEdge = { int(calcData.shellSize.front() / LoD1SCsize) };  //s
	calcData.shellSuperchunkSize;  //s

	float LoDscale = 1.0f; //s
	float layerGrowth = 2.0f; 
	int currentStep = 0; 
	int numShells = 1;

	//make sure we have an even number of SCs along the edge:
	while ((calcData.superChunksPerShellEdge[0] % 2 != 0) || (calcData.superChunksPerShellEdge[0] / 2) % 2 != 0) {
		calcData.superChunksPerShellEdge[0] += 1;
		calcData.shellSize[0] = calcData.superChunksPerShellEdge[0] * LoD1SCsize;
		currentShellSize = calcData.shellSize[0] / 2;
	}

	//Expanding, see how many more shells we can fit into the remaining terain space
	while (calcData.shellSize.front() < terrainSize) {
		LoDscale *= 2; //halve the level of detail by doubling the size of the building blocks
		currentShellCubeSize = LoD1cubeSize * LoDscale;
		calcData.shellSuperchunkSize = LoD1SCsize * LoDscale;

		currentShellSize = currentShellSize * layerGrowth;
		float currentLayerSize = currentShellSize * 2;
		int SCsPerLayerEdge = int(currentLayerSize / calcData.shellSuperchunkSize);
		if (SCsPerLayerEdge % 2 != 0) { //can't have an odd number of SCs on an edge
			SCsPerLayerEdge += 1;
			currentLayerSize = SCsPerLayerEdge * calcData.shellSuperchunkSize;
			currentShellSize = currentLayerSize / 2;
		}

		//If we have no more steps, expand this layer to fit the total terrain size
		if (currentStep == numSteps) {
			while (currentLayerSize < terrainSize) {
				SCsPerLayerEdge += 2;
				currentLayerSize = SCsPerLayerEdge * calcData.shellSuperchunkSize;
				currentShellSize = currentLayerSize / 2;
			}

		}

		calcData.superChunksPerShellEdge.push_front(SCsPerLayerEdge);
		calcData.shellSize.push_front(currentLayerSize);
		numShells++;
		currentStep++;
	}

	return numShells;
}


/** Create chunks where needed for all shells. */
void CTerrain::createAllChunks() {
	for (size_t l=0;l<shells.size();l++) {
		for (size_t s = 0; s < shells[l].superChunks.size(); s++) {
			shells[l].superChunks[s]->createAllChunks();
		}
	}
	cerr << "\nTotal SCs " << totalScs;
	cerr << "\nPassed SCs " << passedSCs;
}

/** Utility function to resize 3d vectors/ */
void CTerrain::resize3dArray(T3dArray &scArray, i32vec3& size) {
	scArray.resize(size.x);
	for (int x=0; x<size.x; ++x) {
		scArray[x].resize(size.y);
		for (int y=0; y<size.y; ++y) {
			scArray[x][y].resize(size.z);
		}
	}
}

/** Create superchunks for the given 3d array of pointers, and fill the supplied vector with pointers to them too. */
void CTerrain::createSuperChunks(T3dArray &scArray, vector<CSuperChunk*>& parentLayer) {
	for (size_t x=0; x<scArray.size(); ++x) {
		for (size_t y=0; y<scArray[0].size(); ++y) {
			for (size_t z=0;z<scArray[0][0].size(); ++z) {
				CSuperChunk* sChunk = new CSuperChunk();
				sChunk->terrain = this;
				scArray[x][y][z] = sChunk;
				parentLayer.push_back(sChunk);
			}
		}
	}
}

extern CSuperChunk* dbgSC = NULL;

/** Initialise the given 3d array of superchunks with the correct position, size, lod, etc. */
void CTerrain::initSuperChunks(T3dArray &scArray, int layerNo, i32vec3& _sizeInChunks) {
	float LoDscale = float(1 << (shells[layerNo].LoD-1));
	int cubesPerSCedge =  chunksPerSChunkEdge * cubesPerChunkEdge;

	for (size_t x=0; x<scArray.size(); ++x) {
		for (size_t y=0; y<scArray[0].size(); ++y) {
			for (size_t z=0;z<scArray[0][0].size(); ++z) {
				CSuperChunk* sChunk = scArray[x][y][z];
				sChunk->nwWorldPos = vec3(x, y, z) * shells[layerNo].scSize;
				sChunk->setSizes(_sizeInChunks,cubesPerChunkEdge,shells[layerNo].cubeSize);		
				sChunk->LoD = shells[layerNo].LoD;
				sChunk->LoDscale = LoDscale;
				sChunk->sampleStep = shells[layerNo].SCsampleStep;
				sChunk->tmpIndex = i32vec3(x,y,z);
				if (x == 3 && y == 1 && z == 0 && layerNo == 1)
					dbgSC = sChunk;
				sChunk->genColour = vec4(col::randHue(),1);
				sChunk->layerNo = layerNo;
			}
		}
	}
}

/**	Create lists of which superchunks belong to which face of this layer.*/
void CTerrain::findLayerFaces(T3dArray &scArray, int layerNo) {
	size_t xSize = scArray.size();
	size_t ySize = scArray[0].size();
	size_t zSize = scArray[0][0].size();	

	for (size_t x=0; x<xSize; ++x) {
		for (size_t y=0; y<ySize; ++y) {
			for (size_t z=0;z<zSize; ++z) {
				if (x == 0)
					shells[layerNo].faceGroup[west].push_back(scArray[x][y][z]);
				if (x == xSize-1)
					shells[layerNo].faceGroup[east].push_back(scArray[x][y][z]);
				if (y == 0)
					shells[layerNo].faceGroup[down].push_back(scArray[x][y][z]);
				if (y == ySize-1)
					shells[layerNo].faceGroup[up].push_back(scArray[x][y][z]);
				if (z == 0)
					shells[layerNo].faceGroup[north].push_back(scArray[x][y][z]);
				if (z == zSize-1) {
					shells[layerNo].faceGroup[south].push_back(scArray[x][y][z]);
				}
			}
		}
	}
}




/** Connect via their n,e,s,w,u,d pointers all the superchunks in the given 3d array. */
void CTerrain::connectSuperChunks(T3dArray& tmpArray) { 
	for (size_t y=0;y<tmpArray[0].size();y++) {
		for (size_t z=0;z<tmpArray[0][0].size();z++) {
			for (size_t x=0;x<tmpArray.size();x++) {
				if (x>0) {
					tmpArray[x][y][z]->wSChunk = tmpArray[x-1][y][z];
					tmpArray[x-1][y][z]->eSChunk = tmpArray[x][y][z];
				}
				if (z>0) {
					tmpArray[x][y][z]->nSChunk = tmpArray[x][y][z-1];
					tmpArray[x][y][z-1]->sSChunk = tmpArray[x][y][z];
				}
				if (y>0) {
					tmpArray[x][y][z]->dSChunk = tmpArray[x][y-1][z];
					tmpArray[x][y-1][z]->uSChunk = tmpArray[x][y][z];
				}
			}
		}
	}
}

/**	Replace the central superchunks of one layer with the superchunks of an inner layer with a higher LoD. */
void CTerrain::insertLayer(T3dArray &outerArray, T3dArray &innerArray, int outerLayerThickness) {
	//iterate through the outer faces of the inner layer
	//and the inner faces of the outer layer, and attach them.
	Tdirection dir;
	CSuperChunk* innerSC, *outerSC, *outerNeighbour;
	int innerLoD = innerArray[0][0][0]->LoD;
	size_t inArray_size = innerArray.size();
	for (size_t x=0; x<inArray_size; ++x) {
		for (size_t y=0; y<inArray_size; ++y) {
			for (size_t z=0;z<inArray_size; ++z) {
				innerSC = innerArray[x][y][z];
				unsigned int a = outerLayerThickness + (x/2);
				unsigned int b = outerLayerThickness + (y/2);
				unsigned int c = outerLayerThickness + (z/2);
				outerSC = outerArray[a][b][c];
				for (int d=0;d<6;d++) { //for each direction...
					dir = Tdirection(d);
					if (innerSC->adj(dir) == NULL) { //if this inner SC points at nothing
						outerNeighbour =  outerSC->adj(dir);
						innerSC->adj(dir) = outerNeighbour; //point it at outer neighbour
						if (outerNeighbour->op(dir)->LoD != innerLoD) { //if outer neighbour still points back at an outer SC
							outerNeighbour->op(dir) = innerSC; //point it at this inner SC
						}
					}
				}	
			}
		}
	}
}


/** Remove the inner superChunks of a layer, at the given depth. */
void CTerrain::hollowLayer(T3dArray &layerArray, int layerNo, int depth) {
		CSuperChunk* sChunk;
		int innerLayerEnd = (layerArray.size()-1) - depth;
		typedef vector<CSuperChunk*>::iterator iter;

		for (int i=depth;i<=innerLayerEnd;i++) {
			for (int j=depth;j<=innerLayerEnd;j++) {
				for (int k=depth;k<=innerLayerEnd;k++) {
					sChunk = layerArray[i][j][k];
					iter pos = std::find(shells[layerNo].superChunks.begin(), shells[layerNo].superChunks.end(),sChunk);
					delete (*pos);
					shells[layerNo].superChunks.erase(pos);
				}
			}
		}
}




/** Return a pointer to a free chunk. */
Chunk* CTerrain::getFreeChunk() {
	Chunk* chunk;
	if (spareChunks.size() > 0) {
		chunk = spareChunks.back();
		spareChunks.pop_back();
	}
	else {
		chunk = new Chunk(); 
		allChunks.push_back(chunk);
	}
	chunk->init();
	return chunk;

}

/** List the given chunk for re-use. */
void CTerrain::prepareToFree(Chunk* chunk) {
	//toFree.push_back(chunk);
	//that's all. Don't try to change its status.

	freeChunk(*chunk);

	return;
}


void CTerrain::update() {
	
	//Skin the next chunk on the list that needs it, if any. */
	double startT = watch::pTimer->milliseconds();
	Chunk* chunk;

	


	size_t s = toSkin.size();
	while ((watch::pTimer->milliseconds() - startT) < 30) { //75 brings judder back, eats too many cycles
						//NB 30 can cause glitches for very fast alternation of north/west scrolling - not real life
		if (s == 0)
			break;
		
		chunk = toSkin.front();
		toSkin.pop_front(); 
		chunk->getSkinned();
		s--;
	}

	

	for (auto it = toFree.begin(); it != toFree.end();) {
		chunk = *it;
		if (chunk->status == chSkinned || chunk->status == chRemoveOnAlert) {
			freeChunk(*chunk);
			it = toFree.erase(it);
		}
		else
			++it;
	}
}


/** Advance the terrain in the given direction. */
void CTerrain::advance(Tdirection dir) {
	i32vec3 scrollVec = dirToVec(dir);

	int inner = shells.size()-1;
	//for (int l=inner;l>=0;l--) {
	for (int layerNo=0; layerNo <= inner; layerNo++) {  //from outer layer to inner
			Tdirection outgoingDir = flipDir(dir);

			//Move all chunks along by the size of LoD1, and maybe trigger the layer's scroll action.
			bool layerScrolled = shells[layerNo].advance(scrollVec); 
			
			//Check if this layer has now scrolled twice and thus reset 
			if (layerScrolled && shells[layerNo].resetCheck(scrollVec)) {
				if (layerNo > 0) {
					addTwoIncomingLayers(layerNo, dir);
				}
			}
	}
}




/** Call an external function that returns true if the isosurface doesn't intersect the given superChunk position.*/
/* bool CTerrain::superChunkIsEmpty(CSuperChunk& SC) {
	return EXTsuperChunkIsEmpty(SC);
} */


/** Call an external function that returns true if the isosurface intersects the given chunk position.*/
/*
bool CTerrain::chunkExists(vec3& samplePos, int LoD) {
	return EXTchunkExists(samplePos, LoD);
}
*/


/** Call an external function that creates geometry for the given chunk.*/
/*
void CTerrain::createChunkMesh(Chunk& chunk) {
	//EXTcreateChunkMesh(chunk);
} */

/** Call an external function to release the memory used by this chunk when it was registed.*/
void CTerrain::freeChunkModel(CModel* chunk) {
	totalTris -= ((Chunk*)chunk)->noTris;
	EXTfreeChunkModel(chunk);

}




/** Extend the given layer of superChunks by two rows in the face direction. */
void CTerrain::addTwoIncomingLayers(int layerNo, Tdirection face) {
	int zAxis = getAxis(face);
	i32vec3 facePos;

	Tdirection xStart,xEnd,yStart,yEnd;
	xStart = getXstart(face); xEnd = flipDir(xStart);
	yStart = getYstart(face); yEnd = flipDir(yStart);

	CSuperChunk* sc;
	for (size_t scNo=0;scNo<shells[layerNo].faceGroup[face].size();scNo++) { //for each face SC...
		sc = shells[layerNo].faceGroup[face][scNo];
		sc->addTwoIncomingLayers(face, xStart, yStart);
	} 
}



void CTerrain::freeChunk(Chunk & chunk) {
	if (chunk.id)
		multiBuf.deleteBlock(chunk.id);
	chunk.status = chFree;
	spareChunks.push_back(&chunk);
}

/** Return the superchunk at the given position. */
CSuperChunk * CTerrain::getSC(const glm::vec3 & pos) {
	float LoD1chunkSize = cubesPerChunkEdge * LoD1cubeSize;
	for (int layerNo = shells.size() - 1; layerNo >= 0; layerNo--) {
		vec3 scrolledPos = pos - (vec3(shells[layerNo].scrollState) * LoD1chunkSize);
		bvec3 inside = lessThanEqual(glm::abs(scrolledPos), abs(shells[layerNo].nwLayerPos));
		if ( all(inside) ) {
			vec3 offset = scrolledPos + glm::abs( shells[layerNo].nwLayerPos);
			i32vec3 index = offset / shells[layerNo].scSize;
			for (size_t sc = 0; sc < shells[layerNo].superChunks.size(); sc++) {
				if (shells[layerNo].superChunks[sc]->tmpIndex == index)
					return shells[layerNo].superChunks[sc];
			}
		}
	}
	return NULL;
}

/** Return the chunk, if any, at the given position. */
Chunk * CTerrain::getChunk(const glm::vec3 & pos) {
	CSuperChunk* sc = getSC(pos);
	if (!sc)
		return NULL;
	vec3 offset = pos + glm::abs(shells[sc->layerNo].nwLayerPos); //+ glm::abs( sc->nwWorldPos);
	offset -= sc->nwWorldPos;
	
	i32vec3 index = offset / sc->chunkSize;
	
	for (size_t chunk = 0; chunk < sc->chunkList.size(); chunk++) {
		if (sc->chunkList[chunk]->scIndex == index)
			return sc->chunkList[chunk];
	}

	return NULL;
}

void CTerrain::getChunkTris(Chunk & chunk, TChunkVert * buf) {
	//get the id of this chunk
	unsigned int id = chunk.id;
	//copy the data from the multibuf to system memory
	multiBuf.copyBlock(id, (char*)buf);

}

/** Return a pointer to a buffer of chunk triangles for the given position, if any. */
void CTerrain::getTris(const glm::vec3& pos, TChunkVert* & buf, unsigned int& noTris) {
	noTris = 0;
	CSuperChunk* sc = getSC(pos); //TO TO: temp!
	if (!sc)
		return;

	//get chunk at this position
		Chunk* chunk = getChunk(pos);
	if (!chunk) {
	
		return;
	}

	if (chunk->id == 0) //TO DO: shouldn't get chunks like this, but they seem to happen occassionally. Investigate.
		return;



	//check if we already have this chunk in the cache
	for (int cacheNo = 0; cacheNo < chunkTriCacheSize; cacheNo++) {
		if (cachedChunkTris[cacheNo].id == chunk->id) {
			noTris = cachedChunkTris[cacheNo].noTris;
			buf = cachedChunkTris[cacheNo].buf;
			return;
		}
	}

	//no? Let's go retrieve it then
	getChunkTris(*chunk, cachedChunkTris[freeChunkTriCache].buf);
	unsigned int size = multiBuf.getBlockSize(chunk->id);
	noTris = cachedChunkTris[freeChunkTriCache].noTris = size / (sizeof(TChunkVert) * 3);
	cachedChunkTris[freeChunkTriCache].id = chunk->id;
	buf = cachedChunkTris[freeChunkTriCache].buf;


	freeChunkTriCache++;
	if (freeChunkTriCache >= chunkTriCacheSize)
		freeChunkTriCache = 0;
}

/** Return the position of the nw bottom corner of the chunk pos is inside. */
glm::vec3 CTerrain::getChunkPos(const glm::vec3 & pos) {
	CSuperChunk* sc = getSC(pos);
	if (!sc)
		return vec3(0);
	vec3 offset = pos + glm::abs(shells[sc->layerNo].nwLayerPos); 
	offset -= sc->nwWorldPos;

	vec3 cornerPos =  (offset / sc->chunkSize) * sc->chunkSize;

	return cornerPos;
}


/** Remove all current terrain and free up the memory. */
void CTerrain::clear() {
	for (size_t layerNo = 0; layerNo < shells.size(); layerNo++) {
		for (size_t scNo = 0; scNo < shells[layerNo].superChunks.size(); scNo++) {
			CSuperChunk* sc = shells[layerNo].superChunks[scNo];
			sc->removeAllChunks();
		}
	}	
}

/** Set the point in sample-space that this terrain will centre on when created. */
void CTerrain::setSampleCentre(glm::vec3 & centrePos) {
	sampleOffset = centrePos;
	for (size_t layerNo = 0; layerNo < shells.size(); layerNo++) {
		vec3 nwCorner = (shells[layerNo].nwLayerPos / worldUnitsPerSampleUnit)
			+ centrePos;
		shells[layerNo].nwSampleCorner = nwCorner;
		for (size_t scNo = 0; scNo < shells[layerNo].superChunks.size(); scNo++) {
			CSuperChunk* sc = shells[layerNo].superChunks[scNo];
			sc->setSamplePos(nwCorner + vec3(sc->tmpIndex) * sc->sampleStep);
		}
	}
}

/** Fill the visibleSClist with SCs that are visible in the given camera frustum. */
void CTerrain::updateVisibleSClist(glm::mat4& camMatrix) {
	CSuperChunk* sc;
	visibleSClist.clear();
	for (size_t layerNo = 0; layerNo < shells.size(); layerNo++) {
		int slSize = shells[layerNo].superChunks.size();
		for (int scNo = 0; scNo < slSize; scNo++) {
			sc = shells[layerNo].superChunks[scNo];
			//if (!sc->isOutsideFustrum(camMatrix)) {
				visibleSClist.push_back(sc);
			//}
		}
	}
}

/**	Draw all the chunks belonging to superchunks on the 'visible' list this frame. */
void CTerrain::drawVisibleChunks() {
	CSuperChunk* sc;
	int childBuf = -1;
	int scListSize = visibleSClist.size();
	for (int scNo = 0; scNo < scListSize; scNo++) {
		sc = visibleSClist[scNo];
		int clSize = sc->chunkList.size();
		for (int chunkNo = 0; chunkNo < clSize; chunkNo++) {
			Chunk* chunk = sc->chunkList[chunkNo];
			chunkDrawShader->setShaderValue(pRenderer->hColour,chunk->drawDetails.colour);
			if (chunk->drawDetails.childBufNo == -1)
				continue; //chunk not skinned yet - TO DO: find a better way to do this
			pRenderer->drawMultiBufChildVerts(drawTris, multiBuf, chunk->drawDetails.childBufNo, chunk->drawDetails.vertStart, chunk->drawDetails.vertCount);
		}
	}
}

/** Run through the grass multibuf, drawing grass. */
void CTerrain::drawGrass(glm::mat4& mvp, std::vector<CSuperChunk*>& drawList) {
	pRenderer->backFaceCulling(false);
	 CSuperChunk* sc; 
	int drawListSize = drawList.size(); int clSize;
	for (int scNo = 0; scNo < drawListSize; scNo++) {
		sc = drawList[scNo];
		if (sc->LoD != 1)
			continue;
		clSize = sc->chunkList.size();
		for (int chunkNo = 0; chunkNo < clSize; chunkNo++) { 
			Chunk* chunk = sc->chunkList[chunkNo];
			if (chunk->grassDrawDetails.childBufNo != -1) {
				pRenderer->drawMultiBufChildVerts(drawPoints, grassMultiBuf, chunk->grassDrawDetails.childBufNo, chunk->grassDrawDetails.vertStart, chunk->grassDrawDetails.vertCount);
			}
		}
	}
	pRenderer->backFaceCulling(true);
}

/** Run through the grass multibuf, drawing instanced trees. */
void CTerrain::drawTrees(glm::mat4& mvp, std::vector<CSuperChunk*>& drawList) {
	CSuperChunk* sc;
	int drawListSize = drawList.size(); int clSize;
	int callsPerFrame = 0;
	for (int scNo = 0; scNo < drawListSize ; scNo++) 
	{
		sc = drawList[scNo];
		if (sc->LoD != 1)
			continue;
		clSize = sc->chunkList.size();
		for (int chunkNo = 0; chunkNo < clSize; chunkNo++) {
			Chunk* chunk = sc->chunkList[chunkNo];
			if (chunk->treeDrawDetails.childBufNo != -1) {
				pRenderer->drawMultiBufChildInstanced(drawTriStrip, treeMultiBuf, chunk->treeDrawDetails.childBufNo, chunk->treeDrawDetails.vertStart, chunk->treeDrawDetails.vertCount);
				callsPerFrame++;
			}
		}
	}
	//cerr << "\nCalls per frame " << callsPerFrame;
	pRenderer->setVAO(0);
}


/** The draw-yourself function. */
void CTerrain::draw() {
	//pRenderer->setShader(Engine.phongShader);
	//material->assign();

	//Engine.phongShader->setNormalModelToCameraMatrix(tmp); //why am I doing this?
	//Engine.phongShader->setMVP(mvp);


}

CTerrain::~CTerrain() {
	for (size_t c=0;c<spareChunks.size();c++) {
		delete spareChunks[c];
	}
	
	for (size_t l=0;l<shells.size();l++) {
		for (size_t s=0;s<shells[l].superChunks.size();s++) {
			delete shells[l].superChunks[s];
		}
	}
//	if (cachedChunkTris.buf)
//		delete cachedChunkTris.buf;
}



CTerrainLayer::CTerrainLayer() {
	scrollState = i32vec3(0);
	resetState = i32vec3(0);
}



/** Advance the terrain of this layer in a given direction, scrolling as necessary. Returns true if a scroll
	occurs. */
bool CTerrainLayer::advance(i32vec3& scrollVec) {
	scrollState += scrollVec;

	nwLayerPos -= vec3(scrollVec) * LoD1chunkSize; //Move whole layer in response to this advance

	//If this layer has been advanced to its scrolling point, scroll it
	int scrollAxis = getAxis(scrollVec);
	int scrollPoint = 1 << (LoD-1);
	if (abs(scrollState[scrollAxis]) == scrollPoint ) {
		scrollState[scrollAxis] = 0;
		scroll(scrollVec);
		return true;
	}

	return false;
}

/** Scroll this terrain layer in the given direction. Any chunks are moved one slot over in their superchunk virtual arrays,
	then all superchunks have their scroll method called. */
void CTerrainLayer::scroll(i32vec3& scrollVec) {
	Tdirection scrollDir = vecToDir(scrollVec);
	int scrollAxis = getAxis(scrollVec);

	float LoDscale = (float) (1 << (LoD-1));
	vec3 scrollStep = vec3(scrollVec ) * LoD1chunkSize * LoDscale;
	for (size_t s=0;s<superChunks.size();s++) { 
		for (size_t c=0;c<superChunks[s]->chunkList.size();c++) {
			superChunks[s]->chunkList[c]->scIndex += -scrollVec;
		}
		superChunks[s]->retractChunkSpace(scrollDir); //shrink boundary where we've moved chunks out
		superChunks[s]->extendChunkSpace(flipDir(scrollDir)); //extend boundary where we've pushed chunks along
	}

	//we're going to scroll this layer, so return it to its initial position first.
	nwLayerPos += scrollStep;
	nwSampleCorner += vec3(-scrollVec) * chunkSampleStep;
	for (size_t s=0;s<superChunks.size();s++) {
		superChunks[s]->scroll(scrollVec);
	}
}

/** Return the chunk of this layer, if any, at this position. */
Chunk * CTerrainLayer::getChunkAt(glm::vec3 & pos) {
	CSuperChunk* nearestSC = getNearestSC(pos); 

	return nullptr;
}

/** Return the nearest superChunk to this position, clamping to the outer sides of the
	layer 'shell'.*/
CSuperChunk * CTerrainLayer::getNearestSC(glm::vec3 & pos) {
	//find index for this position

	//return SC for this index

	return nullptr;
}

bool CTerrainLayer::resetCheck(glm::i32vec3 & scrollVec) {
	int scrollAxis = getAxis(scrollVec);
	resetState += scrollVec;
	if (abs(resetState[scrollAxis]) > 1) {
		resetState[scrollAxis] = 0;
		return true;
	}
	return false;
}

float CTerrainLayer::LoD1chunkSize = 0;
