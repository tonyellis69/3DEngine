#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderShader.h"


/** A wrapper for a prototype wireframe shader. */
class CWireShader : public CRenderShader {
public:
	CWireShader() {};
	void getShaderHandles();
	void setScale(glm::vec3& scale);
	void setColour(glm::vec4& colour);
	void setMVP(glm::mat4& matrix);

	GLuint hScale;
	GLuint hColour;

	GLuint hMVP;
};