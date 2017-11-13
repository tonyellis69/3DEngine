#pragma once

#include "glm\glm.hpp"

class CBaseTexture {
public:
	virtual glm::uvec4 getPixel(int x, int y) { return glm::uvec4(0); };
	virtual void resize(int w, int h) {};
	virtual void create(unsigned char* data, int w, int h) {};

	int width;
	int height;

	int channels;

};