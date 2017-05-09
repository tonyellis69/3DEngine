#pragma once
#include <string>
#include "glew.h"




/** Defines the textures and other items used to shade a model. */

class CMaterial {
public:
	CMaterial() {};
	~CMaterial() {};
	virtual void setTexure(std::string filename) {};
	virtual void assign() {};
	virtual void setColour(glm::vec4& colour) {}
};




