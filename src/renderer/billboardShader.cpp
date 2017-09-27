#include "billboardShader.h"
#include "renderer.h"



void CBillboardShader::getShaderHandles() {
	hCentre = pRenderer->getShaderDataHandle(hShader, "centrePos");
	hCamWorldMatrix = pRenderer->getShaderDataHandle(hShader, "camWorldMatrix");
	hBillboardSize = pRenderer->getShaderDataHandle(hShader, "size");

}


void CBillboardShader::setCentrePos(glm::vec3& centrePos) {
	pRenderer->setShaderValue(hCentre, 1, centrePos);
}

void CBillboardShader::setCamWorldMatrix(glm::mat4& matrix) {
	pRenderer->setShaderValue(hCamWorldMatrix, 1, matrix);
}

void CBillboardShader::setBillboardSize(float size) {
	pRenderer->setShaderValue(hBillboardSize, 1, size);
}