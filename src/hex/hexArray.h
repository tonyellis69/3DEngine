#pragma once

#include <vector> 

#include "hexElement.h"

/** A 2D container for CHexElement objects. */
class CHexArray {
public:
	CHexArray() {};
	void init(int w, int h);
	CHexElement& hex(int x, int y);


	std::vector<CHexElement> flatArray;
	int width;
	int height;

	glm::vec3 bbTL;
	glm::vec3 bbBR;
};