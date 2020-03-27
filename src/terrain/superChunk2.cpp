#include "superChunk2.h"
#include "terrain2.h"

#include <iostream>

#include "..\utils\log.h"

#include "..\colour.h"

using namespace glm;

//ITerrainAppCallback* CSuperChunk2::pCallbackApp = NULL;
CTerrain2* CSuperChunk2::pTerrain;

void CSuperChunk2::setBasics(int shellNo, glm::i32vec3& index, int numSCchunks) {
	isEmpty = true;
	colour = vec4(col::randHue(), 1);
	origIndex = index;
	this->numSCchunks = numSCchunks;
	pTerrain = (CTerrain2*) pTerrainObj; //scrap!!!!!!!
	this->shellNo = shellNo;
}

void CSuperChunk2::setSampleInfo(float sampSize, glm::vec3& samplePos) {
	setSampleSize(sampSize);
	setSampleSpacePosition(samplePos);
}

void CSuperChunk2::checkForTerrainIntersection() {
	isEmpty = pCallbackApp->scIntersectionCheckCallback(sampleSpacePos, sampleSize);
}

void CSuperChunk2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

void CSuperChunk2::setSampleSize(float sampleSize) {
	this->sampleSize = sampleSize;
	chunkSampleSize = sampleSize / numSCchunks;
}



void CSuperChunk2::setTerrainCallback(ITerrain* pTerrain) {
	pTerrainObj = pTerrain;
}



/** Acquire a new chunk at the given index position. */
void CSuperChunk2::createChunk(glm::i32vec3 & index) {

	if (shellNo > 1)
		return;
	vec3 sampleCorner = sampleSpacePos + vec3(index) * chunkSampleSize;

	vec3 terrainPos = pTerrainObj->getSCworldPos(shellNo, origIndex); 

	terrainPos -= vec3(pTerrain->chunkOrigin[3]);

	terrainPos += vec3(index) * pTerrain->shells[shellNo].chunkSize;
	 
	//scChunks.push_back( pTerrain->createChunk(index, sampleCorner, shellNo, terrainPos) );
	scChunks.push_back(pTerrainObj->createChunk(index, sampleCorner, shellNo, terrainPos));


	//pTerrain->chunks[scChunks.back()].colour = colour;
	//pTerrain->chunks[scChunks.back()].colour = pTerrain->shells[shellNo].shellColour;
	pTerrain->chunks[scChunks.back()].colour = colour;


}	


/** Remove all th chunks of this superchunk.*/
void CSuperChunk2::clearChunks() {
	for (auto id : scChunks)
		pTerrainObj->removeChunk(id);
		
	scChunks.clear();
	isEmpty = true;
}

/** Remove any chunks within the proportion of the SC volume defined by this unit volutme. */
void CSuperChunk2::clearChunks(CBoxVolume& unitVolume) {
	i32vec3 indexBL = unitVolume.bl *= vec3(numSCchunks);
	i32vec3 indexTR = unitVolume.tr *= vec3(numSCchunks);

	for (auto id = scChunks.begin(); id != scChunks.end();) {
		i32vec3* chunkIndex = &pTerrain->chunks[*id].index;
		if (all(greaterThanEqual(*chunkIndex, indexBL)) && all(lessThanEqual(*chunkIndex, indexTR))) {
			pTerrainObj->removeChunk(*id);
			id = scChunks.erase(id);
		}
		else
			++id;
	}
}

/** Add any chunks that fall within the proportion of the SC defined by this unit volume. */
void CSuperChunk2::addChunks(CBoxVolume& unitVolume) {
	i32vec3 indexBL = unitVolume.bl *= vec3(numSCchunks);
	i32vec3 indexTR = unitVolume.tr *= vec3(numSCchunks);
	vec3 chunkSampleSpacePos;
	for (int x = 0; x < numSCchunks; x++)
		for (int y = 0; y < numSCchunks; y++)
			for (int z = 0; z < numSCchunks; z++) {
				i32vec3 chunk(x, y, z);
				if (all(greaterThanEqual(chunk, indexBL)) && all(lessThanEqual(chunk, indexTR))) {
					if (chunkExists(chunk))
						continue;

					chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
					if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
						createChunk(chunk);
					}
				}
			}
}

/** Returns true if this SC has a chunk at this index position. */
bool CSuperChunk2::chunkExists(glm::i32vec3& index) {
	for (auto localChunk : scChunks) {
		if (pTerrain->chunks[localChunk].index == index) {
			return true;
		}
	}
	return false;
}

/** Add any chunks that fall outside the proportion of the SC defined by this unit volume. */
void CSuperChunk2::addChunksOutside(CBoxVolume& unitVolume) {
	i32vec3 indexBL = unitVolume.bl *= vec3(numSCchunks);
	i32vec3 indexTR = unitVolume.tr *= vec3(numSCchunks);
	vec3 chunkSampleSpacePos;
	for (int x = 0; x < numSCchunks; x++)
		for (int y = 0; y < numSCchunks; y++)
			for (int z = 0; z < numSCchunks; z++) {
				i32vec3 chunk(x, y, z);
				if (any(lessThan(chunk, indexBL)) || any(greaterThan(chunk, indexTR))) {
					if (chunkExists(chunk))
						continue;

					chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
					if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
						createChunk(chunk);
						isEmpty = false;
					}
				}
			}
}

