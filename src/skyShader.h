#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"



/** A wrapper for the skydome shader. */
class CSkyShader : public CRenderShader {
public:
	CSkyShader() {};
	void getShaderHandles();
	void setMVP(glm::mat4& matrix);
	void setHeightColours(glm::vec3* colours);

	GLuint hSkyDomeHeightColours;
	GLuint hMVP; ///<Handle to standard texture shader MVP matrix

};