#include "multiBuf.h"
#include "renderer.h"

#include "..\vertBufs.h"

CMultiBuf::CMultiBuf() {
	noChildBufs = 0;
	minBufSize = 0;
}

void CMultiBuf::setMultiBufferSize(unsigned int bufSize) {
	setSize(bufSize);
	maxBufSize = bufSize;
	CMultiBuf::maxObjects = 2000;
	freeMem = 0;
	lastId = 0;
}

int CMultiBuf::getElementSize() {

	return elemSize;
}

/** Preserve the previous size bytes of current storage so they can't be overwritten. This is useful when the storage has
	been written to directly. */
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

GLint * CMultiBuf::getFirstArray()
{
	return nullptr;
}

GLsizei * CMultiBuf::getCountArray()
{
	return nullptr;
}

/** Copy the given buffer data into free memory. */
void CMultiBuf::copyBuf(CBaseBuf & srcBuf, unsigned int size) {


	//do we have room for this many bytes in the current child buffer?
	if ((freeMem + size) > maxBufSize || !noChildBufs) {
		//create a new child buffer and make it current
		createChildBuf();
	}

	GLuint hSrcBuf = srcBuf.getBufHandle();
	
	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glBindBuffer(GL_COPY_WRITE_BUFFER, childBufs.back().getBufHandle());
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, freeMem, size);
	

	freeMem += size;

	CChildBuf* currentChild = &childBufs.back();
	unsigned int objCount = currentChild->objCount;
	if (objCount > 0)
		currentChild->first.push_back(currentChild->first[objCount - 1] + currentChild->count[objCount - 1]);
	else
		currentChild->first.push_back(0);
	currentChild->count.push_back( size / elemSize);
	currentChild->objCount++;

	//record on blocks list
	CChildBlock newBlock;
	newBlock.id = (childBufs.size() << 16) + currentChild->getNextId();
	newBlock.start = currentChild->first.back();
	newBlock.size = currentChild->count.back();

	blocks.push_back(newBlock);
	lastId = newBlock.id;
	
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
//	newBuf.setSize(maxBufSize);
//	newBuf.storeLayout(attr[0], attr[1], attr[2], attr[3]);


	childBufs.push_back(newBuf);
	childBufs.back().setSize(maxBufSize);
	childBufs.back().storeLayout(attr[0], attr[1], attr[2], attr[3]);

	

	noChildBufs++;
	freeMem = 0;
}

void CMultiBuf::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	//(re)set this layout for all the child buffers
	for (size_t child = 0; child < childBufs.size(); child++) {
		childBufs[child].storeLayout(attr1, attr2, attr3, attr4);
	}
	CBuf::storeLayout(attr1, attr2, attr3, attr4);
	elemSize = (attr1 + attr2 + attr3 + attr4) * sizeof(float);
	attr[0] = attr1; attr[1] = attr2; attr[2] = attr3; attr[3] = attr4;
}

unsigned int CMultiBuf::getBufHandle() {
	return childBufs.back().hBuffer;
}

void CMultiBuf::setMinSize(unsigned int minSize) {
	minBufSize = minSize;
}

unsigned int CMultiBuf::getLastId() {
	return lastId;
}


CMultiBuf::~CMultiBuf() {
	delete first;
	delete count;

}

unsigned short CChildBuf::getNextId() {
	return nextId++;
}
