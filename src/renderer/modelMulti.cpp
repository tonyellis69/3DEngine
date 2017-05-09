
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

void CModelMulti::drawNew() {
	glBindVertexArray(multiBuf.childBufs[0].hVAO);
	CChildBuf* childBuf;
	for (int child = 0; child < multiBuf.noChildBufs; child++) {
		childBuf = &multiBuf.childBufs[child];
		glBindVertexArray(multiBuf.childBufs[child].hVAO);
		for (int object = 0; object < childBuf->objCount; object++) {
			pRenderer->setShaderValue(pRenderer->hColour, 1, childBuf->colour[object]);
			glDrawArrays(GL_TRIANGLES, childBuf->first[object], childBuf->count[object]);
		}
	}


}

