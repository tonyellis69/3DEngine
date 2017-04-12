#pragma once

#include "glew.h"
//#include "renderer.h"
#include "..\vertBufs.h" //temp!
class CRenderer;

/** Manages a renderer-based buffer for multidraw drawing. */
class CMultiBuf {
public:
	CMultiBuf();
	~CMultiBuf();
	void setMultiBufferSize(unsigned int bufSize, unsigned int maxObjects);
	int getElementSize();
	void reserve(unsigned int size);
	GLint* getFirstArray();
	GLsizei* getCountArray();

	GLint* first;
	GLsizei* count;
	unsigned int freeMem;
	unsigned int bufSize;
	unsigned int maxObjects;
	unsigned int currentObjects;


	GLuint hBuffer; ///<Handle referencing object's vertex data buffer.
	int colourOffset; ///<Start of colour values in buffer.
	unsigned int noVerts;///<Total vertices in buffer.
	int nAttribs; ///<Attributes per vertex.
	GLuint hVAO; ///<Handle referencing vertex array object for this buffer.
	GLuint hIndex; ///<Handle referencing index object for this buffer.
	int indexSize; ///<Number of indices used by buffer.
	int normOffset; ///<Start of normal values in buffer

	int elemSize;

	unsigned int nTris; ///<Number of triangles


	//vBuf::T3Dvert* ptr; ///TEMP!!!!
};