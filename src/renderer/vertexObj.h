#pragma once
#include <glew.h>

/** Holds all the data necessary to use a buffer of vertices uploaded to the 
	graphics card.*/
class CVertexObj
{
public:
	CVertexObj();
	~CVertexObj();


	GLuint hBuffer; ///<Handle referencing object's vertex data buffer.
	int colourOffset; ///<Start of colour values in buffer.
	unsigned int noVerts;///<Total vertices in buffer.
	int nAttribs; ///<Attributes per vertex.
	GLuint hVAO; ///<Handle referencing vertex array object for this buffer.
	GLuint hIndex; ///<Handle referencing index object for this buffer.
	int indexSize; ///<Number of indices used by buffer.
	int normOffset; ///<Start of normal values in buffer

	unsigned int nTris; ///<Number of triangles
};

