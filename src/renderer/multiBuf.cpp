#include "multiBuf.h"
#include "renderer.h"

CMultiBuf::CMultiBuf() {

}

void CMultiBuf::setMultiBufferSize(unsigned int bufSize, unsigned int maxObjects) {
	CMultiBuf::bufSize = bufSize;
	CMultiBuf::maxObjects = maxObjects;
	freeMem = 0;
	first = new GLint[maxObjects];
	count = new GLsizei[maxObjects];
	first[0] = 0;
	currentObjects = 0;
}

int CMultiBuf::getElementSize() {

	return elemSize;
}

void CMultiBuf::reserve(unsigned int elementsUsed) {
	if (currentObjects > 0)
		first[currentObjects] = first[currentObjects - 1] + count[currentObjects - 1];  //freeMem;

	count[currentObjects] = elementsUsed; //how many sequential elements to use from each array
	currentObjects++;
	freeMem += elementsUsed *elemSize;
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
	GLuint hSrcBuf = srcBuf.getBufHandle();
	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glBindBuffer(GL_COPY_WRITE_BUFFER, hBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, freeMem, size);

	freeMem += size;

	if (currentObjects > 0)
		first[currentObjects] = first[currentObjects - 1] + count[currentObjects - 1];
	count[currentObjects] = size / elemSize; //how many sequential elements to use from each array
	currentObjects++;
}


CMultiBuf::~CMultiBuf() {
	delete first;
	delete count;

}
