#include "renderShader.h"
#include "renderer.h"
#include <fstream>

CRenderShader::CRenderShader() {
	noFeedbackSettings = 0;
	//pRenderer = &renderer;// &CRenderer::getInstance();
}

void CRenderShader::load(shaderType shader, std::string shaderFile) {
	renderer.loadShader(shader, shaderFile);
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
	hShader = renderer.attachShaders();
}

void CRenderShader::setFeedbackData(unsigned int nVars) {
	renderer.setFeedbackData(hShader, nVars, feedbackVaryings);
	noFeedbackSettings = nVars;
}

void CRenderShader::link() {
	hShader = renderer.linkShaders(hShader);
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
//	getShaderHandles();
}

unsigned int CRenderShader::getUniformHandle(std::string name) {
	return renderer.getShaderDataHandle(hShader, name);

}

void CRenderShader::setShaderValue(unsigned int matrixHandle, glm::mat3 & matrix) {
	renderer.setShaderValue(matrixHandle, 1, matrix);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, glm::vec2 & vector) {
	renderer.setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, glm::i32vec2& vector) {
	renderer.setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, glm::vec3 & vector) {
	renderer.setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int vecHandle, const glm::vec4 & vector) {
	renderer.setShaderValue(vecHandle, 1, vector);
}

void CRenderShader::setShaderValue(unsigned int intHandle, int value) {
	renderer.setShaderValue(intHandle, 1, value);
}



void CRenderShader::setShaderValue(unsigned int matrixHandle, glm::mat4 & matrix) {
	renderer.setShaderValue(matrixHandle, 1, matrix);

}

void CRenderShader::setShaderValue(unsigned int floatHandle, float value) {
	renderer.setShaderValue(floatHandle, 1, value);
}

void CRenderShader::setTextureUnit(unsigned int samplerHandle, int textureUnit)
{
	renderer.setShaderValue(samplerHandle, 1, (int)textureUnit);

}

void CRenderShader::setShaderValue(unsigned int textureHandle, CBaseTexture& texture) {
	renderer.attachTexture(5, texture); //TO DO : Magic number!
	renderer.setShaderValue(textureHandle, 1, (int)5);
};


void CRenderShader::loadFiles() {
	load(vertex, vertFile);

	std::ifstream geomExists(geomFile);
	if (geomExists) {
		load(geometry, geomFile);
	}

	std::ifstream fragExists(fragFile);
	if (fragExists) {
		load(frag, fragFile);
	}

}




