
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
