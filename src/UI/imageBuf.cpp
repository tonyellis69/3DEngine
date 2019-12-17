#include "imageBuf.h"

#include <algorithm>

#include "../utils/log.h"

C2DimageBuf::C2DimageBuf() {
	nextId = 0;
	highestReservedAddress = 0;
}

/** Set the size of the main buffer. This will wipe any existing content.*/
void C2DimageBuf::setSize(int width, int height) {
	
	imageBuffer.resize(width, height  *2 );
	nextId = 0;
	freeBlocks.clear();
	freeBlocks.push_back({ 0,imageBuffer.height });
	highestReservedAddress = 0;
	reservedBlocks.clear();
}

/** Reserve a chunk of image memory, and return the identifying id. */
unsigned int C2DimageBuf::reserve(glm::i32vec2& size) {
	int index = getFreeBlock(size.y);
	
	if (freeBlocks[index].size > size.y )
		split(index, size.y);
	
	int addr = freeBlocks[index].start;
	reservedBlocks[addr] = freeBlocks[index];
	highestReservedAddress = std::max(highestReservedAddress, addr + freeBlocks[index].size-1);
	freeBlocks.erase(freeBlocks.begin() + index);
	
	return addr;
}

CRenderTexture& C2DimageBuf::getBuffer() {
	return imageBuffer;
}

/** Free the block at this address for re-use.*/
void C2DimageBuf::free(int addr)  {
	TTexBlock block = reservedBlocks[addr];
	freeBlocks.push_back(block);
}

/** Return the index in freeBlocks of a free memory block >= size. */
int C2DimageBuf::getFreeBlock(int size) {
	int index = 0;
	for (auto freeBlock : freeBlocks) {
		if (freeBlock.size >= size) {
			break;
		}
		index++;
	}

	if (index == freeBlocks.size()) {
		index = panic();
	}
	return index;
}

/** Split this block into two, one of size size. */
void C2DimageBuf::split(unsigned int index, int size) {
	TTexBlock newBlock = { freeBlocks[index].start + size , freeBlocks[index].size - size };
	freeBlocks.push_back(newBlock);
	freeBlocks[index].size = size;
}

/** Buffer has been found to be too small, so resize it.*/
int C2DimageBuf::panic() {

	int oldSize = imageBuffer.height;
	imageBuffer.resizeSafe(imageBuffer.width, imageBuffer.height * 1.25f);
	int newSize = imageBuffer.height;

	//is there a free block at the end of the old memory?
	for (auto& freeBlock : freeBlocks) {
		if (freeBlock.start + freeBlock.size >= highestReservedAddress) {
			freeBlock.size += newSize - oldSize;
			return freeBlocks.size() - 1;
		}
	}

	//still here? then the highest block is a reserved block,
	//and we can create a new free block right after it
	TTexBlock newBlock = { highestReservedAddress + 1,newSize - oldSize };
	freeBlocks.push_back(newBlock);
	return freeBlocks.size() - 1;
}


