#pragma once
#include "baseBuf.h"
#include <glew.h>
#include <glm/glm.hpp>
//#include "renderer.h"


/** Holds all the data necessary to use a buffer of vertices uploaded to the 
	graphics card.*/
//enum TRendererDrawMode;
class CRenderer;

class CBuf : public CBaseBuf
{
public:
	CBuf() ;
	~CBuf();
	void storeVertexes(void* verts, unsigned int size, unsigned int nVerts);
	void storeIndex(unsigned int * indices, unsigned int nIndices);
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	void setSize(unsigned int size);
	void createBuf(unsigned int size);
	void copyBuf(CBaseBuf& srcBuf, unsigned int size);
	unsigned int getBufHandle();
	unsigned int getBufSize();
	void freeMem();
	void getData(unsigned char* data, unsigned int size);
	void getLayout(int& attr1, int& attr2, int& attr3, int& attr4);
	unsigned int getNoVerts();
	void setNoVerts(unsigned int nVerts);
	void setInstanced(CBaseBuf& buf, int noAttribs);
	unsigned int getNoIndices();
	unsigned int getIndexHandle();
	CRenderer* getRenderer();

	GLuint hBuffer; ///<Handle referencing object's vertex data buffer.
	int colourOffset; ///<Start of colour values in buffer.
	unsigned int noVerts;///<Total vertices in buffer.
	int nAttribs; ///<Attributes per vertex.
	GLuint hVAO; ///<Handle referencing vertex array object for this buffer.
	GLuint hIndex; ///<Handle referencing index object for this buffer.
	unsigned int noIndices; ///<Number of indices used by buffer.
	int normOffset; ///<Start of normal values in buffer

	unsigned int nTris; ///<Number of triangles

	unsigned int bufSize; ///<Size of vertex data in bytes.
	int attr[4]; ///<Records attribute layout.

	CBaseBuf* instancedBuf; ///If we're drawing an instanced model, this is it.
	GLuint hInstancedBuf; ///<If we're drawing an instanced model, this is it.
	int nInstancedAttribs; ///<How many attributes belong to the instanced model, if we have one.
	unsigned int nInstancedVerts; ///<How many verts belong to the instanced model, if any.

	GLuint indexType; ///<unsigned short or unsigned int

private:
	CRenderer* pRenderer;
};

