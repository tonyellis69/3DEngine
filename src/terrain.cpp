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

CTerrain::CTerrain() : CModelMulti() {
	totalTris = 0;
	chunkOrigin = glm::translate(glm::mat4(1), getPos());
	scrollTriggerPoint = vec3(0);
	chunkOriginInt = i32vec3(0);
	chunkProcessDelay = 0;
	sampleScale = 0.002f;
}

/** Set the dimensions and building block sizes of this terrain. */
void CTerrain::setSizes(int _chunksPerSChunkEdge, int _cubesPerChunkEdge, float _cubeSize) {
	float superChunkSize = _cubeSize * _cubesPerChunkEdge * _chunksPerSChunkEdge ;
	chunksPerSChunkEdge = _chunksPerSChunkEdge;
	LoD1cubeSize = _cubeSize;
	cubesPerChunkEdge = _cubesPerChunkEdge;
	totalTris = 0;
}

/** Create each nested layer of superchunks. */
void CTerrain::createLayers(int superChunksPerTerrainEdge, int noLayers, int layerThickness) {
	T3dArray array3d1, array3d2;
	int currentLoD = noLayers;
	float currentLoDscale = (float) (1 << (currentLoD-1));
	float currentLoDcubeSize = LoD1cubeSize * currentLoDscale;

	//First, create an initial layer, which will become the outmost layer
	float baseLayerSCsize = currentLoDcubeSize * cubesPerChunkEdge * chunksPerSChunkEdge ;
	float baseLayerSize = baseLayerSCsize * superChunksPerTerrainEdge ;
	vec3 nwLayerPos = vec3(baseLayerSize) * -0.5f;

	int cubesPerSCedge =  chunksPerSChunkEdge * cubesPerChunkEdge;
	int baseLayerSampleSize = (int)(superChunksPerTerrainEdge * cubesPerSCedge * currentLoDscale *sampleScale );
	vec3 nwLayerSamplePos(baseLayerSampleSize/2.0f);
	nwLayerSamplePos = vec3(0) - nwLayerSamplePos;


	layers.resize(noLayers); 

	//create all our superchunks, and a temporary 3D array referencing them
	T3dArray* outerArray = &array3d1;
	T3dArray* innerArray = &array3d2 ;
	int superChunksPerLayerEdge = superChunksPerTerrainEdge;

	int layerNo =0;

	resize3dArray(*outerArray,i32vec3(superChunksPerLayerEdge));
	layers[layerNo].nwLayerPos = nwLayerPos;
	layers[layerNo].cubeSize = currentLoDcubeSize;
	layers[layerNo].LoD = currentLoD ;
	layers[layerNo].LoD1cubeSize = LoD1cubeSize;
	layers[layerNo].cubesPerChunkEdge = cubesPerChunkEdge;


	createSuperChunks(*outerArray,layers[layerNo].superChunks);
	initSuperChunks(*outerArray,layerNo,nwLayerSamplePos,i32vec3(chunksPerSChunkEdge));
	connectSuperChunks(*outerArray);
	findLayerFaces(*outerArray,layerNo);
	//base layer created!

	//create inner layers
	int outerLayerThickness = layerThickness;
	float outerlayerSCsize = baseLayerSCsize;
	float outerLoDScale;
	while (currentLoD > 1) {
		outerLoDScale = currentLoDscale;
		currentLoD = currentLoD - 1;
		currentLoDscale = float(1 << (currentLoD-1));
		currentLoDcubeSize = LoD1cubeSize * currentLoDscale;
		layerNo++;

		//create the new 3d array of smaller superchunks
		superChunksPerLayerEdge = (superChunksPerLayerEdge - (outerLayerThickness * 2))  * 2;
		resize3dArray(*innerArray,i32vec3(superChunksPerLayerEdge));

		nwLayerSamplePos += vec3(outerLayerThickness * cubesPerChunkEdge * outerLoDScale * chunksPerSChunkEdge * sampleScale);

		layers[layerNo].nwLayerPos = nwLayerPos += vec3(outerLayerThickness * outerlayerSCsize);
		layers[layerNo].cubeSize = currentLoDcubeSize;
		layers[layerNo].LoD = currentLoD;
		layers[layerNo].LoD1cubeSize = LoD1cubeSize;
		layers[layerNo].cubesPerChunkEdge = cubesPerChunkEdge;
		createSuperChunks(*innerArray,layers[layerNo].superChunks); 
		initSuperChunks(*innerArray,layerNo,nwLayerSamplePos,i32vec3(chunksPerSChunkEdge)); 
		connectSuperChunks(*innerArray);
		findLayerFaces(*innerArray,layerNo);

		//stitch inner layer into outer layer
		insertLayer(*outerArray,*innerArray,outerLayerThickness);
		///TO DO: can probably get rid of all that crap, currently there's no reason to connect SCs of one layer to
		///the SCs of another.

		//remove redundant superChunks from outer layer
		hollowLayer(*outerArray,layerNo-1,outerLayerThickness);

		//All done
		//maker inner layer the next outer layer
		if (innerArray == &array3d1) {
			innerArray = &array3d2; outerArray =  &array3d1; }
		else {
			innerArray = &array3d1; outerArray =  &array3d2;}

		outerlayerSCsize = outerlayerSCsize/2 ;
		outerLayerThickness = outerLayerThickness; //for now
	}

}



