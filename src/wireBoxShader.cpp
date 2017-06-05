#include "wireBoxShader.h"
#include "renderer\renderer.h"

void CWireBoxShader::getShaderHandles() {
	hColour = pRenderer->getShaderDataHandle(hShader, "colour");
	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
}

void CWireBoxShader::setColour(glm::vec4 & colour) {
	pRenderer->setShaderValue(hColour, 1, colour);
}

void CWireBoxShader::setMVP(glm::mat4 & matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}
