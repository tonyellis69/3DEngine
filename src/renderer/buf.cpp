#include "buf.h"
#include <glew.h>
#include <wglew.h>


CBuf::CBuf() {
	hBuffer = 0;
	hVAO = 0;
	hIndex = 0;
	nTris = 0;
	bufSize = 0;

}

void CBuf::storeVertexes(void * verts, unsigned int size, unsigned int nVerts) {
	if (hBuffer == 0) {
		glGenBuffers(1, &hBuffer);
	}
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, (void*)verts, GL_DYNAMIC_DRAW); //was static
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	noVerts = nVerts;
	bufSize = size;
}

void CBuf::storeIndex(unsigned short * indices, unsigned int size, unsigned int nIndices) {
	if (hIndex == 0)
		glGenBuffers(1, &hIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)indices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	noIndices = nIndices;
}

void CBuf::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	if (hVAO == 0)
		glGenVertexArrays(1, &hVAO);
	glBindVertexArray(hVAO);

	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);

	unsigned int stride = (attr1 * sizeof(float)) + (attr2 * sizeof(float)) + (attr3 * sizeof(float))
		+ (attr4 * sizeof(float));

	nAttribs = 1;

	int pointer = 0;
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, attr1, GL_FLOAT, GL_FALSE, stride, (void*)pointer);

	if (attr2) {
		pointer += attr1 * sizeof(float);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, attr2, GL_FLOAT, GL_FALSE, stride, (void*)pointer);
		nAttribs++;
	}

	if (attr3) {
		pointer += attr2 * sizeof(float);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, attr3, GL_FLOAT, GL_FALSE, stride, (void*)pointer);
		nAttribs++;
	}

	if (attr4) {
		pointer += attr3 * sizeof(float);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, attr4, GL_FLOAT, GL_FALSE, stride, (void*)pointer);
		nAttribs++;
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void CBuf::setSize(unsigned int size) {
	if (hBuffer == 0) 
		glGenBuffers(1, &hBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_READ);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	bufSize = size;
}

void CBuf::copyBuf(CBaseBuf & srcBuf, unsigned int size) {
	GLuint hSrcBuf = srcBuf.getBufHandle();
	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glBindBuffer(GL_COPY_WRITE_BUFFER, hBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size);

}

unsigned int CBuf::getBufHandle() {
	return hBuffer;
}

unsigned int CBuf::getBufSize() {
	return bufSize;
}



CBuf::~CBuf() {
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);

	glDeleteBuffers(1, &hBuffer);
}
