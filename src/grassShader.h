#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"



/** A wrapper for the grass drawing shader. */
class CGrassShader : public CRenderShader {
public:
	CGrassShader() {};
	void getShaderHandles();
	void setMVP(glm::mat4& matrix);
	void setTextureUnit(unsigned int textureUnit);


	GLuint hMVP; ///<Handle to standard texture shader MVP matrix
	GLuint hTexure; 
};