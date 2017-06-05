#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"

/** A wrapper for the wire box shader. */
class CWireBoxShader : public CRenderShader {
public:
	CWireBoxShader() {};
	void getShaderHandles();
	void setColour(glm::vec4& colour);
	void setMVP(glm::mat4& matrix);

	unsigned int hMVP;
	unsigned int hColour;

};