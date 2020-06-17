#include "multiBuf2.h"

#include <algorithm>

#include "..\utils\log.h"

/** Set the total size of the buffer. Any existing blocks are erased. */
void CMultiBuf2::setSize(int numBytes) {
	buffer.setSize(numBytes);
	reservedBlocks.clear();
	freeBlocks.clear();
	freeBlocksBySize.clear();
	TBlock initialBlock = { 0, numBytes };
	freeBlocks[0] = initialBlock;
	highestAddr = 0;
}

/** Copy the given buffer to a reserved block of multibuf's free memory, returning the address. */
int CMultiBuf2::copyBuf(CBuf& src, int size) {
	int addr = getFreeBlock(size);
	copyToBlock(src, addr, size);
	return addr;
}

/** Note how vertexes are stored, for the benefit of whatever will draw them. */
void CMultiBuf2::storeLayout(std::initializer_list<int> attribs) {
	std::vector<int> params;
	for (auto attrib : attribs)
		params.push_back(attrib);
	buffer.storeLayout(params[0], params[1], params[2], params[3]);
}

/** Return our VAO for use in drawing the stored vertexes. */
unsigned int CMultiBuf2::getVAO() {
	return buffer.hVAO;
}

/**	Free the reserved block at the given address, so that it can be resused. */
void CMultiBuf2::freeBlock(int addr) {
	
	TBlock freedBlock = reservedBlocks[addr];
	reservedBlocks.erase(addr);

	freeBlocks[addr] = freedBlock;
/*
	//Get the free blocks either side of this one, if they exist
	auto next = freeBlocks.find(freedBlock.start + freedBlock.size);
	auto prev = next;
	if (prev != freeBlocks.end())
		prev = std::prev(next);

	//merge any preceding free block
	if (prev != freeBlocks.end() && (prev->second.start + prev->second.size == addr)) {
		freedBlock.start = prev->second.start;
		freedBlock.size += prev->second.size;
		next = freeBlocks.erase(prev);
	}
 
	//merge any following free block
	if (next != freeBlocks.end()) {
		freedBlock.size += next->second.size;
		freeBlocks.erase(next);
	}
	
	freeBlocks[freedBlock.start] = freedBlock;
	*/
	//Removing just these blocks from freeBlocksBySize is hard, so wipe the whole thing.
	if (!freeBlocksBySize.empty())
		freeBlocksBySize.clear();
}


int CMultiBuf2::exportBlock(int addr, char* buf) {
	int blockSize = reservedBlocks[addr].size;
	glBindBuffer(GL_COPY_READ_BUFFER, buffer.getBufHandle());
	glGetBufferSubData(GL_COPY_READ_BUFFER, addr, blockSize, buf);
	return blockSize;
}



/** Return the address of a free block of memory >= size. */
int CMultiBuf2::getFreeBlock(int size) {
	if (freeBlocksBySize.empty())
		rebuildFreeBlocksSized();
		
	auto largerBlock = freeBlocksBySize.lower_bound(size);

	//if we can't find a free block, panic
	//if (largerBlock == freeBlocksBySize.end()) { 
	while  (largerBlock == freeBlocksBySize.end()) {
		memoryPanic();
		largerBlock = freeBlocksBySize.lower_bound(size);
	}

	TBlock freeBlock;
	if (largerBlock->second.size > size ) {
		freeBlock = largerBlock->second;
		freeBlocksBySize.erase(largerBlock);
		freeBlock = split(freeBlock, size);
	}
	else {
		freeBlock = largerBlock->second;
		freeBlocksBySize.erase(largerBlock);
		freeBlocks.erase(freeBlock.start);
	}

	reservedBlocks[freeBlock.start] = freeBlock;
	highestAddr = std::max(highestAddr,freeBlock.start + freeBlock.size);
	return freeBlock.start;
}

/** Split this block in two, where one new block is of the requested size, and return the address of that block. */
TBlock CMultiBuf2::split(TBlock block, int size) {
	TBlock newBlock;
	newBlock.size = block.size - size;
	newBlock.start = block.start + size;
	freeBlocks.erase(block.start); 
	freeBlocks[newBlock.start] = newBlock;
	freeBlocksBySize.insert({ newBlock.size,newBlock });
	block.size = size;
	return block;
}

/** Copy the given buffer to a reserved block of our buffer. */
void CMultiBuf2::copyToBlock(CBuf& src, int blockAddr, int size) {	
	glBindBuffer(GL_COPY_READ_BUFFER, src.getBufHandle());
	glBindBuffer(GL_COPY_WRITE_BUFFER, buffer.getBufHandle());
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, blockAddr, size);
}

/** We've run out of memory. Create a larger buffer and copy everything to that.*/
void CMultiBuf2::memoryPanic() {
	unsigned int oldSize = buffer.getBufSize();
	unsigned int newSize = oldSize * 1.1;
	buffer.resizeSafe(newSize);

	//Ensure a free block points to our new spare memory:

	//is there a free block at the end of the old memory? If so, extend it
	std::multimap<int, TBlock>::iterator block = freeBlocksBySize.begin();
	while(block != freeBlocksBySize.end()) {
		if (block->second.start + block->second.size >= highestAddr) {
			auto  endBlock = freeBlocks[block->second.start];
			freeBlocks[block->second.start].size += newSize - oldSize;
			endBlock.size += newSize - oldSize;
			freeBlocksBySize.erase(block);
			freeBlocksBySize.insert({ endBlock.size,endBlock });
			return;
		}
		block++;
	}

	//still here? then the highest block is a reserved block, and we can create a new free block right after it
	TBlock newBlock = { highestAddr,newSize - oldSize };
	freeBlocks[highestAddr ] = newBlock;
	freeBlocksBySize.insert({ newBlock.size,newBlock });
}

/** Create a list of free blocks ordered by size. */
void CMultiBuf2::rebuildFreeBlocksSized() {
	for (auto block : freeBlocks)
		freeBlocksBySize.insert({ block.second.size,block.second });
}
