#pragma once

#include "renderer.h"
#include "material.h"

class CTextureSlot {
public:
	GLuint hTexture; ///<OpenGL texture handle.
	glm::vec2 tile;
	glm::vec2 offset; ///<Texture coordinate offset.
	GLuint textureUnit; ///<OpenGL texture unit no.
};

class CRenderMaterial : public CMaterial {
public:
	void addImage(std::string filename);
	unsigned int getTexture(int texNo);
	void assign();
	void setColour(glm::vec4& colour);
	void setShader(unsigned int shader);
	void setOffset(int texNo, glm::vec2& offset);
	void setTile(int texNo, glm::vec2& tile);

	CRenderer* pRenderer;
	
	std::vector<CTextureSlot> textures;
	glm::vec4 colour;
	unsigned int hShader;
};