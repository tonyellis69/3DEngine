#pragma once

#include "glm\glm.hpp"

#include "baseTexture.h"
//#include "renderer.h"
#include <string>

class CRenderer;
class CRenderTexture : public CBaseTexture {
public:
	CRenderTexture();
	CRenderTexture(int w, int h);
	CRenderTexture(int w);
	~CRenderTexture();
	void resize(int w, int h);
	glm::uvec4 getPixel(int x, int y);
//	void createRGBA(unsigned char* data, int w, int h);
	void createGreyscale(unsigned char * data, int w, int h);
	void clear();
	void setPixelData(void* pixels);
	void setArrayData(void * data);
	unsigned char* getChequePattern();
	CRenderTexture& operator=(const CRenderTexture& other);
	void savePNG(std::string filepath);
	void resizeSafe(int w, int h);

	unsigned int handle;
	bool isData;
};

