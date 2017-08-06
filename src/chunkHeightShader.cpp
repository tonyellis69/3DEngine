
#include "renderer/renderer.h"
#include "chunkHeightShader.h"

void ChunkHeightShader::getShaderHandles() {
	pRenderer->setShader(hShader);
	hTopDownMatrix = pRenderer->getShaderDataHandle(hShader,"topDownMatrix");

}

void ChunkHeightShader::setTopDownMatrix(glm::mat4& matrix) {
	pRenderer->setShaderValue(hTopDownMatrix, 1, matrix);
}

