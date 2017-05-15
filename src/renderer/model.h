#pragma once

#include "3dobject.h"
#include <glew.h>//for GL_TRIANGLES
#include "../vertBufs.h"
#include "buf.h"
#include "multiBuf.h"
#include "material.h"

enum  TdrawMode { drawPoints, drawLines, drawTris};

//class CMaterial;

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
	virtual void storeVertexes(void* verts, unsigned int size, unsigned int nVerts) {};
	virtual void storeIndex(unsigned short* indices, unsigned int size, unsigned int nIndices) {};
	virtual void storeLayout(int attr1, int attr2, int attr3, int attr4) {};
	virtual void setDrawMode(TdrawMode drawMode) {};
	virtual void setColour(glm::vec4 newColour) {};
	virtual void setMaterial(CMaterial& material) {};
	virtual void assignMaterial() {};
	virtual CMaterial* getMaterial() { return NULL; };
	virtual void setViewMatrix(glm::mat4& matrix) {};

	int drawMode; ///<Triangles, lines etc
	glm::vec4 colour; //TO DO: scrap??

	glm::mat4 viewMatrix;

//	int nTris; ///<Number of triangles

	//unsigned int hVertexObj; ///<The vertex object internally assigned to this model.



};

