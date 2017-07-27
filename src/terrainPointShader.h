#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderer\renderShader.h"



/** A wrapper for the shader that returns terrain data at point. */
class CTerrainPointShader : public CRenderShader {
public:
	CTerrainPointShader() {};
	void getShaderHandles();
	void setSampleBase(glm::vec3& sampleBase);
	void setOffsetScale(float offsetScale);


	
	GLuint hSampleBase;
	GLuint hOffsetScale;


};