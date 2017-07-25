#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "..\renderer\renderer.h"



/** A wrapper for the standard phong shader. */
class CGUIrectShader : public CRenderShader {
public:
	CGUIrectShader() {};
	void getShaderHandles();
	void setOrtho(glm::mat4 matrix);
	void setColour1(glm::vec4& colour);
	void setColour2(glm::vec4& colour);

	GLuint hOrtho; ///<Handle to  orthographic view matrix
	GLuint hColour1;
	GLuint hColour2;
};