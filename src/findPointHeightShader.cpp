#include "findPointHeightShader.h"
#include "renderer/renderer.h"

void CFindPointHeightShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hCurrentY = pRenderer->getShaderDataHandle(hShader, "currentY");
	hChunkSamplePosition = pRenderer->getShaderDataHandle(hShader, "chunkSamplePosition");
	hSampleScale = pRenderer->getShaderDataHandle(hShader, "sampleScale");
	hChunkLocaliser = pRenderer->getShaderDataHandle(hShader, "chunkLocaliser");
}

void CFindPointHeightShader::setCurrentY(float yValue) {
	pRenderer->setShaderValue(hCurrentY, 1, yValue);
}

void CFindPointHeightShader::setSamplePosition(glm::vec3 samplePos) {
	pRenderer->setShaderValue(hChunkSamplePosition, 1, samplePos);
}


void CFindPointHeightShader::setSampleScale(float sampleScale) {
	pRenderer->setShaderValue(hSampleScale, 1, sampleScale);
}

void CFindPointHeightShader::setChunkLocaliser(glm::vec3 chunkLocaliser) {
	pRenderer->setShaderValue(hChunkLocaliser, 1, chunkLocaliser);
}

