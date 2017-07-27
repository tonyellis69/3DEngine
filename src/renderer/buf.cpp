#include "buf.h"
#include <glew.h>
#include <wglew.h>
#include "renderer.h"

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
	glBufferData(GL_ARRAY_BUFFER, size, (void*)verts, GL_STATIC_DRAW); //was static
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	noVerts = nVerts;
	bufSize = size;
}

void CBuf::storeIndex(unsigned short * indices, unsigned int size, unsigned int nIndices) {
	if (hIndex == 0)
		glGenBuffers(1, &hIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	noIndices = nIndices;
}

void CBuf::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	if (hVAO != 0)
		glDeleteVertexArrays(1, &hVAO);

	glGenVertexArrays(1, &hVAO);

	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBindVertexArray(hVAO);


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

/** Create a OGL buffer of the requested size. Note that this will erase any existing buffer attached to this CBuf.*/
void CBuf::setSize(unsigned int size) {
	freeMem();
	createBuf(size);
	bufSize = size;
}

void CBuf::createBuf(unsigned int size) {	
	if (hBuffer)
		glDeleteBuffers(1, &hBuffer);
	glGenBuffers(1, &hBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);  //READ IS DEADLY
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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

void CBuf::freeMem() {
	if (hBuffer != 0) {
		glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
		glDeleteBuffers(1,&hBuffer);
	}
	if (hVAO != 0) {
		glDeleteVertexArrays(1,&hVAO);
	}
}

void CBuf::setDrawMode(TdrawMode mode) {
	if (mode == drawPoints)
		drawMode = rDrawPoints;
	if (mode == drawLines)
		drawMode = rDrawLines;
	if (mode == drawTris)
		drawMode = rDrawTris;
	if (mode == drawLinesAdjacency)
		drawMode = rDrawLinesAdjacency;
	if (mode == drawLinesStrip)
		drawMode = rDrawLineStrip;
	if (mode == drawTriStrip)
		drawMode = rDrawTriStrip;
	if (mode == drawLineLoop)
		drawMode = rDrawLineLoop;
}

void CBuf::getData(unsigned char * data, unsigned int size) {
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



CBuf::~CBuf() {
//	freeMem();
}
