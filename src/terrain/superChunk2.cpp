#include "superChunk2.h"

#include <iostream>

#include "..\utils\log.h"

using namespace glm;

ITerrainCallback* CSuperChunk2::pCallbackApp = NULL;

void CSuperChunk2::checkForIntersection() {
	isEmpty = pCallbackApp->scIntersectionCheckCallback(sampleSpacePos, sampleSize);
}

void CSuperChunk2::setSampleSpacePosition(glm::vec3 & pos) {
	sampleSpacePos = pos;
}

void CSuperChunk2::setSampleSize(float sampleSize) {
	this->sampleSize = sampleSize;
	chunkSampleSize = sampleSize / SCchunks;
}

void CSuperChunk2::setCallbackApp(ITerrainCallback * pApp) {
	pCallbackApp = pApp;
}

/** For each chunk volume within this SC's chunk clipping limits (if any), create a chunk 
	if the volume is intersected by terain. */
void CSuperChunk2::createAllChunks() {
	glm::vec3 chunkSampleSpacePos;
	for (int x = nwChunkStart.x; x <= seChunkEnd.x; x++) {
		for (int y = nwChunkStart.y; y <= seChunkEnd.y; y++) {
			for (int z = nwChunkStart.z; z <= seChunkEnd.z; z++) {
				chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
				if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
					createChunk(glm::i32vec3(x, y, z));
				}

			}
		}
	}


}

/** Acquire a new chunk at the given index position. */
void CSuperChunk2::createChunk(glm::i32vec3 & index) {
	chunks.push_back(index);
}


////////////////////////////////////////
//TO DO: oops, may not need this. Why not just use creatAllChunks() and simply have a rule
//that if there's already a chunk at a given index point we pass on?
//However - there is a slight cost in looking up each chunk index in the chunks vector
//-that's going to add up. 
// When I've got everything working, see if this is an optimisation.


/** This SC's chunk clipping limits have been extended by two layers. Iterating within that space,
	create a chunk where terrain intersects a chunk volume. */
void CSuperChunk2::addFaceChunks(Tdirection face) {
	glm::i32vec3 localnwChunkStart = nwChunkStart;
	glm::i32vec3 localseChunkEnd = seChunkEnd;
	int axis = getAxis(face);

	if (face == north || face == west || face == down) {
		localseChunkEnd = seChunkEnd;
		localseChunkEnd[axis] = nwChunkStart[axis] + 1;
	}
	else {
		localnwChunkStart = nwChunkStart;
		localnwChunkStart[axis]  = seChunkEnd[axis] - 1;
	}


	glm::vec3 chunkSampleSpacePos;
	for (int x = localnwChunkStart.x; x <= localseChunkEnd.x; x++) {
		for (int y = localnwChunkStart.y; y <= localseChunkEnd.y; y++) {
			for (int z = localnwChunkStart.z; z <= localseChunkEnd.z; z++) {
				chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
				if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
					createChunk(glm::i32vec3(x, y, z));
				}

			}
		}
	}


}

/** Remove all th chunks of this superchunk.*/
void CSuperChunk2::clearChunks() {
	//TO DO: when we have actual chunks, recycle them.
	chunks.clear();
	isEmpty = true;
}

/** Remove any chunks within the proportion of the SC volume defined by this unit volutme. */
void CSuperChunk2::clearChunks(CBoxVolume& unitVolume) {
	i32vec3 indexBL = unitVolume.bl *= vec3(SCchunks);
	i32vec3 indexTR = unitVolume.tr *= vec3(SCchunks);
	for (auto chunk = chunks.begin(); chunk != chunks.end();) {
		if (all(greaterThanEqual(*chunk, indexBL)) && all(lessThanEqual(*chunk, indexTR))) {
			chunk = chunks.erase(chunk);
		}
		else
			++chunk;
	}
}

/** Add any chunks that fall within the proportion of the SC defined by this unit volume. */
void CSuperChunk2::addChunks(CBoxVolume& unitVolume) {
	i32vec3 indexBL = unitVolume.bl *= vec3(SCchunks);
	i32vec3 indexTR = unitVolume.tr *= vec3(SCchunks);
	vec3 chunkSampleSpacePos;
	for (int x=0; x < SCchunks; x++)
		for (int y = 0; y < SCchunks; y++)
			for (int z = 0; z < SCchunks; z++) {
				i32vec3 chunk(x,y,z);
				if (std::find(chunks.begin(), chunks.end(), chunk) == chunks.end())
					if (all(greaterThanEqual(chunk, indexBL)) && all(lessThanEqual(chunk, indexTR))) {
						chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
						if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
							createChunk(glm::i32vec3(x, y, z));
							chunks.push_back(chunk);
						}
					}
			}

}

