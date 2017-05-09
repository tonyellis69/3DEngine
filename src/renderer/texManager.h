#pragma once
#include <string>
#include <map>

#include "glew.h"

/** Keeps track of all textures currently loaded. It is OpenGL based. In theory another texture manager
	class could be created for objects subclassed to use a different manager.*/

class CTextureOGL {
public:
	CTextureOGL() {};
	int width;
	int height;
	unsigned int handle;
	int channels;


};

class CTextureManagerOGL {
public:
	CTextureManagerOGL() {};
	~CTextureManagerOGL() {};

	GLuint getTexture(std::string filename);

	std::map<std::string, CTextureOGL> textures;
};