#include "grassShader.h"
#include "renderer\renderer.h"

void CGrassShader::getShaderHandles() {
	pRenderer->setShader(hShader); //DO WE NEED THIS
	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
	hTexure = pRenderer->getShaderDataHandle(hShader, "grassTex");
	hTime = pRenderer->getShaderDataHandle(hShader, "time");
}

void CGrassShader::setMVP(glm::mat4& matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}


void CGrassShader::setTextureUnit(unsigned int textureUnit) {
	pRenderer->setShaderValue(hTexure, 1, (int)textureUnit);
}

void CGrassShader::setTime(float time) {
	pRenderer->setShaderValue(hTime, 1, time);
}



