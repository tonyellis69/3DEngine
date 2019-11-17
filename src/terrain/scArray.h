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
	CSuperChunk2* element(int x, int y, int z);
	void rotate(glm::i32vec3 dirVec);
	glm::i32vec3 getRotatedIndex(const glm::i32vec3& origIndex);

	glm::i32vec3 getInvRotatedIndex(const glm::i32vec3& origIndex);


	std::vector<CSuperChunk2> flatArray;
private:
	glm::i32vec3 dimensions;
	glm::i32vec3 rotation; ///<Index offset due to rotation
	int size;
};