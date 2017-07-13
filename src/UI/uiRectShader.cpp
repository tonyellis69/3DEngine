#include "uiRectShader.h"
#include "..\renderer\renderer.h"

void CGUIrectShader::getShaderHandles() {
	//pRenderer->setShader(hShader);
	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
}



void CGUIrectShader::setMVP(glm::mat4 matrix) {
	pRenderer->setShaderValue(hMVP, 1, matrix);
}
