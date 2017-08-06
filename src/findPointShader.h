#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"



/** A wrapper for the shader that returns the xyz coordinate of an xz point in 
	heightfield texture. */
class CFindPointShader : public CRenderShader {
public:
	CFindPointShader() {};
	void getShaderHandles();
	void setTextureUnit(unsigned int texUnit);

	GLuint hTextureUnit;



};