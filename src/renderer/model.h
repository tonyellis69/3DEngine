#pragma once

#include "3dobject.h"
#include <glew.h>//for GL_TRIANGLES
#include "../vertBufs.h"
#include "multiBuf.h"
#include "vertexObj.h"

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
	virtual unsigned int getBuffer() { return 0; };
	virtual unsigned int getBuf() { return 0; };
	virtual void setVertexDetails(int noAttribs, int noIndices, int noVerts) { };
	virtual void storeVertexData(glm::vec3* data, unsigned int noVerts, unsigned int size) {};
	virtual void storeVertexLayout(unsigned int hIndex) {};
	virtual void storeIndexedData(unsigned short* index) {};
	virtual void freeBuffers() {};

	int drawMode; ///<Triangles, lines etc

//	int nTris; ///<Number of triangles

	//unsigned int hVertexObj; ///<The vertex object internally assigned to this model.

private:

};

class CRenderer;

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


	CRenderer* pRenderer; ///<Lets models talk to renderer.
	CVertexObj buf; ///<Holds the details of this model's vertex buffer.
};


class CModelMulti : public CModel {
public:
	CModelMulti() : CModel() {};
	CModelMulti(glm::vec3& pos) : CModel(pos) {};
	virtual void setMultiBufferSize(unsigned int bufSize, unsigned int noObjects, int noAttribs, int elemSize) {};
	unsigned int getFreeMem();
	virtual int getElementSize() { return 0; };
	virtual GLint* getFirstArray() { return 0; };
	virtual GLsizei* getCountArray() { return 0; };

	virtual void reserveBuf(unsigned int elementsUsed) {};
	
	CMultiBuf multiBuf;

};