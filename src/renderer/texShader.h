#pragma once

//#include "shader.h"
#include <glew.h>
#include <glm/glm.hpp>
#include "renderShader.h"



/** A wrapper for the standard texture shader. */
class CTexShader : public CRenderShader {
public:
	CTexShader() {};
	void getShaderHandles();
	unsigned int getMVPhandle();
	void setTextureUnit(unsigned int texUnit);
	void setTiling( glm::vec2& tile);
	void setOffset (glm::vec2& offset);
	void setMVP(glm::mat4 matrix);

	GLuint hTextureUnit; ///<Handle to standard texture shader texture units;
	GLuint hTile; ///<Handle to standard texture shader texture tiling
	GLuint hOffset; ///<Handle to standard texture shader texture tiling
	
	
	GLuint hMVP; ///<Handle to standard texture shader MVP matrix
};