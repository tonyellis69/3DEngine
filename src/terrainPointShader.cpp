#include "terrainPointShader.h"
#include "renderer/renderer.h"

void CTerrainPointShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hSampleBase = pRenderer->getShaderDataHandle(hShader,"sampleBase");
	hOffsetScale = pRenderer->getShaderDataHandle(hShader, "offsetScale");
}

void CTerrainPointShader::setSampleBase(glm::vec3& sampleBase) {
	pRenderer->setShaderValue(hSampleBase, 1, sampleBase);
}

void CTerrainPointShader::setOffsetScale(float offsetScale) {
	pRenderer->setShaderValue(hOffsetScale, 1, offsetScale);
}

