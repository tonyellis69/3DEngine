#pragma once

//#include "shader.h"
#include <glew.h>
#include <glm/glm.hpp>
#include "renderShader.h"



/** A wrapper for the standard texture shader. */
class CMultiTexShader : public CRenderShader {
public:
	CMultiTexShader() {};
	void getShaderHandles();
	unsigned int getMVPhandle();
	void setTextureUnit(unsigned int texNo, unsigned int texUnit);
	void setTiling(unsigned int texNo, glm::vec2& tile);
	void setOffset(unsigned int texNo, glm::vec2& offset);
	void setMVP(glm::mat4 matrix);

	GLuint hTextureUnit[16]; ///<Handle to standard texture shader texture units;
	GLuint hTile[16]; ///<Handle to standard texture shader texture tiling
	GLuint hOffset[16]; ///<Handle to standard texture shader texture tiling
	
	
	GLuint hMVP; ///<Handle to standard texture shader MVP matrix
};