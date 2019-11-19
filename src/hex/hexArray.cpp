#include "hexArray.h"

#include <algorithm>"


/** Initialise the array to the given dimensions. */
void CHexArray::init(int w, int h) {
	flatArray.clear();
	flatArray.resize(w * h);
	width = w; height = h;
	float xStep = sqrt(3);
	float yStep = 1.5f;
	bbTL = glm::vec3(0);
	bbBR = glm::vec3(0);

	glm::vec3 origin (w * 0.5f * xStep, h * 0.5f * yStep, 0.0f);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			glm::vec3 offset = glm::vec3(x * xStep, y * yStep, 0) - origin;
			offset += glm::vec3(xStep * 0.5f, 0, 0) * float(y & 1);
			getHex(x, y).position = offset;
			getHex(x, y).content = 1; //TO DO: temp!


			//TO DO: also temp - do this more elegantly!
		}

	}
	bbTL = origin * glm::vec3(-1,-1,-1);
	bbBR = origin * glm::vec3(1,1,-1);

}

/** Return the hex at the given 2D coordinates. */
CHexElement& CHexArray::getHex(int x, int y) {
	x = std::clamp(x, 0, width - 1);
	y = std::clamp(y,0, height - 1);
	return flatArray[y * width + x];
}

/** Return the worldspace position of this hex. */
glm::vec3 CHexArray::getWorldPos(CHex& hex) {
	glm::i32vec2 offset = cubeToOffset(hex);
	offset.x += width / 2;
	offset.y = -offset.y + height / 2;

	return getHex(offset.x, offset.y).position;
}
