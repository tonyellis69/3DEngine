#pragma once

#include "glew.h"
//#include "renderer.h"

class CRenderer;

/** Manages a renderer-based buffer for multidraw drawing. */
class CMultiBuf {
public:
	CMultiBuf();
	~CMultiBuf();
	void setMultiBufferSize(unsigned int bufSize, unsigned int noObjects);

	GLint* first;
	GLsizei* count;
	unsigned int hBuffer;
	unsigned int freeMem;
	unsigned int bufSize;
	unsigned int noObjects;

	//CRenderer* pRenderer;
};