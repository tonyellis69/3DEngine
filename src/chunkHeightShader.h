#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"



/** A wrapper for the chunk heightfield creator shader. */
class ChunkHeightShader : public CRenderShader {
public:
	ChunkHeightShader() {};
	void getShaderHandles();
	void setTopDownMatrix(glm::mat4& matrix);

	GLuint hTopDownMatrix;


};