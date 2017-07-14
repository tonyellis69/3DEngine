#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "..\renderer\renderer.h"



/** A wrapper for the standard phong shader. */
class CGUItexShader : public CRenderShader {
public:
	CGUItexShader() {};
	void getShaderHandles();
	void setOrtho(glm::mat4 matrix);
	void setTextureUnit(unsigned int texUnit);
	void setTiling(glm::vec2& tile);
	void setOffset(glm::vec2& offset);

	GLuint hOrtho; ///<Handle to  orthographic view matrix
	GLuint hTextureUnit; ///<
	GLuint hTile;
	GLuint hOffset;
};