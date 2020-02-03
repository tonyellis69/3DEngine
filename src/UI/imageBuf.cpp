#include "imageBuf.h"

#include <algorithm>

#include "../utils/log.h"

using glm::i32vec2;
using glm::all;
using glm::greaterThanEqual;

C2DimageBuf::C2DimageBuf() {
	nextId = 0;
	highestReservedAddress = 0;
}

/** Set the size of the main buffer. This will wipe any existing content.*/
void C2DimageBuf::setSize(int width, int height) {
	
	imageBuffer.resize(width, height  * 1.5f );
	clear();
}

void C2DimageBuf::clear() {
	nextId = 0;
	freeBlocks.clear();
	freeBlocks.push_back({ {0, 0},{imageBuffer.width, imageBuffer.height} });
	highestReservedAddress = 0;
	//reservedBlocks.clear();
	reservedBlocks2.clear();
}

/** Reserve a chunk of image memory, and return the identifying id. */
glm::i32vec2 C2DimageBuf::reserve( glm::i32vec2 size) {

	size.x = std::min(size.x, imageBuffer.width);

	int index = getFreeBlock(size);
	
	if (freeBlocks[index].size.y > size.y && freeBlocks[index].start.x == 0) //for now, only split full-width blocks
		splitHorizontal(index, size.y);	

	if (freeBlocks[index].size.x > size.x)
		 splitVertical(index, size.x);


	glm::i32vec2 addr = freeBlocks[index].start;
	//reservedBlocks[addr.y] = freeBlocks[index];
	reservedBlocks2[addr] = freeBlocks[index];
	highestReservedAddress = std::max(highestReservedAddress, addr.y + freeBlocks[index].size.y);
	freeBlocks.erase(freeBlocks.begin() + index);
	return addr;
}

CRenderTexture& C2DimageBuf::getBuffer() {
	return imageBuffer;
}

/** Free the block at this address for re-use.*/
void C2DimageBuf::free(glm::i32vec2& addr)  {
	//return;

	//TTexBlock block = reservedBlocks[addr.y];
	TTexBlock block = reservedBlocks2[addr];

	//reservedBlocks.erase(addr.y);
	reservedBlocks2.erase(addr);
	freeBlocks.push_back(block);
}

/** Return the index in freeBlocks of a free memory block >= size. */
int C2DimageBuf::getFreeBlock(const glm::i32vec2& size) {
	int index = 0;
	for (auto freeBlock : freeBlocks) {
		if (all( greaterThanEqual( freeBlock.size, size))) {
			break;
		}
		index++;
	}

	if (index == freeBlocks.size()) {
		if (imageBuffer.width == 780)
			int b = 0;
		index = panic();
	}
	return index;
}

/** Split this block into two horizontally, one of height height. */
void C2DimageBuf::splitHorizontal(unsigned int index, int height) {
	TTexBlock newFreeBlock = {  freeBlocks[index].start + i32vec2(0,height) , {imageBuffer.width, freeBlocks[index].size.y - height} };
	freeBlocks.push_back(newFreeBlock);
	freeBlocks[index].size.y = height;
}

/** Split this block into two vertically, one of width width. */
void C2DimageBuf::splitVertical(unsigned int index, int width) {
	//create a new block encompassing the unused space
	TTexBlock newFreeBlock = { freeBlocks[index].start + i32vec2(width,0)   , {freeBlocks[index].size.x - width, freeBlocks[index].size.y} };
	freeBlocks.push_back(newFreeBlock);
	freeBlocks[index].size.x = width;
}

/** Buffer has been found to be too small, so resize it.*/
int C2DimageBuf::panic() {
	int oldSize = imageBuffer.height;
	imageBuffer.resizeSafe(imageBuffer.width, imageBuffer.height * 1.25f);
	int newSize = imageBuffer.height;

	//is there a free block at the end of the old memory?
	for (auto& freeBlock : freeBlocks) {
		if (freeBlock.start.x == 0 && freeBlock.start.y + freeBlock.size.y >= highestReservedAddress) {
			freeBlock.size.y += newSize - oldSize;
			return freeBlocks.size() - 1;
		}
	}

	//still here? then the highest block is a reserved block,
	//and we can create a new free block right after it
	TTexBlock newBlock = { {0, highestReservedAddress + 1}, {imageBuffer.width, newSize - oldSize} };
	freeBlocks.push_back(newBlock);
	return freeBlocks.size() - 1;
}


