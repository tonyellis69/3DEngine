#pragma once

#include "model.h"

class CRenderModel : public CModel {
public:
	CRenderModel();
	void drawNew();
	CBaseBuf* getBuffer();
	void freeBuffers();
	void storeVertexes(void* verts, unsigned int vertSize, unsigned int nVerts);
	void storeIndex(unsigned int* indices,  unsigned int nIndices);
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	void setDrawMode(TdrawMode drawMode);
	void setMaterial(CMaterial& iMaterial);
	void assignMaterial();
	CMaterial* getMaterial();


	

	
	CBuf buf; ///<Holds the details of this model's vertex buffer.
		unsigned int hMVP;

protected:
	CRenderer* pRenderer; ///<Lets models talk to renderer.
};
