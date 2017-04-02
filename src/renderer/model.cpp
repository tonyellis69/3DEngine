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
	hVertexObj = 0;
	drawMode = GL_TRIANGLES;

}


CRenderModel::CRenderModel() : CModel() {

}

/** Store this model's vertex and index data with the renderer, ie, on the graphics card. */
void CRenderModel::storeIndexed(int noAttribs, vBuf::T3Dvert* verts, unsigned int noVerts, unsigned short* index, int noIndices) {
	if (!hVertexObj)
		hVertexObj = pRenderer->createVertexObj();
	CVertexObj* vertObj = &pRenderer->getVertexObj(hVertexObj);
	vertObj->nAttribs = noAttribs;
	vertObj->noVerts = noVerts;
	vertObj->indexSize = noIndices;

	pRenderer->storeVertexData(vertObj->hBuffer, (glm::vec3*)verts, vertObj->noVerts * sizeof(vBuf::T3Dvert));

	pRenderer->storeIndexData(vertObj->hIndex, index, sizeof(unsigned short) * vertObj->indexSize);

	pRenderer->storeVertexLayout(vertObj->hVAO, vertObj->hBuffer, vertObj->hIndex, noAttribs);
//	pRenderer->getVertexObj(hVertexObj).nAttribs = noAttribs;
};


void CRenderModel::drawNew() {

	pRenderer->drawModel(*this);

}



