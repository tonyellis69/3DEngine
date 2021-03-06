#pragma once
#include <vector>
#include <map>
#include "glew.h"
#include "buf.h"

//class CRenderer;

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
	unsigned int arrayFirst;
	unsigned int arrayCount;
	unsigned int blockStart;
	unsigned int blockSize;
	unsigned int blockIdx;	
	bool unAssign;

};

/** Manages a renderer-specific structure for storing verts for multidraw drawing.
	Data is stored in one or more child buffers, created as needed.
	Each child buffer is divided into blocks of memory storing individual items. 
	Every block is identifiable by a unique id.*/
class CMultiBuf : public CBuf {
public:
	CMultiBuf();
	~CMultiBuf();
	void setMultiBufferSize(unsigned int bufSize);
	int getElementSize();
	void reserve(unsigned int size);
	void copyBuf(CBaseBuf& srcBuf, unsigned int size);
	void setSize(unsigned int size);
	void createChildBuf();
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	unsigned int getBufHandle();
	void setMinSize(unsigned int minSize);
	unsigned int getLastId();
	void draw();
	void deleteBlock(unsigned int id);
	void copyBlock(unsigned int id, char* buf);
	unsigned int getBlockSize(unsigned int id);
	void getElementData(const unsigned int id, unsigned int& firstVert, unsigned int& vertCount,  int& childBufNo);
	void setInstanced(CBaseBuf & buf, int noAttribs);
	

	//GLint* first;
	//GLsizei* count;
	unsigned int freeMem;
	unsigned int maxBufSize;
	unsigned int minBufSize; ///<Minimum space that should always be available.
	unsigned int currentObjects;

	int elemSize;

	std::vector<CChildBuf> childBufs;
	unsigned int noChildBufs;

	int attr[4]; ///<Keeps track of the user-supplied vertex attributes;

	std::map<unsigned int, CChildBlock> blocks;
	unsigned int lastId; ///<Most recently assigned id number.

	std::multimap<unsigned int, unsigned int> freeBlocks;

private:
	unsigned int getFreeBlock(unsigned int size);
	void copyToFreeBlock(unsigned int hSrcBuf, unsigned int freeBlock, unsigned int size);
	void copyToNewBlock(unsigned int hSrcBuf, unsigned int size);



	int tmp;
};