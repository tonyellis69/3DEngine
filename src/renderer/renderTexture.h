#pragma once

#include "glm\glm.hpp"

#include "baseTexture.h"
//#include "renderer.h"


class CRenderer;
class CRenderTexture : public CBaseTexture {
public:
	CRenderTexture();
	CRenderTexture(int w, int h);
	~CRenderTexture();
	void resize(int w, int h);
	glm::uvec4 getPixel(int x, int y);
	void create(unsigned char* data, int w, int h);

	unsigned int handle;
	CRenderer* pRenderer;	
};

