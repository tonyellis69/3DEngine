#include "multiBuf.h"
#include "renderer.h"

#include "..\vertBufs.h"

#include <iostream>
#include <algorithm>

using namespace std;

CMultiBuf::CMultiBuf() {
	noChildBufs = 0;
	minBufSize = 0;
	tmp = 0;
	attr[0] = attr[1] = attr[2] = attr[3] = 0;
}

void CMultiBuf::setMultiBufferSize(unsigned int bufSize) {
	setSize(bufSize);
	maxBufSize = bufSize;
	freeMem = 0;
	lastId = 0;
}

int CMultiBuf::getElementSize() {

	return elemSize;
}

/** Preserve the previous size bytes of current storage so they can't be overwritten. This is useful when the storage has
	been written to directly. */
/* Deprecated, not updated. */
void CMultiBuf::reserve(unsigned int size) {
	freeMem += size;

	CChildBuf* currentChild = &childBufs.back();
	unsigned int objCount = currentChild->objCount;
	if (objCount > 0)
		currentChild->first.push_back(currentChild->first[objCount - 1] + currentChild->count[objCount - 1]);
	else
		currentChild->first.push_back(0);
	currentChild->count.push_back(size / elemSize);
	currentChild->objCount++;

	if (maxBufSize - freeMem < minBufSize )
		createChildBuf();
}


/** Copy the given buffer data into this multibuffer. */
void CMultiBuf::copyBuf(CBaseBuf & srcBuf, unsigned int size) {
	GLuint hSrcBuf = srcBuf.getBufHandle();
	unsigned int freeBlock = getFreeBlock(size);
	if (freeBlock) {
		copyToFreeBlock(hSrcBuf, freeBlock, size);
	}
	else {
		copyToNewBlock(hSrcBuf, size);
	}



//	cerr << "\nNumber of childbufs " << childBufs.size();
//	cerr << " blocks " << blocks.size();
//	cerr << " free blocks " << freeBlocks.size(); 
//	Useful stats. Freeblocks eventually settles at about 1/7 of the total blocks, a little high,  but since 
//	total blocks eventually stops rising anyway it's not a huge concern. An optimisation would be to 
//	consolidate adjacent empty blocks periodically, which would require resizing the blocks map.
}

/** Set the initial size of this multibuffer. This creates its initial child buffer. */
void CMultiBuf::setSize(unsigned int size) {
	if (noChildBufs > 0)
		return;
	maxBufSize = size;
	createChildBuf();
}

void CMultiBuf::createChildBuf() {
	CChildBuf newBuf;
	
	childBufs.push_back(newBuf);
	//childBufs.back().setRenderer(getRenderer());
	childBufs.back().setSize(maxBufSize);
	
	if (instancedBuf) {
		childBufs.back().setInstanced(*instancedBuf, nInstancedAttribs);
	}
	childBufs.back().storeLayout(attr[0], attr[1], attr[2], attr[3]);
	noChildBufs++;
	freeMem = 0;
	
}

void CMultiBuf::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	attr[0] = attr1; attr[1] = attr2; attr[2] = attr3; attr[3] = attr4;
	//(re)set this layout for all the child buffers
	for (size_t child = 0; child < childBufs.size(); child++) {
		childBufs[child].storeLayout(attr1, attr2, attr3, attr4);
	}
	CBuf::storeLayout(attr1, attr2, attr3, attr4); //TO DO: is this needful???
	elemSize = 0;
	for (int attribute = nInstancedAttribs; attribute < 4; attribute++) {
		elemSize += attr[attribute] * sizeof(float);
	}
	//elemSize = (attr1 + attr2 + attr3 + attr4 - ) * sizeof(float);
	
}

unsigned int CMultiBuf::getBufHandle() {
	return childBufs.back().hBuffer;
}

/** Only need to set this if we're using the reserve method - to ensure enough memory is available after
	the reservation. */
void CMultiBuf::setMinSize(unsigned int minSize) {
	minBufSize = minSize;
}

unsigned int CMultiBuf::getLastId() {
	return lastId;
}

void CMultiBuf::draw() {
	/*unsigned int childBufNo = 0;
	unsigned int childNo = 0;
	unsigned int newVao;
	size_t totalBlocks = blocks.size();
	for (unsigned int b = 0; b < totalBlocks; b++) {
		newVao = blocks[b].id >> 16;
		if (childBufNo != newVao) { //NB this is much faster than rebinding every time
			childBufNo = newVao;
			setVAO(childBufs[childBufNo - 1].hVAO);
		}
		glDrawArrays(GL_TRIANGLES,blocks[b].arrayFirst, blocks[b].arrayCount);


	} */


/*	pRenderer->setVAO(childBufs[0].hVAO);
	CChildBuf* childBuf;
	for (int child = 0; child < noChildBufs; child++) {
		childBuf = &childBufs[child];
		pRenderer->setVAO(childBufs[child].hVAO);
		for (int object = 0; object < childBuf->objCount; object++) {
			
			glDrawArrays(GL_TRIANGLES, childBuf->first[object], childBuf->count[object]);
		}
	} */
}

