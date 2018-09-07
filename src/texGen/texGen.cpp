#include "texGen.h"
#include "glm\gtc\matrix_transform.hpp"


CTexGen::CTexGen() {
	pRenderer = &CRenderer::getInstance();

	//create target quad
	vBuf::T2DtexVert corners[4];
	corners[0].v = glm::vec2(-0.5, 0.5); //A
	corners[1].v = glm::vec2(0.5, 0.5); //B  0.5,0
	corners[2].v = glm::vec2(-0.5, -0.5); //C  0,0.5
	corners[3].v = glm::vec2(0.5, -0.5); //D  1,1

	corners[0].tex = glm::vec2(0);
	corners[1].tex = glm::vec2(1, 0);
	corners[2].tex = glm::vec2(0, 1);
	corners[3].tex = glm::vec2(1, 1);

	unsigned int index[6] = { 2,3,0,0,3,1 };
	targetQuad.storeVertexes(corners, sizeof(corners), 4);
	targetQuad.storeIndex(index, 6);
	targetQuad.storeLayout(2, 2, 0, 0);
}

/** Set the texture to draw to. */
void CTexGen::setTarget(CRenderTexture * newTarget) {
	mTarget = newTarget;
}

void CTexGen::setSource(CRenderTexture * newSource) {
	mSource = newSource;
}



void CTestTex::loadShader() {
	shader = pRenderer->createShader("texTest");
	hOrthoMatrix = shader->getUniformHandle("orthoMatrix");
}

void CTestTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hOrthoMatrix, ortho);

	pRenderer->renderToTextureQuad( *mTarget);
}


void CNoiseTex::loadShader() {
	shader = pRenderer->createShader("texNoise");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
	hOrthoMatrix = shader->getUniformHandle("orthoMatrix");
	hOctaves = shader->getUniformHandle("octaves");
}

void CNoiseTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hSamplePos, glm::vec3(2,1,0));
	shader->setShaderValue(hSampleSize, glm::vec3(4, 4,0));
	shader->setShaderValue(hOrthoMatrix, ortho);
	shader->setShaderValue(hOctaves,1);

	pRenderer->renderToTextureQuad(*mTarget);
}


void CColourTex::loadShader() {
	shader = pRenderer->createShader("colourTex");
	hSource = shader->getUniformHandle("source");
	hPalette = shader->getUniformHandle("palette");
}

void CColourTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mSource->handle);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_1D, palette.handle);

//	pRenderer->attachTexture(0, mSource->handle);
//	shader->setTextureUnit(hSource, 0);
//	pRenderer->attachTexture(1, palette.handle);
//	shader->setTextureUnit(hPalette, 1);

	shader->setShaderValue(hSource, 0);
    shader->setShaderValue(hPalette, 1);
	pRenderer->renderToTextureQuad(*mTarget);
	
}
