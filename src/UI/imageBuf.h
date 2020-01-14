#pragma once

#include <vector>
#include <unordered_map>

#include "../renderer/renderTexture.h"


struct TTexBlock {
	int start;
	int size;
};

/** Provides 2D blocks of graphics memory for storage. Attempts to recycle. */
class C2DimageBuf {
public:
	C2DimageBuf();
	void setSize(int width, int height);
	void clear();
	unsigned int reserve(glm::i32vec2& size);
	CRenderTexture& getBuffer();
	void free(int addr);

	CRenderTexture imageBuffer;

private:
	int getFreeBlock(int size);
	void split(unsigned int index, int size);
	int panic();

	std::vector<TTexBlock> freeBlocks;
	std::unordered_map<int,TTexBlock> reservedBlocks;

	unsigned int nextId;

	int highestReservedAddress;
;
};






