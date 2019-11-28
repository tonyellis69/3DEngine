#include "multiBuf2.h"

#include <algorithm>

#include "..\utils\log.h"

/** Set the total size of the buffer. Any existing blocks are erased. */
void CMultiBuf2::setSize(int numBytes) {
	buffer.setSize(numBytes);
	reservedBlocks.clear();
	freeBlocks.clear();
	TBlock initialBlock = { 0, numBytes };
	freeBlocks[0] = initialBlock;
	highestAddr = 0;
}

/** Copy the given buffer to a block of free memory, returning the address. */
int CMultiBuf2::copyBuf(CBuf& src, int size) {
	int addr = getFreeBlock(size);
	copyToBlock(src, addr, size);
	return addr;
}

int CMultiBuf2::getBlock(int size)
{
	return 0;
}

void CMultiBuf2::storeLayout(std::initializer_list<int> attribs) {
	std::vector<int> params;
	for (auto attrib : attribs)
		params.push_back(attrib);

	buffer.storeLayout(params[0], params[1], params[2], params[3]);

}

unsigned int CMultiBuf2::getVAO() {
	return buffer.hVAO;
}

/**	Free the given reserved block, so that it can be resused. */
void CMultiBuf2::freeBlock(int addr) {
	TBlock freedBlock = reservedBlocks[addr];
	reservedBlocks.erase(addr);

	//optional: search free list for neighbouring blocks and merge them
	/*for (const auto& [key, value] : freeBlocks)
		if (value.start == freedBlock.start + freedBlock.size) {
			freeBlocks.size += value.size;
			freeBlocks.erase(key);
		}
		*/

	freeBlocks[addr] = freedBlock;
	sysLog << "\nblock at " << freedBlock.start << " size " << freedBlock.size << " returned to memory.";
	auto next = freeBlocks.find(addr + freedBlock.size);
	while (next != freeBlocks.end()) {
		freeBlocks[addr].size += next->second.size;
		next = freeBlocks.erase(next);
		sysLog << "\nblocks merged!";
		if (next == freeBlocks.end() || next->first != addr + freeBlocks[addr].size)
			break;
	}
}


/** Return the address of a block of memory >= size. */
int CMultiBuf2::getFreeBlock(int size) {
	auto largerBlock = std::find_if(freeBlocks.begin(), freeBlocks.end(),
		[&](std::pair<int, TBlock> p) { return p.second.size >= size; });
		//[&](auto& p) { return p.second.size >= size; });
	if (largerBlock == freeBlocks.end()) {
		liveLog << "\nMultibuf could not find a block of memory " << size << " or larger.";
		return -1;
	}

	TBlock freeBlock;
	if (largerBlock->second.size > size * 2) {
		sysLog << "\nsplitting!";
		freeBlock = split(largerBlock->second, size);
	}
	else {
		sysLog << "\nNot splitting!";
		freeBlock = largerBlock->second;
		freeBlocks.erase(largerBlock);
	}

	reservedBlocks[freeBlock.start] = freeBlock;
	highestAddr = 0;
	for (auto block : reservedBlocks) {
		highestAddr = std::max(highestAddr, block.first);
	}
	
	sysLog << " highest reserved addr " << highestAddr;
	return freeBlock.start;
}

/** Split this block in two, where one new block is of the requested size, and return that block. */
TBlock CMultiBuf2::split(TBlock block, int size) {
	//TBlock block = blockIt->second;
	//sysLog << "\nsplit " << block.start << " size " << block.size;
	TBlock newBlock;
	newBlock.size = block.size - size;
	newBlock.start = block.start + size;
	freeBlocks.erase(block.start);

	freeBlocks[newBlock.start] = newBlock;
	
	block.size = size;
	//freeBlocks[block.start] = block;
	//freeBlocks.insert(std::pair(block.size, block));

	//sysLog  << " into size " << block.size << " and new block "<< newBlock.start << " size " << newBlock.size;
	return block;
}

/** Copy the given buffer to a reserved block of memory. */
void CMultiBuf2::copyToBlock(CBuf& src, int blockAddr, int size) {
	glBindBuffer(GL_COPY_READ_BUFFER, src.getBufHandle());
	glBindBuffer(GL_COPY_WRITE_BUFFER, buffer.getBufHandle());
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, blockAddr, size);
}
