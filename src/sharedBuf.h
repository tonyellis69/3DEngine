#pragma once

#include <vector>

#include <glew.h>

/** A wrapper for an OpenGL vertex buffer, and VAO state. */
class CSharedBuf {
public:
	CSharedBuf();
	void attachData(void* verts, int numVerts, int vertSize);
	void attachIndex(std::vector<unsigned int>& indices, unsigned int numIndices);
	void attachIndex(std::vector<unsigned short>& indices, unsigned int numIndices);
	void addAttribute(int attr);
	void setAttribs();
	void setVAO();
	void clearVAO();
	void setSize(unsigned int size);

	template <typename T>
	std::vector<T> readIndex() {
		GLint size = 0;

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		int elements = size / indexStride;
		std::vector<T> result(elements);

		glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, (void*)result.data());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		return result;
	}

	template <typename T>
	std::vector<T> readVerts() {
		GLint size = 0;

		glBindBuffer(GL_ARRAY_BUFFER, hBuffer);
		glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		int elements = size / stride;
		std::vector<T> result(elements);

		glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, (void*)result.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		return result;
	}


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