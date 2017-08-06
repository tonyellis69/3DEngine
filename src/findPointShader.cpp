#include "findPointShader.h"
#include "renderer/renderer.h"

void CFindPointShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hTextureUnit = pRenderer->getShaderDataHandle(hShader, "heightField");
	
}

void CFindPointShader::setTextureUnit(unsigned int texUnit) {
	pRenderer->setShaderValue(hTextureUnit, 1, (int)texUnit);
}
