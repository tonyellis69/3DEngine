#pragma once

#include "glm/glm.hpp"

//#include "hex.h"




/** A class describing the element of a hex array. */
class CHexElement {
public:
	CHexElement() : content(0), blocks(0) { };
	int content;
	int cost = 0;
	glm::vec3 position = glm::vec3(0); //TO DO: not really used, might save shaders some work
	unsigned int blocks;

};

const int solidHex = 2;
const int emptyHex = 1;


const int blocksNone = 0;
const int blocksEast = 1;
const int blocksSE = 2;
const int blocksSW = 4;
const int blocksWest = 8;
const int blocksNW = 16;
const int blocksNE = 32;
const int blocksAll = 63;
const int blocksAsDoor = 54;