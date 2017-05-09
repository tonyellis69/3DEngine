#pragma once

#include "renderer.h"
#include "material.h"


class CRenderMaterial : public CMaterial {
public:
	void setTexure(std::string filename);
	void assign();
	void setColour(glm::vec4& colour);

	CRenderer* pRenderer;
	GLuint hTexture; ///<OpenGL texture handle.
	glm::vec4 colour;
};