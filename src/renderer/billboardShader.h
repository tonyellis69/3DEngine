#pragma once


#include <glew.h>
#include <glm/glm.hpp>
#include "renderShader.h"



/** A wrapper for the standard billboard shader. */
class CBillboardShader : public CRenderShader {
public:
	CBillboardShader() {};
	void getShaderHandles();

	void setCentrePos(glm::vec3 & centrePos);
	void setCamWorldMatrix(glm::mat4 & matrix);
	void setBillboardSize(float size);
	

	GLuint hCentre; ///<Handle for position of billboard centre.
	GLuint hCamWorldMatrix;
	GLuint hBillboardSize;

};