#pragma once

#include <vector>
#include <unordered_map>

#include "../renderer/renderTexture.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"


struct TTexBlock {
	glm::i32vec2 start;
	glm::i32vec2 size;
};

/** Provides 2D blocks of graphics memory for storage. Attempts to recycle. */
class C2DimageBuf {
public:
	C2DimageBuf();
	void setSize(int width, int height);
	void clear();
	glm::i32vec2 reserve(glm::i32vec2 size);
	CRenderTexture& getBuffer();
	void free(glm::i32vec2& addr);

	void tmpSave() {
		imageBuffer.savePNG("d:\\buffer.png");

	}

	CRenderTexture imageBuffer;

private:
	int getFreeBlock(const glm::i32vec2& size);
	void splitHorizontal(unsigned int index, int height);
	void splitVertical(unsigned int index, int width);
	int panic();

	std::vector<TTexBlock> freeBlocks;
	//std::unordered_map<int,TTexBlock> reservedBlocks;
	std::unordered_map<glm::i32vec2, TTexBlock> reservedBlocks2;

	unsigned int nextId;

	int highestReservedAddress;
;
};






