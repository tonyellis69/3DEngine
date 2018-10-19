#include "texGen.h"
#include "glm\gtc\matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

CTexGen::CTexGen() : frequency(1) {
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

void CTexGen::setFrequency(float freq) {
	frequency = freq;
}

void CTexGen::setTranslation(glm::vec3 & translation) {
	matrix = glm::translate(matrix, translation);
}

void CTexGen::setAngles(glm::vec3& rotationAngles) {
	matrix = glm::rotate(matrix, glm::radians(rotationAngles.x), vec3(1, 0, 0));
	matrix = glm::rotate(glm::mat4(1), glm::radians(-rotationAngles.y), vec3(0, 1, 0)) * matrix;
	matrix = glm::rotate(glm::mat4(1), glm::radians(rotationAngles.z), vec3(0, 0, 1)) * matrix;
}



void CTestTex::loadShader() {
	shader = pRenderer->createShader("texTest");
	hMatrix = shader->getUniformHandle("matrix");
}

void CTestTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hMatrix, matrix);

	pRenderer->renderToTextureQuad( *mTarget);
}


void CNoiseTex::loadShader() {
	shader = pRenderer->createShader("texNoise");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
	hMatrix = shader->getUniformHandle("matrix");
	hOctaves = shader->getUniformHandle("octaves");
	hFrequency = shader->getUniformHandle("frequency");
}

void CNoiseTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hSamplePos, samplePos);
	shader->setShaderValue(hSampleSize, sampleSize);
	shader->setShaderValue(hMatrix, matrix);
	shader->setShaderValue(hOctaves, octaves);
	shader->setShaderValue(hFrequency, frequency);

	pRenderer->renderToTextureQuad(*mTarget);
}

void CNoiseTex::setSample(glm::vec3 & pos, glm::vec3 & size) {
	samplePos = pos;
	sampleSize = size;
}

void CNoiseTex::setOctaves(int octaves) {
	this->octaves = octaves;
}



CColourTex::CColourTex() {
	palette.resize(256, 0);
}

/** Set the 1D texture to use as a colour map. */
void CColourTex::setPalette(ColourGradient& newGradient) {
	palette.setData(newGradient.getData());
}

void CColourTex::loadShader() {
	shader = pRenderer->createShader("colourTex");
	hSource = shader->getUniformHandle("source");
	hPalette = shader->getUniformHandle("palette");
}

void CColourTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	pRenderer->attachTexture1D(1, palette.handle);

	shader->setShaderValue(hSource, 0);
    shader->setShaderValue(hPalette, 1);
	pRenderer->renderToTextureQuad(*mTarget);
}


void CRidgedMultiTex::loadShader() {
	shader = pRenderer->createShader("texRidgedMulti");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
	hMatrix = shader->getUniformHandle("matrix");
	hOctaves = shader->getUniformHandle("octaves");
}


void CylinderTex::loadShader() {
	shader = pRenderer->createShader("texCylinder");
	hMatrix = shader->getUniformHandle("matrix");
	hFrequency = shader->getUniformHandle("frequency");
}

void CylinderTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hFrequency, frequency);
	shader->setShaderValue(hMatrix, matrix);
	pRenderer->renderToTextureQuad(*mTarget);
	
}


void CTurbulenceTex::loadShader() {
	shader = pRenderer->createShader("texTurbulence");
	hFrequency = shader->getUniformHandle("frequency");
	hSource = shader->getUniformHandle("source");
	hPower = shader->getUniformHandle("power");
	hRoughness = shader->getUniformHandle("roughness");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
}

void CTurbulenceTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hFrequency, frequency);
	shader->setShaderValue(hPower, power);
	shader->setShaderValue(hRoughness, roughness);
	shader->setShaderValue(hSamplePos, samplePos);
	shader->setShaderValue(hSampleSize, sampleSize);

	pRenderer->renderToTextureQuad(*mTarget);
}

void CTurbulenceTex::setPower(float power) {
	this->power = power;
}

void CTurbulenceTex::setRoughness(int roughness) {
	this->roughness = roughness;
}

void CTurbulenceTex::setSample(glm::vec3 & pos, glm::vec3 & size) {
	samplePos = pos;
	sampleSize = size;
}


void CScaleBiasTex::loadShader() {
	shader = pRenderer->createShader("texScaleBias");
	hScale = shader->getUniformHandle("scale");
	hBias = shader->getUniformHandle("bias");
	hSource = shader->getUniformHandle("source");
}

void CScaleBiasTex::setScaleBias(float scale, float bias) {
	this->scale = scale;
	this->bias = bias;
}

void CScaleBiasTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hScale, scale);
	shader->setShaderValue(hBias, bias);

	pRenderer->renderToTextureQuad(*mTarget);
}


void CAddTex::setSource2(CRenderTexture * src2) {
	source2 = src2;
}

void CAddTex::loadShader() {
	shader = pRenderer->createShader("texAdd");
	hSource = shader->getUniformHandle("source");
	hSource2 = shader->getUniformHandle("source2");
}

void CAddTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	pRenderer->attachTexture(1, source2->handle);

	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hSource2, 1);

	pRenderer->renderToTextureQuad(*mTarget);
}


void CSeamlessTex::loadShader() {
	shader = pRenderer->createShader("texSeamless");
	hSource = shader->getUniformHandle("source");
}

void CSeamlessTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);

	pRenderer->renderToTextureQuad(*mTarget);
}


void CScalePointTex::setScale(glm::vec3 & scale) {
	this->scale = scale;
}

void CScalePointTex::loadShader() {
	shader = pRenderer->createShader("texScalePoint");
	hSource = shader->getUniformHandle("source");
	hScale = shader->getUniformHandle("scale");
}

void CScalePointTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hScale, scale);

	pRenderer->renderToTextureQuad(*mTarget);
}

void CBillowTex::loadShader() {
	shader = pRenderer->createShader("texBillow");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
	hMatrix = shader->getUniformHandle("matrix");
	hOctaves = shader->getUniformHandle("octaves");
	hFrequency = shader->getUniformHandle("frequency");
	hPersistence = shader->getUniformHandle("persistence");
}

void CBillowTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hSamplePos, samplePos);
	shader->setShaderValue(hSampleSize, sampleSize);
	shader->setShaderValue(hMatrix, matrix);
	shader->setShaderValue(hOctaves, octaves);
	shader->setShaderValue(hFrequency, frequency);
	shader->setShaderValue(hPersistence, persistence);

	pRenderer->renderToTextureQuad(*mTarget);
}

void CBillowTex::setPersistence(float persistence) {
	this->persistence = persistence;
}

void CVoronoiTex::loadShader() {
	shader = pRenderer->createShader("texVoronoi");
	hFrequency = shader->getUniformHandle("frequency");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
	hDistance = shader->getUniformHandle("distance");
	hRandomHue = shader->getUniformHandle("randomHue");
}

void CVoronoiTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hFrequency, frequency);
	shader->setShaderValue(hSamplePos, samplePos);
	shader->setShaderValue(hSampleSize, sampleSize);
	shader->setShaderValue(hDistance, distance);
	shader->setShaderValue(hRandomHue, randomHue);

	pRenderer->renderToTextureQuad(*mTarget);
}

void CVoronoiTex::setDistance(bool onOff) {
	distance = onOff;
}

void CVoronoiTex::setRandomHue(bool onOff) {
	randomHue = onOff;
}

void CVoronoiTex::setSample(glm::vec3 & pos, glm::vec3 & size) {
	samplePos = pos;
	sampleSize = size;
}


void CSelectTex::loadShader() {
	shader = pRenderer->createShader("texSelect");
	hSource = shader->getUniformHandle("source1");
	hSource2 = shader->getUniformHandle("source2");
	hMap = shader->getUniformHandle("map");
	hLowerBound = shader->getUniformHandle("lowerBound");
	hUpperBound = shader->getUniformHandle("upperBound");
	hFalloff = shader->getUniformHandle("falloff");
}

void CSelectTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);

	pRenderer->attachTexture(1, source2->handle);
	shader->setShaderValue(hSource2, 1);

	pRenderer->attachTexture(2, map->handle);
	shader->setShaderValue(hMap, 2);

	shader->setShaderValue(hLowerBound, lowerBound);
	shader->setShaderValue(hUpperBound, upperBound);
	shader->setShaderValue(hFalloff, falloff);

	pRenderer->renderToTextureQuad(*mTarget);
}

void CSelectTex::setSource2(CRenderTexture * src2) {
	source2 = src2;
}

void CSelectTex::setControl(CRenderTexture * map) {
	this->map = map;
}

void CSelectTex::setBounds(float lower, float upper) {
	lowerBound = lower;
	upperBound = upper;
}

void CSelectTex::setFalloff(float falloff) {
	this->falloff = falloff;
}


void CLayerTex::loadShader() {
	shader = pRenderer->createShader("texLayer");
	hSource = shader->getUniformHandle("source");
	hSource2 = shader->getUniformHandle("source2");
}

void CLayerTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	pRenderer->attachTexture(1, source2->handle);

	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hSource2, 1);

	pRenderer->renderToTextureQuad(*mTarget);
}
