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

  CSuperChunk*  dbgSC = NULL;

 int totalbufsize = 0;
 int totalchunks = 1;

CTerrain::CTerrain() : CModelMulti() {
	totalTris = 0;
	chunkOrigin = glm::translate(glm::mat4(1), getPos());
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
	int baseLayerSampleSize = (int)(superChunksPerTerrainEdge * cubesPerSCedge * currentLoDscale );
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

		nwLayerSamplePos += vec3(outerLayerThickness * cubesPerChunkEdge * outerLoDScale * chunksPerSChunkEdge);

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
			layers[l].superChunks[s]->createChunks();
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

/** Initialise the given 3d array of superchunks with the correct position, size, lod, etc. */
void CTerrain::initSuperChunks(T3dArray &scArray, int layerNo, vec3 nwLayerSamplePos, i32vec3& _sizeInChunks) {
	//float thisSuperChunkSize = layers[layerNo].cubeSize * cubesPerChunkEdge  *chunksPerSChunkEdge ;
	vec3 thisSuperChunkSize(_sizeInChunks);
	thisSuperChunkSize *= layers[layerNo].cubeSize * cubesPerChunkEdge ;
	float LoDscale = float(1 << (layers[layerNo].LoD-1));
	int cubesPerSCedge =  chunksPerSChunkEdge * cubesPerChunkEdge;
	float layerSCsampleRange = cubesPerSCedge * LoDscale; 

	for (size_t x=0; x<scArray.size(); ++x) {
		for (size_t y=0; y<scArray[0].size(); ++y) {
			for (size_t z=0;z<scArray[0][0].size(); ++z) {
				CSuperChunk* sChunk = scArray[x][y][z];
				sChunk->nwWorldPos = layers[layerNo].nwLayerPos + vec3(x,y,z) * thisSuperChunkSize;
				sChunk->setSizes(_sizeInChunks,cubesPerChunkEdge,layers[layerNo].cubeSize);		
				sChunk->LoD = layers[layerNo].LoD;
				sChunk->LoDscale = LoDscale;
				sChunk->setSamplePos(nwLayerSamplePos + (vec3(x,y,z) * layerSCsampleRange));
				sChunk->tmpIndex = i32vec3(x,y,z);
				sChunk->genColour = vec4(col::randHue(),1);
				if (sChunk->tmpIndex == i32vec3(0, 2, 0) && sChunk->LoD == 1)
					dbgSC = sChunk;
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
	chunk->live = false;
	return chunk;

}

/** Return the given chunk to the spareChunks list. */
void CTerrain::freeChunk(Chunk* chunk) {
	chunk->live = false;
	freeChunkModel(chunk);
	spareChunks.push_back(chunk);
}


void CTerrain::update() {
	//return;
		//watch::watch1 << "total tris: " << totalTris;
	watch::watch1 << "tot buf " << totalbufsize;
	watch::watch1 << " cnks " << totalchunks;
	watch::watch1 << " avg " << (totalbufsize/totalchunks);


		CSuperChunk *sc = layers[0].faceGroup[0][0];
	
	//Skin the next chunk on the list that needs it, if any. */
	double startT = watch::pTimer->milliseconds();

	size_t s = toSkin.size();
	while ((watch::pTimer->milliseconds() - startT) < 75) {
		if (s == 0)
			return;
		Chunk* chunk = toSkin.back().chunk;
		CSuperChunk* parentSC = toSkin.back().parentSC;
		toSkin.pop_back(); //remove from list
		//pass chunk to shade chunk func. Generates model and registers it.
		createChunkMesh(*chunk);

		//check if this chunk replaces another
		if (parentSC) {
			parentSC->chunksToSkin--;
			if (parentSC->chunksToSkin == 0 && parentSC->overlapDir != none) { //all chunks skinned for this SC
				Tdirection overlapDir = parentSC->overlapDir;
				Tdirection faceDir = flipDir(overlapDir);
				CSuperChunk* overlappedSC = parentSC->adj(overlapDir);
				if (overlappedSC->LoD < chunk->LoD) { //if this new L2 chunk overlaps a cluster of smaller, L1 SCs...
					overlappedSC->removeOutscrolledChunks(faceDir);
					overlappedSC->adj(getXdir(overlapDir))->removeOutscrolledChunks(faceDir); //////
					overlappedSC->adj(getYdir(overlapDir))->removeOutscrolledChunks(faceDir);
					overlappedSC->adj(getYdir(overlapDir))->adj(getXdir(overlapDir))->removeOutscrolledChunks(faceDir);
					parentSC->overlapDir = none;
				}
				else { //if L1 chunk partially overlaps an L2 SC
					overlappedSC->overlapCount--;
					if (overlappedSC->overlapCount == 0) {
						overlappedSC->removeFace(faceDir);
					}
					parentSC->overlapDir = none;
				}

			}
		}


		chunk->live = true;
		s--;
	}
}


/** Advance the terrain in the given direction. */
void CTerrain::advance(Tdirection dir) {
	if (dir == none) //TO DO: should not happen!
		return;
	//find the axis along which we're scrolling 0=x,1=y,z=2, and the direction, 1 or -1.
	int scrollAxis = getAxis(dir);
	int scrollDir = (int)dirToVec(dir)[scrollAxis];//-1 when scrolling-in from the north
	i32vec3 scrollVec(0,0,0);
	scrollVec[scrollAxis] = scrollDir;



	chunkOrigin = glm::translate(chunkOrigin, vec3(-scrollVec) * (float)cubesPerChunkEdge * LoD1cubeSize);


	size_t inner = layers.size()-1;
	//for (int l=inner;l>=0;l--) {
	for (int l=0;l<=inner;l++) {  //from outer layer to inner
			Tdirection outgoingDir = flipDir(dir);

			//advance this layer
			bool scrolled = layers[l].advance(scrollVec); 
			//this will move all chunks along by the size of LoD1, and may trigger the layer's scroll action.
		
			if (scrolled) {
				if (!layers[l].shifted[outgoingDir]) {
					if (!layers[l].shifted[dir]) {
						layers[l].shifted[outgoingDir] = true;
					}
					else
						layers[l].shifted[dir] = false;
				}
				else {
					layers[l].shifted[outgoingDir] = false;
					if (l > 0) {
						addTwoIncomingLayers(l,dir);  
					}
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
		sc->extendBoundary(face);sc->extendBoundary(face); //brings it down from 2 to 0
		sc->sizeInChunks[zAxis] += 1;
		sc->chunksToSkin = 0;
	
		int zStart = sc->outerLayer(face); int zEnd = zStart+1;
		if ((face == south) || (face == east)||(face == up)) {
			zEnd = sc->outerLayer(face); zStart = zEnd-1;
		}
	
		for ( facePos.z =zStart;facePos.z <= zEnd;facePos.z++) {
			//for each potential chunk space
			for (facePos.x=sc->outerLayer(xStart);facePos.x <= sc->outerLayer(xEnd);facePos.x++) {
				for (facePos.y=sc->outerLayer(yStart);facePos.y <= sc->outerLayer(yEnd);facePos.y++) {
					i32vec3 realPos = rotateByDir(facePos,face);
					vec3 samplePos = sc->nwSamplePos + (vec3(realPos) * (float)cubesPerChunkEdge * sc->LoDscale);
					if (sc->createChunkAsRequired(realPos,samplePos,sc))
						sc->chunksToSkin++;
				}
			}
		}
		if (sc->chunkList.size() > 0) {
			sc->overlapDir = face;
			sc->adj(face)->overlapCount++;

		}

	}
}



CTerrain::~CTerrain() {
	for (size_t c=0;c<spareChunks.size();c++) {
		
		freeChunkModel((CModel*)spareChunks[c]);
		delete spareChunks[c];
	}
	
	

	for (size_t l=0;l<layers.size();l++) {
		for (size_t s=0;s<layers[l].superChunks.size();s++) {
			delete layers[l].superChunks[s];
		}

	}
}


/** Create a multidraw buffer of the requested size. */
void CRenderTerrain::setMultiBufferSize(unsigned int bufSize,unsigned int noObjects) {
	pRenderer->storeVertexData(multiBuf.hBuffer, 0, bufSize);
	multiBuf.setMultiBufferSize(bufSize, noObjects);
}



CTerrainLayer::CTerrainLayer() {
	scrollState = i32vec3(0);
	shifted[0] = shifted[1] = shifted[2] = shifted[3] = shifted[4] = shifted[5] = false;
}

/** Advance the terrain of this layer in a given direction, scrolling as necessary. Returns true if a scroll
	occurs. */
bool CTerrainLayer::advance(i32vec3& scrollVec) {
	scrollState += scrollVec;

	//jump all chunks along one chunk relative to terrain, to compensate for terrain being jumped back to
	//starting position elsewhere
	vec3 step = vec3(scrollVec * cubesPerChunkEdge) * LoD1cubeSize;
	for (size_t s=0;s<superChunks.size();s++) {
		superChunks[s]->nwWorldPos -= step;
	}
	//superChunks[0]->terrain->chunkOrigin = glm::translate(superChunks[0]->terrain->chunkOrigin, vec3(scrollVec) * cubesPerChunkEdge * LoD1cubeSize);
	//TO DO: evil hack to get at terrain! Fix!

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
	vec3 step = vec3(scrollVec * cubesPerChunkEdge) * LoD1cubeSize * LoDscale;
	for (size_t s=0;s<superChunks.size();s++) { 
		for (int c=0;c<superChunks[s]->chunkList.size();c++) {
			superChunks[s]->chunkList[c]->scIndex += -scrollVec;
		}


		superChunks[s]->shrinkBoundary(scrollDir); //shrink boundary where we've mode chunks out
		superChunks[s]->extendBoundary(flipDir(scrollDir)); //extend boundary where we've moved chunks along
		
		superChunks[s]->nwWorldPos += step;

		
	}

	for (size_t s=0;s<superChunks.size();s++) {
			
		if (superChunks[s] == dbgSC)
			int b = 0;
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
