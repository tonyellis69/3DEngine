#include "skyShader.h"
#include "renderer\renderer.h"

void CSkyShader::getShaderHandles() {
	pRenderer->setShader(hShader); //DO WE NEED THIS
	hMVP = pRenderer->getShaderDataHandle(hShader, "mvpMatrix");
	hSkyDomeHeightColours = pRenderer->getShaderDataHandle(hShader,"heightColour");
}

void CSkyShader::setMVP(glm::mat4& matrix)
{
	pRenderer->setShaderValue(hMVP, 1, matrix);
}

void CSkyShader::setHeightColours(glm::vec3 * colours)
{
	pRenderer->setShaderValue(hSkyDomeHeightColours, 4, colours[0]);

}


