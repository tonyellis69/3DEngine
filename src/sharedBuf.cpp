#include "sharedBuf.h"


#include <glew.h>

CSharedBuf::CSharedBuf() {
	//glGenBuffers(1, &hBuffer);
	hBuffer = 0;
	hIndex = 0;
	hVAO = 0;
}



void CSharedBuf::attachData(void* verts, int numVerts, int vertSize) {
	if (hBuffer == 0)
		glGenBuffers(1, &hBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVerts * vertSize, verts, GL_STATIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	this->numVerts = numVerts;
}

void CSharedBuf::attachIndex(unsigned int* indices, unsigned int numIndices) {
	if (hIndex == 0)
		glGenBuffers(1, &hIndex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);

	if (numIndices < 0xffff) {
		unsigned short* shortBuf = new unsigned short[numIndices];
		std::copy(indices, indices + numIndices, shortBuf);
		indexStride = sizeof(unsigned short int);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * indexStride, (void*)shortBuf, GL_STATIC_DRAW);
		delete shortBuf;
	}
	else 
	////TO DO: doesn't work! Need the number of verts to make this decision, which we don't yet have.

	{
		indexStride = sizeof(unsigned int);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * indexStride, indices, GL_STATIC_DRAW);
	}
	
	//NB NO GL ERROR HERE

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CSharedBuf::attachIndex(unsigned short* indices, unsigned int numIndices) {
	if (hIndex == 0)
		glGenBuffers(1, &hIndex);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);

	indexStride = sizeof(unsigned short int);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * indexStride, indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CSharedBuf::addAttribute(int attr) {
	attribs.push_back(attr);
}

void CSharedBuf::setAttribs() {
	stride = 0;
	for (auto attr : attribs)
		stride += attr * sizeof(float);

	if (hVAO == 8)
		int b = 0;

	if (hVAO != 0)
		glDeleteVertexArrays(1, &hVAO);
	glGenVertexArrays(1, &hVAO);

	glBindVertexArray(hVAO);

	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);


	int index = 0;
	int pointer = 0;
	for (auto attrib : attribs) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, attrib, GL_FLOAT, GL_FALSE, stride, (void*)pointer);
		glVertexAttribDivisor(index, 0);
		pointer += attrib * sizeof(float);
		index++;
	}




	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	attribs.clear();
}

void CSharedBuf::setVAO() {
	glBindVertexArray(hVAO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);

}

void CSharedBuf::clearVAO() {
	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CSharedBuf::setSize(unsigned int size) {
	glGenBuffers(1, &hBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	hIndex = 0;
	hVAO = 0;
}


CSharedBuf::~CSharedBuf() {

	glDeleteVertexArrays(1, &hVAO);
	hVAO = 0;

	glDeleteBuffers(1, &hBuffer);
	hBuffer = 0;


	glDeleteBuffers(1, &hIndex);
	hIndex = 0;
}


