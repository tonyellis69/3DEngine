#pragma once
#include "baseBuf.h"
#include <glew.h>
#include <glm/glm.hpp>



/** Holds all the data necessary to use a buffer of vertices uploaded to the 
	graphics card.*/
enum TRendererDrawMode;
class CBuf : public CBaseBuf
{
public:
	CBuf();
	~CBuf();
	void storeVertexes(void* verts, unsigned int size, unsigned int nVerts);
	void storeIndex(unsigned short * indices, unsigned int size, unsigned int nIndices);
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	void setSize(unsigned int size);
	void createBuf(unsigned int size);
	void copyBuf(CBaseBuf& srcBuf, unsigned int size);
	unsigned int getBufHandle();
	unsigned int getBufSize();
	void freeMem();
	void setDrawMode(TdrawMode mode);
	void getData(unsigned char* data, unsigned int size);

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
	TRendererDrawMode drawMode; ///<How this buffer expects to be drawn, if it all.
};

