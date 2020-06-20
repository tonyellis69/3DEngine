#pragma once

#include <unordered_map>

#include "..\vertBufs.h"

#include "..\renderer\multiBuf2.h"

#define NUM_DATABUFS  40
#define ARBITRARY_MAX_VERTS 4200

typedef vBuf::T3DnormVert TChunkVert2;
typedef unsigned long long timeStamp_t;

class TChunkTriBuf2 {
public:
	TChunkTriBuf2() { id = noTris = 0; }
	unsigned int id;
	unsigned int noTris;
	TChunkVert2 buf[ARBITRARY_MAX_VERTS];
	timeStamp_t timeStamp;
};

/** Maintains a cache of recently requested chunk triangle data. */
class ChunkDataCache {
public:
	ChunkDataCache() {
		nextFreeBuf = 0; timeStamp = 0;
	}
	TChunkTriBuf2* getData(int addr);
	void setMultiBuf(CMultiBuf2* multiBuf) {
		this->multiBuf = multiBuf;
	}

private:
	TChunkTriBuf2* getFreeDataBuf();
	void incrementTimeStamp();

	std::unordered_map <int, TChunkTriBuf2*> chunkCache;
	CMultiBuf2* multiBuf;

	TChunkTriBuf2 dataBufs[NUM_DATABUFS];

	int nextFreeBuf;
	timeStamp_t timeStamp;
};