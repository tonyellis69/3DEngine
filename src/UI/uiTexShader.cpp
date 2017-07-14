#include "uiTexShader.h"
#include "..\renderer\renderer.h"

void CGUItexShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hOrtho = pRenderer->getShaderDataHandle(hShader, "orthoMatrix");
	hTextureUnit = pRenderer->getShaderDataHandle(hShader, "textureUnit");
	hTile = pRenderer->getShaderDataHandle(hShader, "tile");
	hOffset = pRenderer->getShaderDataHandle(hShader, "offset");

}



void CGUItexShader::setOrtho(glm::mat4 matrix) {
	pRenderer->setShaderValue(hOrtho, 1, matrix);
}
void CGUItexShader::setTextureUnit(unsigned int texUnit) {
	pRenderer->setShaderValue(hTextureUnit, 1, (int)texUnit);
}

void CGUItexShader::setTiling(glm::vec2 & tile) {
	pRenderer->setShaderValue(hTile, 1, tile);
}

void CGUItexShader::setOffset(glm::vec2 & offset) {
	pRenderer->setShaderValue(hOffset, 1, offset);
}



