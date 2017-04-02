#pragma once

#include "3dobject.h"
#include <glew.h>//for GL_TRIANGLES
#include "../vertBufs.h"
#include "multiBuf.h"

//class CRenderer;

/** Class for 3d polygon models. 
	NB: because vertices are ultimately stored with the graphics hardware,
	ie, 'server side', we don't actually store them with the model.*/
class CModel : public C3dObject {
public:
	CModel();
	CModel(glm::vec3& pos);
	void init();
	virtual void storeIndexed(int noAttribs, vBuf::T3Dvert *verts, unsigned int noVerts, unsigned short* index, int noIndices) {};
	virtual void drawNew() {};

	int drawMode; ///<Triangles, lines etc

//	int nTris; ///<Number of triangles

	unsigned int hVertexObj; ///<The vertex object internally assigned to this model.

private:

};

class CRenderer;

class CRenderModel : public CModel {
public:
	CRenderModel();
	void storeIndexed(int noAttribs, vBuf::T3Dvert* verts, unsigned int noVerts, unsigned short* index, int noIndices);
	void drawNew();

	CRenderer* pRenderer; ///<Lets models talk to renderer.
};


class CModelMulti : public CModel {
public:
	CModelMulti() : CModel() {};
	CModelMulti(glm::vec3& pos) : CModel(pos) {};
	virtual void setMultiBufferSize(unsigned int bufSize, unsigned int noObjects) {};

	CMultiBuf multiBuf;
};