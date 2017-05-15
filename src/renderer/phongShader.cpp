#include "phongShader.h"
#include "renderer.h"

void CPhongShader::getShaderHandles() {
	pRenderer->setShader(hShader); //DO WE NEED THIS
	hNormalModelToCameraMatrix = pRenderer->getShaderDataHandle(hShader, "normalModelToCameraMatrix");
	hLightDirection = pRenderer->getShaderDataHandle(hShader, "lightDirection");
	hLightIntensity = pRenderer->getShaderDataHandle(hShader, "lightIntensity");
	hAmbientLight = pRenderer->getShaderDataHandle(hShader, "ambientLight");
	hColour = pRenderer->getShaderDataHandle(hShader, "colour");

	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
}

void CPhongShader::setNormalModelToCameraMatrix(glm::mat3 & matrix) {
	pRenderer->setShaderValue(hNormalModelToCameraMatrix, 1, matrix);
}

void CPhongShader::setLightDirection(glm::vec3 & lightDir) {
	pRenderer->setShaderValue(hLightDirection, 1, lightDir);
}

void CPhongShader::setLightIntensity(glm::vec4 lightIntensity) {
	pRenderer->setShaderValue(hLightIntensity, 1, lightIntensity);
}

void CPhongShader::setAmbientLight(glm::vec4 ambient) {
	pRenderer->setShaderValue(hAmbientLight, 1, ambient);
}

void CPhongShader::setColour(glm::vec4 & colour) {
	pRenderer->setShaderValue(hColour, 1, colour);
}

void CPhongShader::setMVP(glm::mat4 matrix) {
	pRenderer->setShaderValue(hMVP,1, matrix);
}
