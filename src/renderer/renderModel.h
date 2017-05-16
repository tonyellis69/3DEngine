#pragma once

#include "model.h"

class CRenderModel : public CModel {
public:
	CRenderModel();
	void storeIndexed(int noAttribs, vBuf::T3Dvert* verts, unsigned int noVerts, unsigned short* index, int noIndices);
	void drawNew();
	unsigned int getBuffer();
	unsigned int getBuf();
	void setVertexDetails(int noAttribs, int noIndices, int noVerts);
	void storeVertexData(glm::vec3* data, unsigned int noVerts, unsigned int size);
	void storeVertexLayout(unsigned int hIndex);
	void storeIndexedData(unsigned short* index);
	void freeBuffers();
	void storeVertexes(void* verts, unsigned int size, unsigned int nVerts);
	void storeIndex(unsigned short* indices, unsigned int size, unsigned int nIndices);
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	void setDrawMode(TdrawMode drawMode);
	void setColour(glm::vec4 newColour);
	void setMaterial(CMaterial& iMaterial);
	void assignMaterial();
	CMaterial* getMaterial();
	void setViewMatrix(glm::mat4 & matrix);

	

	CRenderer* pRenderer; ///<Lets models talk to renderer.
	CBuf buf; ///<Holds the details of this model's vertex buffer.
		unsigned int hMVP;
};
