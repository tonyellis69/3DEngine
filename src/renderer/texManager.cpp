
#include <iostream>
using namespace std;

#include "texManager.h"
#include "..\SOIL.h"


CTextureManagerOGL::~CTextureManagerOGL() {
	for (size_t tex = 0; tex < textures.size(); tex++)
			delete textures[tex];
}

GLuint CTextureManagerOGL::getTexture(std::string filename) {

	if (textureStrings.empty() || textureStrings.find(filename) == textureStrings.end()) { //not found
		CRenderTexture* newTexture = new CRenderTexture;
		unsigned char* data = SOIL_load_image(filename.c_str(), &newTexture->width, &newTexture->height, &newTexture->channels, SOIL_LOAD_AUTO);

		int flags = SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y;
		if (data) {
			newTexture->handle = SOIL_create_OGL_texture(data, newTexture->width, newTexture->height, newTexture->channels, SOIL_CREATE_NEW_ID, flags); //then turn it into an OpenGL texture
			SOIL_free_image_data(data);
		}
		else {
			cerr << "\nFailed to load texture file " << filename;
			delete newTexture;
			return NULL;
		}

		addTexture(newTexture,filename);
	}

	return textureStrings[filename]->handle;
}

void CTextureManagerOGL::addTexture(CRenderTexture* texture, std::string filename) {
	textures.push_back(texture);
	textureStrings[filename] = texture;
}

CBaseTexture* CTextureManagerOGL::createTextureObject() {
	CRenderTexture* newTexture = new CRenderTexture();
	glGenTextures(1, &newTexture->handle);
	string ident = "texObj"; ident += std::to_string(internalID);
	addTexture(newTexture, ident);
	internalID++;
	return newTexture;
}

/** Create an empty OGL texure of the given size - useful for rendering to texture. */
CBaseTexture * CTextureManagerOGL::createEmptyTexture(int width, int height) {
	CRenderTexture* newTexture = new CRenderTexture();
	glGenTextures(1, &newTexture->handle);
	glBindTexture(GL_TEXTURE_2D, newTexture->handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	string ident = "tex"; ident += std::to_string(internalID);
	newTexture->channels = 4; //lazily assume rgba
	newTexture->width = width;
	newTexture->height = height;
	addTexture(newTexture, ident);
	internalID++;
	return newTexture;
}
