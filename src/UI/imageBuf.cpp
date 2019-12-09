#include "imageBuf.h"

#include "../utils/log.h"

C2DimageBuf::C2DimageBuf() {
	nextId = 0;
}

/** Set the size of the main buffer. This will wipe any existing content.*/
void C2DimageBuf::setSize(int width, int height) {
	texBuf.resize(width, height  *2 );
	nextId = 0;
	freeBlocks.clear();
	freeBlocks.push_back({ 0,texBuf.height });
}

/** Reserve a chunk of image memory, and return the identifying id. */
unsigned int C2DimageBuf::reserve(glm::i32vec2& size) {
	int index = getFreeBlock(size.y);
	
	if (freeBlocks[index].size >= size.y * 2)
		split(index, size.y);
	
	int addr = freeBlocks[index].start;
	reservedBlocks[addr] = freeBlocks[index];
	freeBlocks.erase(freeBlocks.begin() + index);
	
	return addr;
}

CRenderTexture& C2DimageBuf::getBuffer() {
	return texBuf;
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
		liveLog << "\nCouldn't find a texBlock!!";
		int b = 0; //(no? enlarge texBuf and copy, but that can wait);
	}
	return index;
}

/** Split this block into two, one of size size. */
void C2DimageBuf::split(unsigned int index, int size) {
	TTexBlock newBlock = { freeBlocks[index].start + size, freeBlocks[index].size - size };
	freeBlocks.push_back(newBlock);
	freeBlocks[index].size = size;
}


