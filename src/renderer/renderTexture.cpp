
#include "glew.h"

#include "renderer.h"
#include "renderTexture.h"

#include <iostream> //for cerr

/** Constructor for user-created default textures. Creating via Renderer hereby deprecated. */
CRenderTexture::CRenderTexture()  {
	pRenderer = &CRenderer::getInstance();
	handle = 0;
	//create a simple 2x2 texture with a default pattern.
	//DON'T register with textureManager, which is starting to look redundant.

	//TO DO: the below will NOT work with textures declared as members of CBaseApp
	//because engine hasn't initialised renderer yet!

	if (!pRenderer->initialised)
		return;

	 channels = 4; //lazily assume rgba
	width = 2;
	 height = 2;
	float pixels[] = {	0.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,1.0f,
						1.0f, 1.0f, 1.0f,1.0f,   0.0f, 0.0f, 0.0f,1.0f };

	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, pixels); 
	
}

CRenderTexture::CRenderTexture(int w, int h) {
	resize(w, h);
}

CRenderTexture::~CRenderTexture() {
	glDeleteTextures(1, &handle);
}

/** Resize this texture, freeing any previously used memory. */
//TO DO: careful! We may not want to set mipmapping to GL_NEAREST.
void CRenderTexture::resize(int w, int h) {
	glDeleteTextures(1, &handle);
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL); //GL_FLOAT was GL_UNSIGNED_BYTE
	channels = 4; //lazily assume rgba
	width = w;
	height = h;
}

glm::uvec4  CRenderTexture::getPixel(int x, int y) {
	unsigned char data[4];
	GLuint hTempFrameBuffer;
	glGenFramebuffers(1, &hTempFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, hTempFrameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, handle, 0);
	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, (void*)data);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm::uvec4 returnData;
	returnData[0] = data[0];
	returnData[1] = data[1];
	returnData[2] = data[2];
	returnData[3] = data[3];

	return returnData;
}

/** Create an RGBA texture from data. */
void CRenderTexture::createRGBA(unsigned char * data, int w, int h) {
	handle = SOIL_create_OGL_texture(data, w, h, SOIL_LOAD_RGBA, handle, 0);
	width = w;
	height = h;
	channels = 4;
}

/** Create a single channel texture from data. */
void CRenderTexture::createGreyscale(unsigned char * data, int w, int h) {
	//handle = SOIL_create_OGL_texture(data, w, h, SOIL_LOAD_L, handle, 0);
	width = w;
	height = h;
	channels = 1;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width, height, 0, GL_LUMINANCE,
		GL_UNSIGNED_BYTE, data);
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void CRenderTexture::clear() {
	resize(width, height);
}


