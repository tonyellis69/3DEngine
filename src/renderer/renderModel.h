#pragma once

#include "model.h"

class CRenderModel : public CModel {
public:
	CRenderModel();
	void drawNew();
	CBaseBuf* getBuffer();
	void freeBuffers();
	void storeVertexes(void* verts, unsigned int size, unsigned int nVerts);
	void storeIndex(unsigned short* indices, unsigned int size, unsigned int nIndices);
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	void setDrawMode(TdrawMode drawMode);
	void setMaterial(CMaterial& iMaterial);
	void assignMaterial();
	CMaterial* getMaterial();


	

	CRenderer* pRenderer; ///<Lets models talk to renderer.
	CBuf buf; ///<Holds the details of this model's vertex buffer.
		unsigned int hMVP;
};