/** Add chunks that fall within the proportion of the SC defined by outerUnitVoluCGlobalFuncDeclNodeme that do not also 
	fall within innerUnitVolume. */
void CSuperChunk2::addChunksBetween(CBoxVolume& outerUnitVolume, CBoxVolume& innerUnitVolume) {
	i32vec3 outerBL = outerUnitVolume.bl *= vec3(numSCchunks);
	i32vec3 outerTR = outerUnitVolume.tr *= vec3(numSCchunks);
	i32vec3 innerBL = innerUnitVolume.bl *= vec3(numSCchunks);
	i32vec3 innerTR = innerUnitVolume.tr *= vec3(numSCchunks);
	vec3 chunkSampleSpacePos;
	for (int x = 0; x < numSCchunks; x++)
		for (int y = 0; y < numSCchunks; y++)
			for (int z = 0; z < numSCchunks; z++) {	
				i32vec3 chunk(x, y, z);
				//does this chunk fall within the inner volume? Then we can bail
				if (all(greaterThanEqual(chunk, innerBL)) && all(lessThanEqual(chunk, innerTR)))
					continue;

				if (all(greaterThanEqual(chunk, outerBL)) && all(lessThanEqual(chunk, outerTR))) {
					if (chunkExists(chunk))
						continue;

					chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
					if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
						createChunk(chunk);
					}
				}
			}
}



/** Remove any chunks in the outermost layers of this face. */
void CSuperChunk2::clearOuterLayerChunks(Tdirection face, int layersToKeep) {
	int axis = getAxis(face);
	int limitA = layersToKeep; int limitB = numSCchunks;
	if (face == north || face == west || face == down) {
		limitA = 0; limitB = numSCchunks - layersToKeep;
	}

	for (auto id = scChunks.begin(); id != scChunks.end();) {
		i32vec3 chunkIndexPos = pTerrainObj->getChunkIndex(*id);
		if (chunkIndexPos[axis] >= limitA && chunkIndexPos[axis] < limitB) {
			pTerrainObj->removeChunk(*id);
			id = scChunks.erase(id);
		}
		else
			++id;
	}
}


/** If clippee is partially or entirely inside this volume, return true
	and with the proportion of overlap in clipVol, expressed as a unit volume. */
std::tuple<bool, CBoxVolume> CBoxVolume::findOverlap(CBoxVolume clipVol)
{
	vec3 SCboundaryFix(1.0f);
	if (any(lessThan(clipVol.tr, bl + SCboundaryFix)) || any(greaterThan(clipVol.bl, tr - SCboundaryFix)))
		return { false,clipVol };

	vec3 volSize(clipVol.tr - clipVol.bl);

	vec3 boundaryFix(1);
	vec3 unitBl = max(clipVol.bl, bl) - clipVol.bl + boundaryFix;
	vec3 unitTr = min(clipVol.tr, tr) - clipVol.bl - boundaryFix;

	unitBl = unitBl / volSize;
	unitTr = unitTr / volSize;
	clipVol.set(unitBl, unitTr);
	return { true, clipVol };
}



/** If clippee is partially or entirely outside this volume, return true
	and with the proportion of overlap in clipVol, expressed as a unit volume. */
bool CBoxVolume::doesNotEntirelyEnvelop(CBoxVolume& clipVol) {
	vec3 SCboundaryFix(1.0f);
	if (all(lessThan(clipVol.tr, tr + SCboundaryFix)) && all(greaterThan(clipVol.bl, bl - SCboundaryFix)))
		return false;

	vec3 volSize(clipVol.tr - clipVol.bl);

	//find overlap
	vec3 boundaryFix(1);
	vec3 unitBl = max(clipVol.bl, bl);
	vec3 unitTr = min(clipVol.tr, tr);

	unitBl = unitBl - clipVol.bl;
	unitTr = unitTr - clipVol.bl;

	for (int axis = 0; axis < 3; axis++)
		if (unitBl[axis] == unitTr[axis]) {
			unitBl[axis] = volSize[axis];
			unitTr[axis] = 0;
		}


	unitBl = unitBl + boundaryFix;
	unitTr = unitTr - boundaryFix;

	unitBl = unitBl / volSize;
	unitTr = unitTr / volSize;
	clipVol.set(unitBl, unitTr);
	return true;
}
//TO DO: make above redundant

std::tuple<bool, CBoxVolume> CBoxVolume::findNotOrPartiallyOverlapped(CBoxVolume clipVol) {
	vec3 SCboundaryFix(1.0f);
	if (all(lessThan(clipVol.tr, tr + SCboundaryFix)) && all(greaterThan(clipVol.bl, bl - SCboundaryFix)))
		return { false, clipVol };

	vec3 volSize(clipVol.tr - clipVol.bl);

	//find overlap
	vec3 boundaryFix(1);
	vec3 unitBl = max(clipVol.bl, bl);
	vec3 unitTr = min(clipVol.tr, tr);

	unitBl = unitBl - clipVol.bl;
	unitTr = unitTr - clipVol.bl;

	for (int axis = 0; axis < 3; axis++)
		if (unitBl[axis] == unitTr[axis]) {
			unitBl[axis] = volSize[axis];
			unitTr[axis] = 0;
		}


	unitBl = unitBl + boundaryFix;
	unitTr = unitTr - boundaryFix;

	unitBl = unitBl / volSize;
	unitTr = unitTr / volSize;
	clipVol.set(unitBl, unitTr);
	return { true, clipVol };
}