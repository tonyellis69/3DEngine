#pragma once

#include <vector>

#include "../userTypes.h"

#include "glm\glm.hpp"

typedef struct {
	float r, g, b, a;
} tmpRGBAtype;



using  vert3_t = std::vector<glm::vec3>;
class CBaseBuf {	
	
public:
	CBaseBuf();
	virtual void storeVertexes(void* verts, unsigned int size, unsigned int nVerts) {};
	virtual void storeVertexes(vert3_t& verts, vert3_t& normals) {};
	virtual void storeIndex(unsigned int * indices, unsigned int nIndices) {};
	virtual void setSize(unsigned int size) {};
	virtual void copyBuf(CBaseBuf& srcBuf, unsigned int size) {};
	virtual unsigned int getBufHandle() { return 0; };
	virtual unsigned int getBufSize() { return 0; };
	virtual void freeMem() {};
	virtual void setMinSize(unsigned int minSize) {};
	virtual void  storeLayout(int attr1, int attr2, int attr3, int attr4) {};
	virtual void reserve(unsigned int size) {};
	virtual unsigned int getLastId() { return 0; };
	virtual void draw() {};
	virtual void deleteBlock(unsigned int id) {};
	virtual void setBlockColour(unsigned int id, tmpRGBAtype& colour) {};
	virtual void getData(unsigned char* data, unsigned int size) {};
	virtual void getLayout(int& attr1, int& attr2, int& attr3, int& attr4) {};
	virtual unsigned int getNoVerts() { return 0; };
	virtual void setNoVerts(unsigned int nVerts) {};
	virtual void setInstanced(CBaseBuf& buf, int noAttribs) {};
	virtual unsigned int getNoIndices() { return 0; };
	virtual unsigned int getIndexHandle() { return 0; };
	virtual void getElementData(const unsigned int id, unsigned int& firstVert, unsigned int& vertCount,  int& childBufNo) {};
		
	



};