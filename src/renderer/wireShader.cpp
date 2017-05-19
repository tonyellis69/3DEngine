#include "wireShader.h"
#include "renderer.h"

void CWireShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
	hScale = pRenderer->getShaderDataHandle(hShader, "scale");
	hColour = pRenderer->getShaderDataHandle(hShader, "colour");
}

void CWireShader::setScale(glm::vec3 & scale) {
	pRenderer->setShaderValue(hScale, 1, scale);
}

void CWireShader::setColour(glm::vec4 & colour) {
	pRenderer->setShaderValue(hColour, 1, colour);
}

void CWireShader::setMVP(glm::mat4& matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}
