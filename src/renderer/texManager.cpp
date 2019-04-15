
#include <iostream>
using namespace std;

#include "texManager.h"
//#include "..\SOIL.h"

#include "../external/stb_image.h"

#include "renderer.h"

CTextureManagerOGL::~CTextureManagerOGL() {
	for (size_t tex = 0; tex < textures.size(); tex++)
			delete textures[tex];
}

CBaseTexture* CTextureManagerOGL::getTexture(std::string filename) {
	//TO DO: temporarily commented out while I try to remove dependency on soil.h
	
	if (textureStrings.empty() || textureStrings.find(filename) == textureStrings.end()) { //not found
		CRenderTexture* newTexture = new CRenderTexture;
	//	unsigned char* data = SOIL_load_image(filename.c_str(), &newTexture->width, &newTexture->height, &newTexture->channels, SOIL_LOAD_AUTO);
		unsigned char* data = stbi_load(filename.c_str(), &newTexture->width, &newTexture->height, &newTexture->channels, 0);

		//int flags = SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y;
		if (data) {
			//newTexture->handle = SOIL_create_OGL_texture(data, newTexture->width, newTexture->height, newTexture->channels, SOIL_CREATE_NEW_ID, flags); //then turn it into an OpenGL texture
			//newTexture->handle = SOIL_create_OGL_texture(data, newTexture->width, newTexture->height, newTexture->channels, SOIL_CREATE_NEW_ID, flags); //then turn it into an OpenGL texture

			//glGenTextures(1, &newTexture->handle);
			glBindTexture(GL_TEXTURE_2D, newTexture->handle);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newTexture->width, newTexture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);








			//SOIL_free_image_data(data);
		}
		else {
			cerr << "\nFailed to load texture file " << filename;
			delete newTexture;
			return NULL;
		}

		addTexture(newTexture,filename);
	}
	
	return textureStrings[filename];
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
	CRenderTexture* newTexture = new CRenderTexture(width, height);
	string ident = "tex"; ident += std::to_string(internalID);
	addTexture(newTexture, ident);
	internalID++;
	return newTexture;
}

