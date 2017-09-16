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
