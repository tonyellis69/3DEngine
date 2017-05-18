#pragma once
#include <string>
#include "glew.h"

#include <glm/glm.hpp>
#include "shader.h"

/** Defines the textures and other items used to shade a model. */
class CShader;
class CMaterial {
public:
	CMaterial() {};
	~CMaterial() {};
	virtual void addImage(std::string filename) {};
	virtual unsigned int getTexture(int texNo) { return 0; };
	virtual void assign() {};
	virtual void setColour(glm::vec4& colour) {};
	virtual void setShader(CShader* shader) {};
	virtual void setOffset(int texNo, glm::vec2& offset) {};
	virtual void setTile(int texNo, glm::vec2& tile) {};
	virtual void unAssign() {};
	virtual CShader* getShader() { return 0; };

	TStandardShaderType currentShaderType;
};




