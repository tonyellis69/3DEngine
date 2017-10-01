#include "renderShader.h"
#include "renderer.h"
#include <fstream>

CRenderShader::CRenderShader() {
	noFeedbackSettings = 0;
	pRenderer = &CRenderer::getInstance();
}

void CRenderShader::load(shaderType shader, std::string shaderFile) {
	pRenderer->loadShader(shader, shaderFile);
	if (shader == vertex)
		vertFile = shaderFile;
	if (shader == geometry)
		geomFile = shaderFile;
	if (shader == frag)
		fragFile = shaderFile;

}

/**	Load, compile and link a shader using the given filename. */
void CRenderShader::create(std::string shaderName) {
	vertFile = shaderName + ".vert";
	geomFile = shaderName + ".geom";
	fragFile = shaderName + ".frag";

	loadFiles();
	attach();
	link();
}

/**	Load, compile and link a feedback shader. */
void CRenderShader::create(std::string shaderName, int nFeedbacks) {

	vertFile = shaderName + ".vert";
	geomFile = shaderName + ".geom";
	fragFile = shaderName + ".frag";

	loadFiles();
	attach();

	setFeedbackData(nFeedbacks);
	link();
}

void CRenderShader::attach() {
	hShader = pRenderer->attachShaders();
}

void CRenderShader::setFeedbackData(unsigned int nVars) {
	pRenderer->setFeedbackData(hShader, nVars, feedbackVaryings);
	noFeedbackSettings = nVars;
}

void CRenderShader::link() {
	hShader = pRenderer->linkShaders(hShader);
}

unsigned int CRenderShader::getShaderHandle() {
	return hShader;
}

void CRenderShader::recompile() {
	loadFiles();
	attach();
	if (noFeedbackSettings > 0) {
		setFeedbackData(noFeedbackSettings);
	}
	link();
	getShaderHandles();
}

unsigned int CRenderShader::getUniformHandle(std::string name) {
	return pRenderer->getShaderDataHandle(hShader, name);

}

void CRenderShader::setShaderValue(unsigned int matrixHandle, glm::mat3 & matrix) {
	pRenderer->setShaderValue(matrixHandle, 1, matrix);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, glm::vec2 & vector) {
	pRenderer->setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, glm::vec3 & vector) {
	pRenderer->setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, glm::vec4 & vector) {
	pRenderer->setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int intHandle, int value) {
	pRenderer->setShaderValue(intHandle, 1, value);
}



void CRenderShader::setShaderValue(unsigned int matrixHandle, glm::mat4 & matrix) {
	pRenderer->setShaderValue(matrixHandle, 1, matrix);
}

void CRenderShader::setShaderValue(unsigned int floatHandle, float value) {
	pRenderer->setShaderValue(floatHandle, 1, value);
}

void CRenderShader::loadFiles() {
	load(vertex, vertFile);

	ifstream geomExists(geomFile);
	if (geomExists) {
		load(geometry, geomFile);
	}

	ifstream fragExists(fragFile);
	if (fragExists) {
		load(frag, fragFile);
	}

}

void CRenderShader::getDataHandles() {
	//get any default handles here
}


CShader * CRenderShader::getThisShader() {
	return this;
}


