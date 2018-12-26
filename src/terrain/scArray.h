#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "superChunk2.h"


/** A 3D array of superchunks. */
class CSCarry {
public:
	void setSize(int width, int height, int depth);
	int getSize() { return size; }
	glm::i32vec2 getDimensions();
	CSuperChunk2& element(int x, int y, int z);
	void rotate(glm::i32vec3 dirVec);

private:
	std::vector<CSuperChunk2> flatArray;
	glm::i32vec3 dimensions;
	glm::i32vec3 rotation; ///<Index offset due to rotation
	int size;
};