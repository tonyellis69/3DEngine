#include "buf.h"
#include <glew.h>
#include <wglew.h>
#include "renderer.h"

CBuf::CBuf() {
	pRenderer = &CRenderer::getInstance();
	hBuffer = 0;
	hVAO = 0;
	hIndex = 0;
	noIndices = 0;
	noVerts = 0;
	nTris = 0;
	bufSize = 0;
	hInstancedBuf = 0;
	nInstancedAttribs = 0;
	instancedBuf = NULL;
	indexType = GL_UNSIGNED_SHORT;
	indexStride = sizeof(unsigned short);
	frameCount = 1;

}

void CBuf::storeVertexes(void * verts, unsigned int size, unsigned int nVerts) {
	if (hBuffer == 0) {
		glGenBuffers(1, &hBuffer);
	}


	//NB any pre-existing data is freed
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, (void*)verts, GL_STATIC_DRAW); //was static
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	noVerts = nVerts;
	bufSize = size;
	if (noVerts > 65535) {
		indexType = GL_UNSIGNED_INT;
		indexStride = sizeof(unsigned int);
	}
	else {
		indexType = GL_UNSIGNED_SHORT;
		indexStride = sizeof(unsigned short);
	}
}

/**	Merge separate arrays of vertex data into this buf. */
void CBuf::storeVertexes(vert3_t & verts, vert3_t & normals) {
	std::vector<vBuf::T3DnormVert> normVerts;
	int i = 0;
	for (auto v : verts) {
		vBuf::T3DnormVert nv;
		nv.v = v; nv.normal = normals[i++];
		normVerts.push_back(nv);
	}
	unsigned int nVerts = normVerts.size();
	storeVertexes(normVerts.data(), sizeof(vBuf::T3DnormVert) * nVerts, nVerts);
}

/**	Merge separate arrays of vertex data into this buf, including texture coordinates. */
void CBuf::storeVertexes(vert3_t & verts, vert3_t & normals, vert2_t&  texCoords) {
	std::vector<vBuf::T3DnormTexVert> normTexVerts;
	int i = 0;
	for (auto v : verts) {
		normTexVerts.push_back({ v,normals[i],texCoords[i] });
		i++;
	}
	unsigned int nVerts = normTexVerts.size();
	storeVertexes(normTexVerts.data(), sizeof(vBuf::T3DnormTexVert) * nVerts, nVerts);
}

void CBuf::storeIndex(unsigned int * indices,  unsigned int nIndices) {
	unsigned int size; void* indicesPtr; unsigned short* shortBuf = NULL;
	if (indexType == GL_UNSIGNED_SHORT) {
		size = nIndices * sizeof(unsigned short);
		shortBuf = new unsigned short[nIndices];
		std::copy(indices, indices + nIndices, shortBuf);
		indicesPtr = shortBuf;
	}
	else {
		size = nIndices * sizeof(unsigned int);
		indicesPtr = indices;
	}
	if (hIndex == 0)
		glGenBuffers(1, &hIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indicesPtr, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	noIndices = nIndices;
	frameIndices = noIndices; //default. Overrule for multi frame buffers
	if (shortBuf)
		delete[] shortBuf;
}

void CBuf::storeLayout(int attr1, int attr2, int attr3, int attr4) {
	
	if (hVAO != 0)
		glDeleteVertexArrays(1, &hVAO);

	glGenVertexArrays(1, &hVAO);

	if (hInstancedBuf)
		glBindBuffer(GL_ARRAY_BUFFER, hInstancedBuf);
	else
		glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	pRenderer->setVAO(hVAO);
	attr[0] = attr1;
	attr[1] = attr2;
	attr[2] = attr3;
	attr[3] = attr4;

	unsigned int stride = 0;
	//stride = (attr1 * sizeof(float)) + (attr2 * sizeof(float)) + (attr3 * sizeof(float))
	//	+ (attr4 * sizeof(float));
	
	int index = 0;
	while (attr[index] != 0 && index < 4) {
		stride += attr[index] * sizeof(float);
		index++;
		if (nInstancedAttribs == index)
			break;
	}


	nAttribs = 1;
	
	int pointer = 0; index = 0;
	while (attr[index] !=0 && index < 4) {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, attr[index], GL_FLOAT, GL_FALSE, stride, (void*)pointer);
		glVertexAttribDivisor(index, 0);
		
		pointer += attr[index] * sizeof(float);
		nAttribs++;
		index++;
		if (nInstancedAttribs == index)
			break;
	}


	//Any instancing attributes to take care of?
	if (nInstancedAttribs) {

		glBindBuffer(GL_ARRAY_BUFFER, hBuffer);

		stride = 0;  index = nInstancedAttribs;
		while (attr[index] != 0 && index < 4) {
			stride += attr[index] * sizeof(float);
			index++;
		}

		pointer = 0; index = nInstancedAttribs;
		while (attr[index] != 0 && index < 4) {
			glEnableVertexAttribArray(index );
			glVertexAttribPointer(index , attr[index], GL_FLOAT, GL_FALSE, stride, (void*)pointer);
			glVertexAttribDivisor(index, 1);
			pointer += attr[index - nInstancedAttribs] * sizeof(float);
			nAttribs++;
			index++;
		}
	}

	if (instancedBuf)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, instancedBuf->getIndexHandle());
	else
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	pRenderer->setVAO(0);
}

