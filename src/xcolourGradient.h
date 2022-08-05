#pragma once

#include<vector>
#include<map>
#include<list>

#include "glm/glm.hpp"

struct TColour {
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
	ColourGradient(std::initializer_list<int> bytes);
	void* getData();
	void insertColour(unsigned char index, glm::i32vec4& newColour);
	void changeColour(unsigned char tabPos, glm::i32vec4& newColour);
	glm::i32vec4 getColour(int index);
	int moveTab(int oldPos, int newPos);
	void deleteTab(unsigned char tabPos);
	void rotateTabHues(float rotation);

	const std::map<int, glm::i32vec4>& getTabs();
	std::map<int, glm::i32vec4> tabs;
	std::vector<TPixel> pixels;
};