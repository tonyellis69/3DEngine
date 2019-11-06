#pragma once

#include <vector> 

#include "hex.h"

/** A 2D container for CHex objects. */
class CHexArray {
public:
	CHexArray() {};
	void init(int w, int h);
	CHex& hex(int x, int y);


	std::vector<CHex> flatArray;
	int width;
	int height;
};