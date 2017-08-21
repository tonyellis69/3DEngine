#pragma once
#include "model.h"

//class CModel;

class CModelMulti : public CModel {
public:
	CModelMulti() : CModel() {};
	CModelMulti(glm::vec3& pos) : CModel(pos) {};
	void setMultiBufferSize(unsigned int bufSize);
	unsigned int getFreeMem();
	virtual int getElementSize() { return 0; };
	virtual GLint* getFirstArray() { return 0; };
	virtual GLsizei* getCountArray() { return 0; };
	void storeLayout(int attr1, int attr2, int attr3, int attr4);
	void drawNew();


	virtual void reserveBuf(unsigned int elementsUsed) {};

	CRenderer* pRenderer; ///<Lets models talk to renderer.
	CMultiBuf multiBuf;

};