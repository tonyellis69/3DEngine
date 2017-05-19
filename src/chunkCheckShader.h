#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"


/** A wrapper for the chunk-checking shader. */
class ChunkCheckShader : public CRenderShader {
public:
	ChunkCheckShader() {};
	void getShaderHandles();
	void setSampleCorner(glm::vec3& sampleCorner);
	void setLoDscale(float LoDscale);

	GLuint hNWsamplePos;
	GLuint hLoDscale;
};