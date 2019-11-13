#include "scArray.h"

#include <glm/gtx/common.hpp>
#include <glm/gtc/integer.hpp>

#include <glm/gtx/integer.hpp>

using namespace glm;


void CSCarry::setSize(int width, int height, int depth) {
	size = width * height * depth;
	flatArray.resize(size);
	dimensions = i32vec3(width, height, depth);
	rotation = i32vec3(0);
}

glm::i32vec2 CSCarry::getDimensions() {
	return dimensions;
}

CSuperChunk2 & CSCarry::element(int x, int y, int z) {
	x = (x + rotation.x) % dimensions.x;
	y = (y + rotation.y) % dimensions.y;
	z = (z + rotation.z) % dimensions.z;

	int index = x + (y * dimensions.x * dimensions.z) + (z * dimensions.x);
	return flatArray[index];
}

void CSCarry::rotate(glm::i32vec3 dirVec) {
	rotation += dirVec;

	if (rotation.x < 0)
		rotation.x = dimensions.x - 1;
	if (rotation.y < 0)
		rotation.y = dimensions.y - 1;
	if (rotation.z < 0)
		rotation.z = dimensions.z - 1;


	rotation.x  %= dimensions.x;
	rotation.y %= dimensions.y;
	rotation.z %= dimensions.z;
}

glm::i32vec3 CSCarry::getRotatedIndex(const glm::i32vec3& origIndex) {
	return (origIndex + rotation) % dimensions;
}

glm::i32vec3 CSCarry::getInvRotatedIndex(const glm::i32vec3& origIndex) {
	i32vec3 inv = origIndex - rotation;

	if (inv.x < 0)
		inv.x += dimensions.x;
	if (inv.y < 0)
		inv.y += dimensions.y;
	if (inv.z < 0)
		inv.z += dimensions.z;


	return inv % dimensions;
}


