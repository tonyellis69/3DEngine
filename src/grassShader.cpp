#include "grassShader.h"
#include "renderer\renderer.h"

void CGrassShader::getShaderHandles() {
	pRenderer->setShader(hShader); //DO WE NEED THIS
	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
}

void CGrassShader::setMVP(glm::mat4& matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}




