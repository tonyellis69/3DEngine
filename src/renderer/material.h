#pragma once
#include <string>
#include "glew.h"

#include <glm/glm.hpp>


/** Defines the textures and other items used to shade a model. */

class CMaterial {
public:
	CMaterial() {};
	~CMaterial() {};
	virtual void addImage(std::string filename) {};
	virtual unsigned int getTexture(int texNo) { return 0; };
	virtual void assign() {};
	virtual void setColour(glm::vec4& colour) {};
	virtual void setShader(unsigned int shader) {};
	virtual void setOffset(int texNo, glm::vec2& offset) {};
	virtual void setTile(int texNo, glm::vec2& tile) {};
};




