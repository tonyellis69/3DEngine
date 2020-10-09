#include "buf2.h"

CBuf2::CBuf2() {
}

void CBuf2::setVAO() {
	sharedBuf->setVAO();
}

/** Set the size of the buffer in bytes. Note that this will create a new
	buffer, disassociating this object from any existing memory. */
void CBuf2::setSize(unsigned int size) {
	sharedBuf = std::make_shared<CSharedBuf>();
	sharedBuf->setSize(size);
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
	sharedBuf->addAttribute(attr);
}

void CBuf2::storeIndexShort(std::vector<unsigned short>& index) {
	if (!sharedBuf)
		sharedBuf = std::make_shared<CSharedBuf>();

	sharedBuf->setNumVerts(numVerts);
	sharedBuf->attachIndex(index.data(), index.size());
	numElements = index.size();
}

