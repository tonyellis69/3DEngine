#pragma once

#include <vector> 

#include "hexElement.h"
#include "hex.h"

/** A 2D container for CHexElement objects. */
class CHexArray {
public:
	CHexArray() {};
	void init(int w, int h);
	CHexElement& getHex(int x, int y);
	glm::vec3 getWorldPos(CHex& hex);


	std::vector<CHexElement> flatArray;
	int width;
	int height;

	glm::vec3 bbTL;
	glm::vec3 bbBR;
};