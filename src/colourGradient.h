#pragma once

#include<vector>

#include "glm/glm.hpp"

struct TColour {
	unsigned char index;
	glm::i32vec4 colour;
};

struct TPixel {
	unsigned char r, g, b, a;
};

/** A class for managing colour palettes based on linear interpolation
	between adjustable value. */
class ColourGradient {
public:
	ColourGradient();
	void* getData();
	void insertColour(unsigned int index, glm::i32vec4 newColour);

	TColour getNextEntry(unsigned char index);

	std::vector<TColour> colours;
	std::vector<TPixel> pixels;
};