#include "hexArray.h"


/** Initialise the array to the given dimensions. */
void CHexArray::init(int w, int h) {
	flatArray.clear();
	flatArray.resize(w * h);
	width = w; height = h;
	float xStep = sqrt(3);
	float yStep = 1.5f;

	glm::vec3 origin (w * 0.5f * xStep, h * 0.5f * yStep, 0.0f);
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			glm::vec3 offset = glm::vec3(x * xStep, y * yStep, 0) - origin;
			offset += glm::vec3(xStep * 0.5f, 0, 0) * float(y & 1);
			hex(x, y).position = offset;
		}

	}
}

/** Return the hex at the given 2D coordinates. */
CHex& CHexArray::hex(int x, int y) {
	return flatArray[y * width + x];
}
