#include "uiRectShader.h"
#include "..\renderer\renderer.h"

void CGUIrectShader::getShaderHandles() {
	//pRenderer->setShader(hShader);
	hOrtho = pRenderer->getShaderDataHandle(hShader, "orthoMatrix");
	hColour1 = pRenderer->getShaderDataHandle(hShader, "colour1");
	hColour2 = pRenderer->getShaderDataHandle(hShader, "colour2");
}

void CGUIrectShader::setOrtho(glm::mat4 matrix) {
	pRenderer->setShaderValue(hOrtho, 1, matrix);
}

void CGUIrectShader::setColour1(glm::vec4 & colour) {
	pRenderer->setShaderValue(hColour1, 1, colour);
}

void CGUIrectShader::setColour2(glm::vec4 & colour) {
	pRenderer->setShaderValue(hColour2, 1, colour);
}