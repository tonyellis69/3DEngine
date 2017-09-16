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

class CShader;
class CRenderMaterial : public CMaterial {
public:
	CRenderMaterial();
	void addImage(std::string filename);
	unsigned int getTexture(int texNo);
	void assign();
	void setColour(glm::vec4& colour);
	void setShader(CShader* shader);
	void setOffset(int texNo, glm::vec2& offset);
	void setTile(int texNo, glm::vec2& tile);
	void unAssign();
	CShader* getShader();

	
	
	std::vector<CTextureSlot> textures;
	glm::vec4 colour;
	//unsigned int hShader;
	CShader* shader;

protected:
	CRenderer* pRenderer;
};