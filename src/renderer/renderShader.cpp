#include "renderShader.h"
#include "renderer.h"
#include <fstream>

void CRenderShader::load(shaderType shader, std::string shaderFile) {
	pRenderer->loadShader(shader, shaderFile);

}

/**	Load, compile and link a shader using the given filename. */
void CRenderShader::create(std::string shaderName) {
	string fileName = shaderName + ".vert";
	load(vertex, fileName);

	fileName = shaderName + ".geom";
	ifstream geomExists(fileName);
	if (geomExists) {
		load(geometry, fileName);
	}

	fileName = shaderName + ".frag";
	ifstream fragExists(fileName);
	if (fragExists) {
		load(frag, fileName);
	}

	link();
}

void CRenderShader::link() {
	hShader = pRenderer->linkShaders();
}

unsigned int CRenderShader::getShaderHandle() {
	return hShader;
}

void CRenderShader::getDataHandles() {
	//get any default handles here
}


CShader * CRenderShader::getThisShader() {
	return this;
}
