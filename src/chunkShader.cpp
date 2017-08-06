#include "chunkShader.h"
#include "renderer/renderer.h"

void ChunkShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hChunkCubeSize = pRenderer->getShaderDataHandle(hShader,"cubeSize");
	hChunkLoDscale = pRenderer->getShaderDataHandle(hShader, "LoDscale");
	hChunkSamplePos = pRenderer->getShaderDataHandle(hShader, "samplePos");
	hChunkTriTable = pRenderer->getShaderDataHandle(hShader, "triTableTex");
	hChunkTerrainPos = pRenderer->getShaderDataHandle(hShader, "terrainPos");
	hSamplesPerCube = pRenderer->getShaderDataHandle(hShader, "samplesPerCube");
	
}

void ChunkShader::setChunkCubeSize(float cubeSize) {
	pRenderer->setShaderValue(hChunkCubeSize, 1, cubeSize);
}

void ChunkShader::setChunkLoDscale(float LoDscale) {
	pRenderer->setShaderValue(hChunkLoDscale, 1, LoDscale);
}

void ChunkShader::setChunkSamplePos(glm::vec3 & samplePos) {
	pRenderer->setShaderValue(hChunkSamplePos, 1, samplePos);
}

void ChunkShader::setChunkTriTable(CBaseTexture& dataTexture) {
	//CRenderTexture* tex = (CRenderTexture*) &dataTexture;
		
	pRenderer->attachTexture(5, dataTexture); //TO DO : Magic number!
	pRenderer->setShaderValue(hChunkTriTable, 1, (int)5);
}

void ChunkShader::setChunkTerrainPos(glm::vec3 terrainPos) {
	pRenderer->setShaderValue(hChunkTerrainPos, 1, terrainPos);
}

void ChunkShader::setSamplesPerCube(float sampleScale) {
	pRenderer->setShaderValue(hSamplesPerCube, 1, sampleScale);
}
