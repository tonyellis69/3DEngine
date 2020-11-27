#include "chunkDataCache.h"

#include "utils/log.h"

/** Return the chunk data at this multibuf address. If it's not already in the 
	cache, export it to the cache from the multibuf. */
TChunkTriBuf2* ChunkDataCache::getData(int chunkaddr) {
	timeStamp++;
	auto chunkData = chunkCache.find(chunkaddr);
	if (chunkData == chunkCache.end()) {
		//we need to copy this data to our cache

		TChunkTriBuf2* buf = getFreeDataBuf();

		int size = multiBuf->exportBlock(chunkaddr, (char*)buf->buf);
		buf->noTris = (size / sizeof(TChunkVert2)) / 3.0f;
		buf->timeStamp = timeStamp;
		chunkCache[chunkaddr] = buf;
		buf->id = chunkaddr;
		return buf;
	}

	chunkData->second->timeStamp = timeStamp;
	return chunkData->second;
}

/** Remove this chunk data, if we have it, freeing the buffer for re-use. */
void ChunkDataCache::freeBuf(int addr) {
	auto chunkData = chunkCache.find(addr);
	if (chunkData != chunkCache.end()) {
		chunkData->second->timeStamp = 0;
		chunkCache.erase(chunkData);
	}
}

/** Return the address of a free buffer in the cache. If all buffers are
	full, return the least recently used one. */
TChunkTriBuf2* ChunkDataCache::getFreeDataBuf() {
	if (nextFreeBuf < NUM_DATABUFS)
		return &dataBufs[nextFreeBuf++];


	//find the least recently used buf
	timeStamp_t lowestTimeStamp = timeStamp;
	int redundantBuf;
	for (int buf = 0; buf < NUM_DATABUFS; buf++) {
		if (dataBufs[buf].timeStamp < lowestTimeStamp) {
			lowestTimeStamp = dataBufs[buf].timeStamp;
			redundantBuf = buf;
		}
	}

	return &dataBufs[redundantBuf];
}

/** Clunkily avoid disaster if the timestamp rolls over, at the cost
	of losing the cache.*/
//TO DO: see if this check is slower than just using long long for timestamps
//seems to make no difference, so stick with long long for now
void ChunkDataCache::incrementTimeStamp() {
	timeStamp++;
	if (timeStamp == 0) {
		nextFreeBuf = 0;
	}

}
