#pragma once

#include <vector>

/** A wrapper for an OpenGL vertex buffer, and VAO state. */
class CSharedBuf {
public:
	CSharedBuf();
	void attachData(void* verts, int numVerts, int vertSize);
	void attachIndex(unsigned int* indices, unsigned int numIndices);
	void attachIndex(unsigned short* indices, unsigned int numIndices);
	void addAttribute(int attr);
	void setAttribs();
	void setVAO();
	void clearVAO();
	void setSize(unsigned int size);
	void setNumVerts(unsigned int n) {
		numVerts = n;
	}
	unsigned int getBufferHandle() {
		return hBuffer;
	}

	~CSharedBuf();

private:
	unsigned int hBuffer;
	int stride;
	unsigned int numVerts;

	unsigned int hIndex;
	int indexStride;

	unsigned int hVAO;
	std::vector<int> attribs;

	unsigned int size = 0;
};