void CMultiBuf::deleteBlock(unsigned int id) {


	
//	unsigned short childBufNo = id >> 16;
//	unsigned short blockId = id & 0xFF;

//	CChildBuf* childBuf = &childBufs[childBufNo-1];
	//childBuf->first.erase(childBuf->first.begin() + blockId - 1);
//	childBuf->count[blockId-1] = 0;
	//childBuf->objCount--;

	//The problem is that we can't delete individual entries in the childbuf lists without invalidating the ids of the rest,
	//as they're based on positionHint.
	//need to find the block on the block list, then recompile the relevant childbuf lists. One optimisation would be to set
	//count to zero in the short term, and only recompile at a certain threshold.
	//wait.we're still invalidating the ids of existing chunks if we recompile the lists.
	//the id has to relate to the permanent, blocks list. But then how does the blocks list identify the parts of a childBuf
	//it wants to erase? It will have to hold that childbuf index positionHint and reset those when it recompiles

	//this will be less of an issue once we start re-using blocks

	//so...
	//find this id in the block list
	//find the relevant childbuf
	//clear that block
	//mark it as empty in the blocks list
	
	CChildBlock* block = &blocks[id];
	unsigned short childBufNo = (id >> 16) - 1;
	unsigned short blockIdx = block->blockIdx;

	CChildBuf* childBuf = &childBufs[childBufNo];
	childBuf->count[blockIdx] = 0;
	block->unAssign = true;
	freeBlocks.insert(pair<unsigned int,unsigned int>(block->blockSize,id));
	
}


void CMultiBuf::copyBlock(unsigned int id, char * buf) {
	CChildBlock* block = &blocks[id];
	unsigned short childBufNo = (id >> 16) - 1;
	
	CChildBuf* childBuf = &childBufs[childBufNo];
	GLuint hSrcBuf = childBuf->getBufHandle();

	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glGetBufferSubData(GL_COPY_READ_BUFFER, block->blockStart, block->blockSize, buf);
}

unsigned int CMultiBuf::getBlockSize(unsigned int id) {
	CChildBlock* block = &blocks[id];
	return block->blockSize;
}

/** Return the vertex data for the given element. */
void CMultiBuf::getElementData(const unsigned int id, unsigned int& firstVert, unsigned int& vertCount,  int& childBufNo) {
	CChildBlock* block = &blocks[id];
	firstVert = block->arrayFirst;
	vertCount = block->arrayCount;
	childBufNo = (id >> 16) - 1;
}

/** Return the id of the nearest larger block than size, or zero. */
unsigned int CMultiBuf::getFreeBlock(unsigned int size) {
	std::multimap<unsigned int, unsigned int>::iterator it = freeBlocks.lower_bound(size);
	if (it == freeBlocks.end())
		return 0;
	unsigned int blockId = it->second;
	freeBlocks.erase(it);
	return blockId;
}

/** Copy size bytes of the given buffer to a freed block of memory. */
void CMultiBuf::copyToFreeBlock(unsigned int hSrcBuf, unsigned int freeBlock, unsigned int size) {
	
	
	CChildBlock* block = &blocks[freeBlock];
	unsigned short childBufNo = (freeBlock >> 16) - 1;
	unsigned int blockStart = block->blockStart;
	unsigned int blockIdx = block->blockIdx;
	CChildBuf* childBuf = &childBufs[childBufNo];
	GLuint hDestBuf = childBuf->getBufHandle();

	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glBindBuffer(GL_COPY_WRITE_BUFFER, hDestBuf);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, blockStart, size);

	childBuf->count[blockIdx] = size / elemSize;
	block->unAssign = false;
	block->arrayCount = childBuf->count[blockIdx];
	lastId = freeBlock;
}

/** Reserve a new block of memory and copy size bytes of the given buffer to it. */
void CMultiBuf::copyToNewBlock(unsigned int hSrcBuf, unsigned int size) {
	//do we have room for this many bytes in the current child buffer?
	if ((freeMem + size) > maxBufSize || !noChildBufs) {
		//create a new child buffer and make it current
		createChildBuf();
	}

	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glBindBuffer(GL_COPY_WRITE_BUFFER, childBufs.back().getBufHandle());
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, freeMem, size);

	CChildBuf* currentChild = &childBufs.back();
	unsigned int objCount = currentChild->objCount;

	currentChild->first.push_back(freeMem / elemSize);
	currentChild->count.push_back(size / elemSize);

	//record on blocks list
	CChildBlock newBlock;
	newBlock.id = (childBufs.size() << 16) + currentChild->getNextId();
	//so id  = childBuf number + next free id number of that childbuffer

	newBlock.arrayFirst = currentChild->first.back();
	newBlock.arrayCount = currentChild->count.back();
	newBlock.blockStart = freeMem;
	newBlock.blockSize = size;
	newBlock.blockIdx = currentChild->objCount;
	newBlock.unAssign = false;

	blocks[newBlock.id] = newBlock;
	lastId = newBlock.id;

	currentChild->objCount++;
	freeMem += size;
}

/**	Assign a vertex buffer for instanced drawing.  */
void CMultiBuf::setInstanced(CBaseBuf& buf, int noAttribs) {
	for (size_t child = 0; child < childBufs.size(); child++) {
		childBufs[child].setInstanced(buf, noAttribs);
	}
	instancedBuf = &buf;
	hInstancedBuf = buf.getBufHandle();
	nInstancedAttribs = noAttribs;
}




CMultiBuf::~CMultiBuf() {
	//delete first;
	//delete count;

}

unsigned short CChildBuf::getNextId() {
	return nextId++;
}
