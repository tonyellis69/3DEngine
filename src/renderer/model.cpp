#include "model.h"
#include "vertexObj.h"
#include "renderer.h"

CModel::CModel(glm::vec3& pos) :  C3dObject(pos) {
	init();
}

CModel::CModel() {
	init();
}

void CModel::init() {
	drawMode = GL_TRIANGLES;

}


CRenderModel::CRenderModel() : CModel() {

}

/** Store this model's vertex and index data with the renderer, ie, on the graphics card. */
void CRenderModel::storeIndexed(int noAttribs, vBuf::T3Dvert* verts, unsigned int noVerts, unsigned short* index, int noIndices) {

	buf.nAttribs = noAttribs;
	buf.noVerts = noVerts;
	buf.indexSize = noIndices;

	pRenderer->storeVertexData(buf.hBuffer, (glm::vec3*)verts, buf.noVerts * sizeof(vBuf::T3Dvert));

	pRenderer->storeIndexData(buf.hIndex, index, sizeof(unsigned short) * buf.indexSize);

	pRenderer->storeVertexLayout(buf.hVAO, buf.hBuffer, buf.hIndex, noAttribs);

};


void CRenderModel::drawNew() {

	pRenderer->drawModel(*this);

}

unsigned int CRenderModel::getBuffer() {
	return buf.hBuffer;
}

unsigned int CRenderModel::getBuf()
{
	return 0;
}

void CRenderModel::setVertexDetails(int noAttribs, int noIndices, int noVerts){
	buf.nAttribs = noAttribs;
	buf.noVerts = noVerts;
	buf.indexSize = noIndices;
}

void CRenderModel::storeVertexData(glm::vec3 * data, unsigned int noVerts, unsigned int size) {
	pRenderer->storeVertexData(buf.hBuffer, data, size * buf.nAttribs * noVerts);
	buf.noVerts = noVerts;
//	buf.hIndex = 0;
	//buf.indexSize = 0;
}

void CRenderModel::storeVertexLayout(unsigned int hIndex) {
	pRenderer->storeVertexLayout(buf.hVAO, buf.hBuffer, hIndex, buf.nAttribs);
}

void CRenderModel::storeIndexedData(unsigned short * index) {
	pRenderer->storeIndexData(buf.hIndex, index, sizeof(unsigned short) * buf.indexSize);
}

void CRenderModel::freeBuffers() {
	pRenderer->freeBuffer(buf.hBuffer);
	if (buf.hIndex > 0)
		pRenderer->freeBuffer(buf.hIndex);
	pRenderer->freeVAO(buf.hVAO);
}

unsigned int CModelMulti::getFreeMem() {
	return multiBuf.freeMem;
}
