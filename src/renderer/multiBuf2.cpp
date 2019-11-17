#include "multiBuf2.h"

#include "..\utils\log.h"

/** Set the total size of the buffer. Any existing blocks are erased. */
void CMultiBuf2::setSize(int numBytes) {
	buffer.setSize(numBytes);
	reservedBlocks.clear();
	freeBlocks.clear();
	freeBlocks[numBytes] = { 0,numBytes };

}

/** Copy the given buffer to a block of free memory, returning the address. */
int CMultiBuf2::copyBuf(CBuf& src, int size) {
	int bufSize = src.getBufSize();
	int addr = getFreeBlock(bufSize);
	return addr;
}

int CMultiBuf2::getBlock(int size)
{
	return 0;
}

void CMultiBuf2::freeBlock(int addr)
{
}


/** Return the address of a block of memory >= size. */
int CMultiBuf2::getFreeBlock(int size) {
	auto largerBlock = freeBlocks.lower_bound(size);
	if (largerBlock == freeBlocks.end()) {
		liveLog << "\nMultibuf could not find a block of memory " << size << " or larger.";
		return -1;
	}

	TBlock freeBlock;
	if (largerBlock->second.size > size * 2) {
		freeBlock = split(largerBlock->second, size);
	}
	else
		freeBlock = largerBlock->second;

	reservedBlocks[freeBlock.start] = freeBlock;
	freeBlocks.erase(freeBlock.size);
	return freeBlock.start;
}

/** Split this block in two, where one new block is of the requested size, and return that block. */
TBlock CMultiBuf2::split(TBlock& block, int size) {
	TBlock newBlock;
	newBlock.size = block.size - size;
	newBlock.start = block.start + size;
	block.size = size;
	freeBlocks[newBlock.start] = newBlock;

	return block;
}
