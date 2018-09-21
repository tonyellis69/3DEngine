#pragma once

#include "glm\glm.hpp"

class CBaseTexture {
public:
	virtual glm::uvec4 getPixel(int x, int y) { return glm::uvec4(0); };
	virtual void resize(int w, int h) {};
	virtual void createRGBA(unsigned char* data, int w, int h) {};
	virtual void createGreyscale(unsigned char* data, int w, int h) {};
	virtual void setData(void* pixels) {};


	int width;
	int height;

	int channels;

};