/** Create chunks where needed for all layers. */
void CTerrain::createAllChunks() {
	for (size_t l=0;l<layers.size();l++) {
		for (size_t s=0;s<layers[l].superChunks.size();s++)
			layers[l].superChunks[s]->createAllChunks();
	}
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
void CTerrain::initSuperChunks(T3dArray &scArray, int layerNo, vec3 nwLayerSamplePos, i32vec3& _sizeInChunks) {
	vec3 thisSuperChunkSize(_sizeInChunks);
	thisSuperChunkSize *= layers[layerNo].cubeSize * cubesPerChunkEdge ;
	float LoDscale = float(1 << (layers[layerNo].LoD-1));
	int cubesPerSCedge =  chunksPerSChunkEdge * cubesPerChunkEdge;
	float layerSCsampleRange = cubesPerSCedge * LoDscale * sampleScale; 

	for (size_t x=0; x<scArray.size(); ++x) {
		for (size_t y=0; y<scArray[0].size(); ++y) {
			for (size_t z=0;z<scArray[0][0].size(); ++z) {
				CSuperChunk* sChunk = scArray[x][y][z];
				sChunk->nwWorldPos = vec3(x,y,z) * thisSuperChunkSize;
				sChunk->setSizes(_sizeInChunks,cubesPerChunkEdge,layers[layerNo].cubeSize);		
				sChunk->LoD = layers[layerNo].LoD;
				sChunk->LoDscale = LoDscale;
				sChunk->setSamplePos(nwLayerSamplePos + (vec3(x,y,z) * layerSCsampleRange));
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
					layers[layerNo].faceGroup[west].push_back(scArray[x][y][z]);
				if (x == xSize-1)
					layers[layerNo].faceGroup[east].push_back(scArray[x][y][z]);
				if (y == 0)
					layers[layerNo].faceGroup[down].push_back(scArray[x][y][z]);
				if (y == ySize-1)
					layers[layerNo].faceGroup[up].push_back(scArray[x][y][z]);
				if (z == 0)
					layers[layerNo].faceGroup[north].push_back(scArray[x][y][z]);
				if (z == zSize-1) {
					layers[layerNo].faceGroup[south].push_back(scArray[x][y][z]);
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
					iter pos = std::find(layers[layerNo].superChunks.begin(), layers[layerNo].superChunks.end(),sChunk);
					delete (*pos);
					layers[layerNo].superChunks.erase(pos);
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
	while ((watch::pTimer->milliseconds() - startT) < 75) { //75 brings judder back, eats too many cycles
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

	size_t inner = layers.size()-1;
	//for (int l=inner;l>=0;l--) {
	for (int layerNo=0; layerNo <= inner; layerNo++) {  //from outer layer to inner
			Tdirection outgoingDir = flipDir(dir);

			//Move all chunks along by the size of LoD1, and maybe trigger the layer's scroll action.
			bool layerScrolled = layers[layerNo].advance(scrollVec); 
			
			//Check if this layer has now scrolled twice and thus reset 
			if (layerScrolled && layers[layerNo].resetCheck(scrollVec)) {
				if (layerNo > 0) {
					addTwoIncomingLayers(layerNo, dir);
				}
			}
	}
}




/** Call an external function that returns true if the isosurface doesn't intersect the given superChunk position.*/
bool CTerrain::superChunkIsEmpty(vec3& samplePos, int LoD) {
	return EXTsuperChunkIsEmpty(samplePos, LoD);
}


/** Call an external function that returns true if the isosurface intersects the given chunk position.*/
bool CTerrain::chunkExists(vec3& samplePos, int LoD) {
	return EXTchunkExists(samplePos, LoD);
}



/** Call an external function that creates geometry for the given chunk.*/
void CTerrain::createChunkMesh(Chunk& chunk) {
	EXTcreateChunkMesh(chunk);
}

/** Call an external function to release the memory used by this chunk when it was registed.*/
void CTerrain::freeChunkModel(CModel* chunk) {
	totalTris -= ((Chunk*)chunk)->noTris;
	EXTfreeChunkModel(chunk);

}




/** Extend the given layer of superChunks by two rows in the face direction. */
void CTerrain::addTwoIncomingLayers(int layerNo, Tdirection face) {
	int zAxis = getAxis(face);
	i32vec3 facePos;
	Chunk* newChunk;

	Tdirection xStart,xEnd,yStart,yEnd;
	xStart = getXstart(face); xEnd = flipDir(xStart);
	yStart = getYstart(face); yEnd = flipDir(yStart);

	CSuperChunk* sc;
	for (size_t scNo=0;scNo<layers[layerNo].faceGroup[face].size();scNo++) { //for each face SC...
		sc = layers[layerNo].faceGroup[face][scNo];
		sc->addTwoIncomingLayers(face, xStart, yStart);
	} 
}



void CTerrain::freeChunk(Chunk & chunk) {
	if (chunk.id)
		multiBuf.deleteBlock(chunk.id);
	chunk.status = chFree;
	spareChunks.push_back(&chunk);
}

CTerrain::~CTerrain() {
	for (size_t c=0;c<spareChunks.size();c++) {
		delete spareChunks[c];
	}
	
	for (size_t l=0;l<layers.size();l++) {
		for (size_t s=0;s<layers[l].superChunks.size();s++) {
			delete layers[l].superChunks[s];
		}
	}
}





void CRenderTerrain::reserveBuf(unsigned int elementsUsed) {
	multiBuf.reserve(elementsUsed);

}

unsigned int CRenderTerrain::getBuffer() {
	return multiBuf.hBuffer;
}



CTerrainLayer::CTerrainLayer() {
	scrollState = i32vec3(0);
	shifted[0] = shifted[1] = shifted[2] = shifted[3] = shifted[4] = shifted[5] = false;
	resetState = i32vec3(0);
}

/** Advance the terrain of this layer in a given direction, scrolling as necessary. Returns true if a scroll
	occurs. */
bool CTerrainLayer::advance(i32vec3& scrollVec) {
	scrollState += scrollVec;

	nwLayerPos -= vec3(scrollVec * cubesPerChunkEdge) * LoD1cubeSize; //Move whole layer in response to this advance

	//If this layer has been advanced to its scrolling point, scroll it
	int scrollAxis = getAxis(scrollVec);
	int scrollPoint = 1 << (int)LoD-1;
	if ( (abs(scrollState.x) == scrollPoint) || (abs(scrollState.y) == scrollPoint) || (abs(scrollState.z) == scrollPoint) ) {
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
	vec3 sampleStep = vec3(scrollVec * cubesPerChunkEdge) * LoD1cubeSize * LoDscale;
	for (size_t s=0;s<superChunks.size();s++) { 
		for (int c=0;c<superChunks[s]->chunkList.size();c++) {
			superChunks[s]->chunkList[c]->scIndex += -scrollVec;
		}
		superChunks[s]->shrinkBoundary(scrollDir); //shrink boundary where we've mode chunks out
		superChunks[s]->extendBoundary(flipDir(scrollDir)); //extend boundary where we've moved chunks along
	}

	//we're going to scroll this layer, so return it to its initial position first.
	nwLayerPos += sampleStep; 

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
