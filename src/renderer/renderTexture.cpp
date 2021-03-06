
#include "glew.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "..\external\stb_image_write.h"

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
	isData = false;
	 channels = 4; //lazily assume rgba
	width = 128;
	height = 128;
	
	unsigned char* chequerBoardPixels = getChequePattern();


	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, chequerBoardPixels);
	delete chequerBoardPixels;
}

CRenderTexture::CRenderTexture(int w, int h) {
	resize(w, h);
}

/** Create a 1D texture.*/
CRenderTexture::CRenderTexture(int w) {
	resize(w, 0);
}

CRenderTexture::~CRenderTexture() {
	glDeleteTextures(1, &handle);
}

/** Resize this texture, freeing any previously used memory. */
//TO DO: careful! We may not want to set mipmapping to GL_NEAREST.
void CRenderTexture::resize(int w, int h) {
	channels = 4; //lazily assume rgba
	width = w;
	height = h;
	isData = false;
	glDeleteTextures(1, &handle);
	glGenTextures(1, &handle);
	if (h > 0) {
		unsigned char* chequerBoardPixels = getChequePattern();
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, chequerBoardPixels); //GL_FLOAT was GL_UNSIGNED_BYTE
		delete chequerBoardPixels;
		return;
	}
	
	//1D? fill with greyscale gradient, probably what's wanted.
	unsigned char* pixels = new unsigned char[w * 4];
	int b = 0;
	for (int p = 0; p < width; p++) {
		b = p * 4;
		pixels[b] =  (p / /*(float)*/width) * 255;
		pixels[b + 1] = pixels[b + 2] = pixels[b];  pixels[b + 3] = 255;
	}
	glBindTexture(GL_TEXTURE_1D, handle);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //was GL_LINEAR, this fixes problem of grey creeping between black and white in colour gradients
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, width, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	delete pixels;
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
/*
void CRenderTexture::createRGBA(unsigned char * data, int w, int h) {
	handle = SOIL_create_OGL_texture(data, w, h, SOIL_LOAD_RGBA, handle, 0);
	width = w;
	height = h;
	channels = 4;
}
*/
/** Create a single channel texture from data. */
void CRenderTexture::createGreyscale(unsigned char * data, int w, int h) {
	//handle = SOIL_create_OGL_texture(data, w, h, SOIL_LOAD_L, handle, 0);
	width = w;
	height = h;
	channels = 1;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
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

void CRenderTexture::setPixelData(void * pixels) {
	if (height > 0) {
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		return;
	}
	glBindTexture(GL_TEXTURE_1D, handle);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

/** Turn this texture into an array texture, and store data in it. */
void CRenderTexture::setArrayData(void * data) {
	isData = true;
	if (height > 0) {
		glBindTexture(GL_TEXTURE_2D, handle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED,
			GL_FLOAT, data);
		return;
	}
	glBindTexture(GL_TEXTURE_1D, handle);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, width, 0, GL_RED,
		GL_FLOAT, data);
}

/** Return pixels for a pattern to fill the current texture.*/
unsigned char * CRenderTexture::getChequePattern() {
	int noChecks = 32;
	int xStep = width / noChecks;
	int yStep = height / noChecks;

	unsigned char* pixels = new unsigned char[width * height * channels];
	memset(pixels, 255, width * height * channels);
	if (xStep == 0 || yStep == 0)
		return pixels;
	int p = 0;
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			unsigned char b = (x / xStep) % 2;
			b = b ^ (y / yStep) % 2;
			b = b * 255;
			pixels[p] = pixels[p + 1] = pixels[p + 2] = b; pixels[p + 3] = 32;
			p += 4;
		}
	}
	return pixels;
}


CRenderTexture & CRenderTexture::operator=(const CRenderTexture & other) {
	if (&other == this)
		return *this;
	width = other.width;
	height = other.height;
	channels = other.channels;

	GLenum target;
	if (height > 0) 
		target = GL_TEXTURE_2D;
	else 
		target = GL_TEXTURE_1D;
	
	GLuint hTempFrameBuffer;
	glGenFramebuffers(1, &hTempFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, hTempFrameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, other.handle, 0);
	glBindTexture(target, handle);

	glCopyTexImage2D(target, 0, GL_RGBA, 0, 0, width, height, 0);
	//TO DO: GL_RGBA is assumed. If want to copy other formats I need to check for them.
	glBindTexture(target,0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	return *this;
}

/** Save this texture as a PNG image file.*/
void CRenderTexture::savePNG(std::string filepath) {
	std::vector<unsigned char> data(width * height * channels);
	glBindTexture(GL_TEXTURE_2D, handle);
	if (channels == 1) //kludge for saving font bitmaps
		glGetTexImage(GL_TEXTURE_2D, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, &data[0]);
	else
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
	//SOIL_save_image (filepath.c_str(), SOIL_SAVE_TYPE_BMP,width,height,channels,data.data() );

	stbi_write_png(filepath.c_str(), width, height, channels, data.data(), width * channels);
}

/** Resize this texture while preserving its contents. */
void CRenderTexture::resizeSafe(int w, int h) {
	//create new texture
	CRenderTexture newTexture(w, h);
	
	//newTexture = *this;

	GLuint hTempFrameBuffer;
	glGenFramebuffers(1, &hTempFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, hTempFrameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, handle, 0);
	glBindTexture(GL_TEXTURE_2D, newTexture.handle);

	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0,0,0,0,0, width,height);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	width = w;
	height = h;
	unsigned int oldHandle = handle;
	handle = newTexture.handle;
	newTexture.handle = oldHandle;
	//when newTexture is destroyed on exit, it actually destroys old texture.
}


