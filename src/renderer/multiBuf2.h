#pragma once

#include <unordered_map>
#include <map>

#include "buf.h"

struct TBlock {
	int start;
	int size;
};

/** An class that enables a single vertex buffer to be accessed as a number
	of separate blocks, which can then be read and written to without the 
	overhead of changing buffers. Freed blocks are recycled. */
class CMultiBuf2 {
public:
	CMultiBuf2() {}
	void setSize(int numBytes);
	int copyBuf(CBuf& src, int size);
	void freeBlock(int addr);
	int exportBlock(int id, char* buf);
	void storeLayout(std::initializer_list<int> attribs);
	unsigned int getVAO();


private:
	int getFreeBlock(int size);
	TBlock split(TBlock block, int size);
	void copyToBlock(CBuf& src, int blockAddr, int size);
	void memoryPanic();
	void rebuildFreeBlocksSized();


	std::multimap<int, TBlock> freeBlocksBySize; //ordered by size
	std::map<int, TBlock> freeBlocks; //ordered by start
	std::unordered_map<int, TBlock> reservedBlocks; //ordered by start

	int highestAddr;


	CBuf buffer; //make private!!!!!
};
