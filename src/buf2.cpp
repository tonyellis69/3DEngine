#include "buf2.h"

CBuf2::CBuf2() {
}

void CBuf2::setVAO() {
	sharedBuf->setVAO();
}

void CBuf2::clearVAO() {
	sharedBuf->clearVAO();
}

/** Set the size of the buffer in bytes. Note that this will create a new
	buffer, disassociating this object from any existing memory. */
void CBuf2::setSize(unsigned int size) {
	sharedBuf = std::make_shared<CSharedBuf>();
	sharedBuf->setSize(size);
}

/** Free the memory associated with this buffer by throwing it away.
	We provide a new sharedBuf in case the user still wants to use this buffer. */
void CBuf2::freeMem() {
	sharedBuf = std::make_shared<CSharedBuf>();
	numVerts = 0;
	numElements = 0;
}

unsigned int CBuf2::getBufferHandle() {
	return sharedBuf->getBufferHandle();
}

/** Stores the given vertex array in a shareable internal buffer. */
void CBuf2::storeVerts(void* data, unsigned int numVerts, int vertSize) {
	if (!sharedBuf)
		sharedBuf = std::make_shared<CSharedBuf>();
	
	sharedBuf->attachData(data,  numVerts, vertSize);

	sharedBuf->setAttribs();

}


void CBuf2::storeIndexInt(std::vector<unsigned int>& index) {
	if (!sharedBuf)
		sharedBuf = std::make_shared<CSharedBuf>();

	sharedBuf->setNumVerts(numVerts);
	sharedBuf->attachIndex(index.data(),index.size());
	numElements = index.size();
}
void CBuf2::addAttributeSize(int attr) {
	if (!sharedBuf)
		sharedBuf = std::make_shared<CSharedBuf>();
	sharedBuf->addAttribute(attr);
}

void CBuf2::storeIndexShort(std::vector<unsigned short>& index) {
	if (!sharedBuf)
		sharedBuf = std::make_shared<CSharedBuf>();

	sharedBuf->setNumVerts(numVerts);
	sharedBuf->attachIndex(index.data(), index.size());
	numElements = index.size();
}

