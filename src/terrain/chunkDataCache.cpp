#include "chunkDataCache.h"

#include "utils/log.h"

/** Return the chunk data at this multibuf address. If it's not already in the 
	cache, export it to the cache from the multibuf. */
TChunkTriBuf2* ChunkDataCache::getData(int chunkAddr) {
	timeStamp++;
	auto chunkData = chunkCache.find(chunkAddr);
	if (chunkData == chunkCache.end()) {
		//we need to copy this data to our cache

		sysLog << "\nChunk data not found in cache.";

		TChunkTriBuf2* buf = getFreeDataBuf();

		int size = multiBuf->exportBlock(chunkAddr, (char*)buf->buf);
		buf->noTris = (size / sizeof(TChunkVert2)) / 3.0f;
		buf->timeStamp = timeStamp;
		chunkCache[chunkAddr] = buf;
		return buf;
	}

	chunkData->second->timeStamp = timeStamp;
	return chunkData->second;
}

/** Return the address of a free buffer in the cache. If all buffers are
	full, return the least recently used one. */
TChunkTriBuf2* ChunkDataCache::getFreeDataBuf() {
	if (nextFreeBuf < NUM_DATABUFS)
		return &dataBufs[nextFreeBuf++];

	sysLog << "\nRecycling buffers.";

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
