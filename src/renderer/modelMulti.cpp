
#include "modelMulti.h"
#include "renderer.h"

void CModelMulti::setMultiBufferSize(unsigned int bufSize) {
	multiBuf.setMultiBufferSize(bufSize);
}

unsigned int CModelMulti::getFreeMem() {
	return multiBuf.freeMem;
}

void CModelMulti::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	multiBuf.storeLayout(attr1, attr2, attr3, attr4);
}



