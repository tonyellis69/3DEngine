#include "multiBuf.h"
#include "renderer.h"

CMultiBuf::CMultiBuf() {

}

void CMultiBuf::setMultiBufferSize(unsigned int bufSize, unsigned int noObjects) {
	CMultiBuf::bufSize = bufSize;
	CMultiBuf::noObjects = noObjects;
	freeMem = 0;
	first = new GLint[noObjects];
	count = new GLsizei[noObjects];
}




CMultiBuf::~CMultiBuf() {
	delete first;
	delete count;

}
