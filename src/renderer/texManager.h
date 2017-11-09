#pragma once
#include <string>
#include <vector>
#include <map>

#include "glew.h"
#include "glm\glm.hpp"

#include "renderTexture.h"

/** Keeps track of all textures currently loaded. It is OpenGL based. In theory another texture manager
class could be created for objects subclassed to use a different manager.*/

class CTextureManagerOGL {
public:
	CTextureManagerOGL() { internalID = 0; };
	~CTextureManagerOGL();

	CBaseTexture* getTexture(std::string filename);
	void addTexture(CRenderTexture* texture, std::string filename);
	CBaseTexture* createTextureObject();
	CBaseTexture* createEmptyTexture(int width, int height);

	std::map<std::string, CRenderTexture*> textureStrings;
	std::vector< CRenderTexture*> textures;

	int internalID;
};