#include "texShader.h"
#include "renderer.h"

void CTexShader::getShaderHandles() {
	pRenderer->setShader(hShader); //DO WE NEED THIS
	hTextureUnit = pRenderer->getShaderDataHandle(hShader,"mySampler");
	hTile = pRenderer->getShaderDataHandle(hShader, "tile");
	
	hOffset = pRenderer->getShaderDataHandle(hShader, "offset");
	

	hMVP = pRenderer->getShaderDataHandle(hShader,"mvpMatrix");

}



unsigned int CTexShader::getMVPhandle(){
	return hMVP;
}

void CTexShader::setTextureUnit( unsigned int texUnit) {
	pRenderer->setShaderValue(hTextureUnit, 1, (int)texUnit);
}

void CTexShader::setTiling( glm::vec2 & tile) {
	pRenderer->setShaderValue(hTile, 1, tile);
}

void CTexShader::setOffset( glm::vec2 & offset) {
	pRenderer->setShaderValue(hOffset, 1, offset);
}

void CTexShader::setMVP(glm::mat4 matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}
