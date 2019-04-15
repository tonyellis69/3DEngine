#pragma once

#include <glm/glm.hpp>

#include "renderTexture.h"

/** A class defining the colours, textures and shaders needed to create a
	particular material. */
class CMaterial2 {
public:
	CMaterial2() : ambient({ 0.2f }, { 0.2f }, { 0.2f }, { 1.0f }),
					diffuse({ 0.8f }, { 0.8f }, { 0.8f }, { 1.0f }),
					specular({0.5f }, { 0.5f }, { 0.5f }, { 1.0f }),
					shininess(32.0f)
	{}		
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	float shininess;

	CRenderTexture* texture1;


};