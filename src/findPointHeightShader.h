#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"



/** A wrapper for a shader that iteratively finds the highest terrain level
	at a given xz coordinate. */
class CFindPointHeightShader : public CRenderShader {
public:
	CFindPointHeightShader() {};
	void getShaderHandles();
	void setCurrentY( float yValue);
	void setSamplePosition(glm::vec3 samplePos);
	void setSampleScale(float sampleScale);
	void setChunkLocaliser(glm::vec3 chunkLocaliser);

	GLuint hCurrentY;
	GLuint hChunkSamplePosition;
	GLuint hSampleScale;
	GLuint hChunkLocaliser;



};