/** Add any chunks that fall outise the proportion of the SC defined by this unit volume. */
void CSuperChunk2::addChunksOutside(CBoxVolume& unitVolume) {
	i32vec3 indexBL = unitVolume.bl *= vec3(SCchunks);
	i32vec3 indexTR = unitVolume.tr *= vec3(SCchunks);
	vec3 chunkSampleSpacePos;
	for (int x = 0; x < SCchunks; x++)
		for (int y = 0; y < SCchunks; y++)
			for (int z = 0; z < SCchunks; z++) {
				i32vec3 chunk(x, y, z);
				if (std::find(chunks.begin(), chunks.end(), chunk) == chunks.end())
					if (any(lessThan(chunk, indexBL)) || any(greaterThan(chunk, indexTR))
					//	|| any (equal(indexBL,indexTR)) 
						) {
						chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
						if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
							createChunk(glm::i32vec3(x, y, z));
							chunks.push_back(chunk);
							isEmpty = false;
						}
					}
			}
}

/** Remove any chunks withing the given volume of chunkspace. */
void CSuperChunk2::clearOverlappedChunks(TBoxVolume & chunkClippingVolume) {
	//return;
	for (auto chunk = chunks.begin(); chunk != chunks.end();) {
		if (chunk->x >= chunkClippingVolume.bl.x && chunk->y >= chunkClippingVolume.bl.y && chunk->z >= chunkClippingVolume.bl.z
			&& chunk->x <= chunkClippingVolume.tr.x && chunk->y <= chunkClippingVolume.tr.y && chunk->z <= chunkClippingVolume.tr.z) {
			chunk = chunks.erase(chunk);
		}
		else
			++chunk;
	}
}

/** Remove any chunks beyond the overlap boundary in this direction. */
void CSuperChunk2::clearScrolledOutChunks(Tdirection face, int overlap) {
	int axis = getAxis(face);
	for (auto chunk = chunks.begin(); chunk != chunks.end();) {
		if (face == north || face == west || face == down) {
			if ((*chunk)[axis] < SCchunks - overlap)
				chunk = chunks.erase(chunk);
			else
				++chunk;
		}
		else {
			if ((*chunk)[axis] >= overlap)
				chunk = chunks.erase(chunk);
			else
				++chunk;

		}
	}
}

/** Add new chunks where required withing the clipping volume. This is used
	to add chunks on the inner face of a shell, where they've previously been
	overlapped by the chunks of the inner shell. */
void CSuperChunk2::restoreClippedChunks(TBoxVolume& chunkVolume) {
	glm::vec3 chunkSampleSpacePos;
	for (int x = 0; x < SCchunks; x++) 
		for (int y = 0; y < SCchunks; y++) 
			for (int z = 0; z < SCchunks; z++) 
				if ( glm::any(lessThan(glm::i32vec3(x,y,z), chunkVolume.bl)) || 
					glm::any(greaterThan(glm::i32vec3(x, y, z), chunkVolume.tr))) {
					auto found = find(chunks.begin(), chunks.end(), glm::i32vec3(x, y, z));
					if (found == chunks.end()) {
						chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
						if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
							createChunk(glm::i32vec3(x, y, z));
						}
					}
				}


/*
	for (int x = chunkVolume.bl.x; x <= chunkVolume.tr.x; x++) {
		for (int y = chunkVolume.bl.y; y <= chunkVolume.tr.y; y++) {
			for (int z = chunkVolume.bl.z; z <= chunkVolume.tr.z; z++) {
				chunkSampleSpacePos = sampleSpacePos + glm::vec3(x, y, z) * chunkSampleSize;
				if (pCallbackApp->chunkCheckCallback(chunkSampleSpacePos, chunkSampleSize)) {
					createChunk(glm::i32vec3(x, y, z));
				}
			}
		}
	}
*/

}

/** If clippee is partially or entirely inside this volume, return true
	and with the proportion of overlap in clipVol, expressed as a unit volume. */
bool CBoxVolume::isClippedBy(CBoxVolume& clipVol){
	vec3 SCboundaryFix(1.0f);
	if (any(lessThan(clipVol.tr, bl + SCboundaryFix)) || any(greaterThan(clipVol.bl, tr - SCboundaryFix)))
		return false;

	vec3 volSize(clipVol.tr - clipVol.bl);

	vec3 boundaryFix(1);
	vec3 unitBl = max(clipVol.bl, bl) - clipVol.bl + boundaryFix;
	vec3 unitTr = min(clipVol.tr, tr) - clipVol.bl - boundaryFix;

	unitBl = unitBl / volSize;
	unitTr = unitTr / volSize;
	clipVol.set(unitBl, unitTr);
	return true;
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
