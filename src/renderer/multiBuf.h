#pragma once
#include <vector>
#include "glew.h"
#include "buf.h"

class CRenderer;

class CChildBuf : public CBuf {
public:
	CChildBuf() { CBuf::CBuf();  objCount = 0; nextId = 1; };
	unsigned short getNextId();
	std::vector<GLint> first;
	std::vector<GLsizei> count;
	unsigned int objCount;
	unsigned short nextId;
};

class CChildBlock {
public:
	unsigned int id;
	unsigned int start;
	unsigned int size;


};

/** Manages a renderer-based buffer for multidraw drawing. */
class CMultiBuf : public CBuf {
public:
	CMultiBuf();
	~CMultiBuf();
	void setMultiBufferSize(unsigned int bufSize);
	int getElementSize();
	void reserve(unsigned int size);
	GLint* getFirstArray();
	GLsizei* getCountArray();
	void copyBuf(CBaseBuf& srcBuf, unsigned int size);
	void setSize(unsigned int size);
	void createChildBuf();
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	unsigned int getBufHandle();
	void setMinSize(unsigned int minSize);
	unsigned int getLastId();
	

	GLint* first;
	GLsizei* count;
	unsigned int freeMem;
	unsigned int maxBufSize;
	unsigned int minBufSize; ///<Minimum space that should always be available.
	unsigned int maxObjects;
	unsigned int currentObjects;

	int elemSize;

	std::vector<CChildBuf> childBufs;
	unsigned int noChildBufs;

	int attr[4]; ///<Keeps track of the user-supplied vertex attributes;

	std::vector<CChildBlock> blocks;
	unsigned int lastId; ///<Most recently assigned id number.
};