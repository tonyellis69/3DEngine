#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"

#include "renderer\texManager.h"


/** A wrapper for the chunk-skinning shader. */
class ChunkShader : public CRenderShader {
public:
	ChunkShader() {};
	void getShaderHandles();
	void setChunkCubeSize(float cubeSize);
	void setChunkLoDscale(float LoDscale);
	void setChunkSamplePos(glm::vec3& samplePos);
	void setChunkTriTable(CBaseTexture& dataTexture);
	void setChunkTerrainPos(glm::vec3 terrainPos);
	void setSamplesPerCube(float sampleScale);

	
	GLuint hChunkCubeSize;
	GLuint hChunkLoDscale;
	GLuint hChunkSamplePos;
	GLuint hChunkTriTable;
	GLuint hChunkTerrainPos;
	GLuint hSamplesPerCube;

};