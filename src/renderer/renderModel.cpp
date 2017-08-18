
#include "renderModel.h"

#include "renderer.h"

CRenderModel::CRenderModel() : CModel() {

}

void CRenderModel::drawNew() {
	material->assign();
	pRenderer->drawModel(*this);
	material->unAssign();
}

CBaseBuf* CRenderModel::getBuffer() {
	return &buf;
}


void CRenderModel::freeBuffers() {
	pRenderer->freeBuffer(buf.hBuffer);
	if (buf.hIndex > 0)
		pRenderer->freeBuffer(buf.hIndex);
	pRenderer->freeVAO(buf.hVAO);
}

void CRenderModel::storeVertexes(void * verts, unsigned int vertSize, unsigned int nVerts) {
	buf.storeVertexes(verts, vertSize  * nVerts, nVerts);
}

void CRenderModel::storeIndex(unsigned int * indices, unsigned int nIndices) {
	buf.storeIndex(indices, nIndices);
}

void CRenderModel::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	buf.storeLayout(attr1, attr2, attr3, attr4);
}

void CRenderModel::setDrawMode(TdrawMode iDrawMode) {
	drawMode = pRenderer->getGLdrawMode(iDrawMode);
}



void CRenderModel::setMaterial(CMaterial & iMaterial) {
	material = &iMaterial;
	if (material->getShader()) //TO DO 
		hMVP = material->getShader()->getMVPhandle();
}

void CRenderModel::assignMaterial() {
	material->assign();
}

CMaterial * CRenderModel::getMaterial() {
	return material;
}


