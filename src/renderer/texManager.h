#pragma once
#include <string>
#include <vector>
#include <map>

#include "glew.h"


class CBaseTexture {
public:
	int width;
	int height;

	int channels;

};

class CRenderTexture : public CBaseTexture {
public:
	CRenderTexture() {};
	unsigned int handle;
};



/** Keeps track of all textures currently loaded. It is OpenGL based. In theory another texture manager
class could be created for objects subclassed to use a different manager.*/

class CTextureManagerOGL {
public:
	CTextureManagerOGL() { internalID = 0; };
	~CTextureManagerOGL();

	GLuint getTexture(std::string filename);
	void addTexture(CRenderTexture* texture, std::string filename);
	CBaseTexture* createTextureObject();
	CBaseTexture* createEmptyTexture(int width, int height);

	std::map<std::string, CRenderTexture*> textureStrings;
	std::vector< CRenderTexture*> textures;

	int internalID;
};