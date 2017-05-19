#include "chunkCheckShader.h"
#include "renderer/renderer.h"

void ChunkCheckShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hNWsamplePos = pRenderer->getShaderDataHandle(hShader, "nwSamplePos");
	hLoDscale = pRenderer->getShaderDataHandle(hShader, "LoDscale");
	
}

void ChunkCheckShader::setSampleCorner(glm::vec3 & sampleCorner) {
	pRenderer->setShaderValue(hNWsamplePos, 1, sampleCorner);
}

void ChunkCheckShader::setLoDscale(float LoDscale) {
	pRenderer->setShaderValue(hLoDscale, 1, LoDscale);
}
