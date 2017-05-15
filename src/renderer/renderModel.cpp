
#include "renderModel.h"

#include "renderer.h"

CRenderModel::CRenderModel() : CModel() {

}

/** Store this model's vertex and index data with the renderer, ie, on the graphics card. */
void CRenderModel::storeIndexed(int noAttribs, vBuf::T3Dvert* verts, unsigned int noVerts, unsigned short* index, int noIndices) {

	buf.nAttribs = noAttribs;
	buf.noVerts = noVerts;
	buf.noIndices = noIndices;

	pRenderer->storeVertexData(buf.hBuffer, (glm::vec3*)verts, buf.noVerts * sizeof(vBuf::T3Dvert));

	pRenderer->storeIndexData(buf.hIndex, index, sizeof(unsigned short) * buf.noIndices);

	pRenderer->storeVertexLayout(buf.hVAO, buf.hBuffer, buf.hIndex, noAttribs);

};


void CRenderModel::drawNew() {

	material->assign();


	//	pRenderer->setShaderValue(hMVP, 1, viewMatrix * worldMatrix);


	pRenderer->drawModel(*this);
	

	material->free();

}

unsigned int CRenderModel::getBuffer() {
	return buf.hBuffer;
}

unsigned int CRenderModel::getBuf()
{
	return 0;
}


void CRenderModel::setVertexDetails(int noAttribs, int noIndices, int noVerts) {
	buf.nAttribs = noAttribs;
	buf.noVerts = noVerts;
	buf.noIndices = noIndices;
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
	pRenderer->storeIndexData(buf.hIndex, index, sizeof(unsigned short) * buf.noIndices);
}

void CRenderModel::freeBuffers() {
	pRenderer->freeBuffer(buf.hBuffer);
	if (buf.hIndex > 0)
		pRenderer->freeBuffer(buf.hIndex);
	pRenderer->freeVAO(buf.hVAO);
}

void CRenderModel::storeVertexes(void * verts, unsigned int size, unsigned int nVerts) {
	buf.storeVertexes(verts, size, nVerts);
}

void CRenderModel::storeIndex(unsigned short * indices, unsigned int size, unsigned int nIndices) {
	buf.storeIndex(indices, size, nIndices);
}

void CRenderModel::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	buf.storeLayout(attr1, attr2, attr3, attr4);
}

void CRenderModel::setDrawMode(TdrawMode iDrawMode) {
	if (iDrawMode == drawPoints)
		drawMode = GL_POINTS;
	if (iDrawMode == drawLines)
		drawMode = GL_LINES;
	if (iDrawMode == drawTris)
		drawMode = GL_TRIANGLES;
}

void CRenderModel::setColour(glm::vec4 newColour) {
	colour = newColour;
}

void CRenderModel::setMaterial(CMaterial & iMaterial) {
	material = &iMaterial;
	if (material->getShader() ) //TO DO 
		hMVP = material->getShader()->getMVPhandle();
}

void CRenderModel::assignMaterial() {
	material->assign();
}

CMaterial * CRenderModel::getMaterial() {
	return material;
}

void CRenderModel::setViewMatrix(glm::mat4 & matrix) {
	viewMatrix = matrix;
}

