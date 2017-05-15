#include "texShader.h"
#include "renderer.h"

void CTexShader::getShaderHandles() {
	pRenderer->setShader(hShader); //DO WE NEED THIS
	hTextureUnit[0] = pRenderer->getShaderDataHandle(hShader,"mySampler1");
	hTextureUnit[1] = pRenderer->getShaderDataHandle(hShader, "mySampler2");
	hTile[0] = pRenderer->getShaderDataHandle(hShader, "tile1");
	hTile[1] = pRenderer->getShaderDataHandle(hShader, "tile2");
	hOffset[0] = pRenderer->getShaderDataHandle(hShader, "offset1");
	hOffset[1] = pRenderer->getShaderDataHandle(hShader, "offset2");

	hMVP = pRenderer->getShaderDataHandle(hShader,"mvpMatrix");

}

/*
CShader * CTexShader::getThisShader(){
	return this;
} */

unsigned int CTexShader::getMVPhandle(){
	return hMVP;
}

void CTexShader::setTextureUnit(unsigned int texNo, unsigned int texUnit) {
	pRenderer->setShaderValue(hTextureUnit[texNo], 1, (int)texUnit);
}

void CTexShader::setTiling(unsigned int texNo, glm::vec2 & tile) {
	pRenderer->setShaderValue(hTile[texNo], 1, tile);
}

void CTexShader::setOffset(unsigned int texNo, glm::vec2 & offset) {
	pRenderer->setShaderValue(hOffset[texNo], 1, offset);
}

void CTexShader::setMVP(glm::mat4 matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}
