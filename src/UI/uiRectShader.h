#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "..\renderer\renderer.h"



/** A wrapper for the standard phong shader. */
class CGUIrectShader : public CRenderShader {
public:
	CGUIrectShader() {};
	void getShaderHandles();
	void setMVP(glm::mat4 matrix);

	GLuint hMVP; ///<Handle to standard texture shader MVP matrix

};