#pragma once

#include <vector>
#include <memory>

#include "sharedBuf.h"

/** A smart vertex buffer storage class. */
class CBuf2 {
public:
	CBuf2();
	void setVAO();
	void clearVAO();
	void setSize(unsigned int size);
	void freeMem();
	unsigned int getBufferHandle();

	template <typename V, typename... T>
	void storeVerts(std::vector<V>& verts, T... t) {
		freeMem();
		numVerts = verts.size();
		processParam( t...);

		storeVerts(verts.data(), verts.size(), sizeof(V));
	}

	template <typename H, typename... T>
	void processParam(H h, T... t) {
		processParam(h);
		processParam(t...);
	}

	void processParam(int attr) {
		addAttributeSize(attr);
	}

	void processParam(std::vector<unsigned short>& index) {
		storeIndexShort(index);
	}

	void processParam(std::vector<unsigned int>& index) {
		storeIndexInt(index);
	}

	int numElements = 0; ///<For indexed buffers, the total indices

	unsigned int err = 0;

private:
	void storeVerts(void* data, unsigned int numVerts, int vertSize);
	void storeIndexShort(std::vector<unsigned short>& index);
	void storeIndexInt(std::vector<unsigned int>& index);
	void addAttributeSize(int attr);

	std::shared_ptr<CSharedBuf> sharedBuf;

	unsigned int numVerts;

};