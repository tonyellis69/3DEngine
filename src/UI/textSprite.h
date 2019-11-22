#pragma once

#include <glm/glm.hpp>

/** Defines a rectangular block of pre-rendered text. */
class CTextSprite {
public:
	CTextSprite(int posX, int posY);


	unsigned int bufId; 
	glm::i32vec2 pagePosition;
	glm::i32vec2 size;


};