/** Create a OGL buffer of the requested size. Note that this will erase any existing buffer attached to this CBuf.*/
void CBuf::setSize(unsigned int size) {
	freeMem();
	createBuf(size);
	bufSize = size;
}

/** Change the reported size of the buffer without erasing memory. */
void CBuf::reduceReportedSize(unsigned int size) {
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
	unsigned int srcSize = srcBuf.getBufSize();
	setSize(srcSize);

	/*if (hBuffer == 0) {
		glGenBuffers(1, &hBuffer);
	}*/
	GLuint hSrcBuf = srcBuf.getBufHandle();
	glBindBuffer(GL_COPY_READ_BUFFER, hSrcBuf);
	glBindBuffer(GL_COPY_WRITE_BUFFER, hBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, srcSize);
	int attrs[4];
	srcBuf.getLayout(attrs[0], attrs[1],attrs[2],attrs[3]);
	storeLayout(attrs[0], attrs[1], attrs[2], attrs[3]);
	noVerts = srcBuf.getNoVerts();
	//bufSize = srcSize;
}

unsigned int CBuf::getBufHandle() {
	return hBuffer;
}

unsigned int CBuf::getBufSize() {
	return bufSize;
}

void CBuf::freeMem() {
	if (hBuffer != 0) {
		//glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
		glDeleteBuffers(1,&hBuffer);
		hBuffer = 0;
	}
	if (hVAO != 0) {
		glDeleteVertexArrays(1,&hVAO);
		hVAO = 0;
	}
	if (hIndex != 0) {
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
		glDeleteBuffers(1, &hIndex);
		hIndex = 0;
	}
}



void CBuf::getData(unsigned char * data, unsigned int size) {
	glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/**	Return the vertex layout. */
void CBuf::getLayout(int& attr1, int& attr2, int& attr3, int& attr4) {
	attr1 = attr[0];
	attr2 = attr[1];
	attr3 = attr[2];
	attr4 = attr[3];
}

unsigned int CBuf::getNoVerts() {
	return noVerts;
}

void CBuf::setNoVerts(unsigned int nVerts) {
	noVerts = nVerts;
}

/**	Assign a vertex buffer for instanced drawing. */
void CBuf::setInstanced(CBaseBuf & buf, int noAttribs) {
	instancedBuf = &buf;
	//get the buffer handle
	hInstancedBuf = buf.getBufHandle();
	nInstancedAttribs = noAttribs;

}

unsigned int CBuf::getNoIndices() {
	return noIndices;
}

unsigned int CBuf::getIndexHandle() {
	return hIndex;
}



CRenderer * CBuf::getRenderer() {
	return pRenderer;
}

/** Resize the buffer while preserving its contents. */
void CBuf::resizeSafe(unsigned int size) {
	//create the larger buffer.
	unsigned int hNewBuffer;
	glGenBuffers(1, &hNewBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, hNewBuffer);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//copy everything to it
	glBindBuffer(GL_COPY_READ_BUFFER, hBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, hNewBuffer);
	glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, bufSize);


	//free old buffer
	//glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
	glDeleteBuffers(1, &hBuffer);

	//make new buffer 'our' buffer

	hBuffer = hNewBuffer;
	bufSize = size;
	storeLayout(attr[0], attr[1], attr[2], attr[3]);
}



CBuf::~CBuf() {
	freeMem();
}
