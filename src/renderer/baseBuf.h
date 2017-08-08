#pragma once

#include "../userTypes.h"

typedef struct {
	float r, g, b, a;
} tmpRGBAtype;



class CBaseBuf {	
public:
	CBaseBuf();
	virtual void storeVertexes(void* verts, unsigned int size, unsigned int nVerts) {};
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



};