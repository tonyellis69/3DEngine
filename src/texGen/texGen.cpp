#include "texGen.h"
#include "glm\gtc\matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

CTexGen::CTexGen(TexGenType derivedType) :
	texGenType(derivedType), srcTex1(NULL), srcTex2(NULL), srcTex3(NULL)  {
	pRenderer = &CRenderer::getInstance();
	mTarget.resize(512, 512);
	//TO DO: do without magic numbers!!!!!!!!!!!!!!!!!!!!!
	count = 0;
}

CTexGen::~CTexGen() {
	//int b = 0;
}

/** Set the texture to draw to. */
void CTexGen::setTarget(CRenderTexture * newTarget) {
	//mTarget = newTarget;
}

CRenderTexture * CTexGen::getTarget() {
	return &mTarget;
}

void CTexGen::setSource(CTexGen * newSource) {
	mSource = newSource->getTarget();
	srcTex1 = newSource;
}

void CTexGen::setSource2(CTexGen * newSource) {
	source2 = newSource->getTarget();
	srcTex2 = newSource;
}

void CTexGen::setSource3(CTexGen * newSource) {
	source3 = newSource->getTarget();
	srcTex3 = newSource;

}



void CTexGen::setTranslation(glm::vec3 & translation) {
	this->translation = translation;
	buildMatrix();
}

void CTexGen::write(std::ofstream & out) {
	writeObject(matrix, out);
	writeObject(rotationAngles, out);
	writeObject(translation, out);
}

void CTexGen::read(std::ifstream & in) {
	readObject(matrix, in);
	readObject(rotationAngles, in);
	readObject(translation, in);
}

void CTexGen::setAngles(glm::vec3& rotationAngles) {
	this->rotationAngles = rotationAngles;
	buildMatrix();
}

void CTexGen::buildMatrix() {
	matrix = glm::translate(mat4(1), translation);
	matrix = glm::rotate(matrix, glm::radians(rotationAngles.x), vec3(1, 0, 0));
	matrix = glm::rotate(glm::mat4(1), glm::radians(-rotationAngles.y), vec3(0, 1, 0)) * matrix;
	matrix = glm::rotate(glm::mat4(1), glm::radians(rotationAngles.z), vec3(0, 0, 1)) * matrix;
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

	pRenderer->renderToTextureQuad(mTarget);
}

void CNoiseTex::setFrequency(float freq) {
	frequency = freq;
}

void CNoiseTex::setPersistence(float persist) {
	persistence = persist;
}

void CNoiseTex::setSampleWidth(float width) {
	sampleSize.x = width;
}

void CNoiseTex::setSampleHeight(float height) {
	sampleSize.y = height;
}

void CNoiseTex::setSample(glm::vec3 & pos, glm::vec3 & size) {
	samplePos = pos;
	sampleSize = size;
}

void CNoiseTex::setOctaves(int octaves) {
	this->octaves = octaves;
}

void CNoiseTex::compose() {
	
}

void CNoiseTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(octaves, out);
	writeObject(frequency, out);
	writeObject(persistence, out);
	writeObject(samplePos, out);
	writeObject(sampleSize, out);
}

void CNoiseTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(octaves, in);
	readObject(frequency, in);
	readObject(persistence, in);
	readObject(samplePos, in);
	readObject(sampleSize, in);
}




CColourTex::CColourTex() : CTexGen(texColour) {
	palette.resize(256, 0);
	name = "Colourise";
}

/** Set the 1D texture to use as a colour map. */
void CColourTex::setPalette(ColourGradient& newGradient) {

	palette.setData(newGradient.getData());
	colourGradient = newGradient;
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
	pRenderer->renderToTextureQuad(mTarget);
}

void CColourTex::write(std::ofstream & out) {
	int tabCount = colourGradient.tabs.size();
	writeObject(tabCount,out);
	for (auto tab : colourGradient.tabs) {
		writeObject(tab.first, out);
		writeObject(tab.second, out);
	}
}

void CColourTex::read(std::ifstream & in) {
//	readObject(colourGradient.tabs, in);
}


void CRidgedMultiTex::loadShader() {
	shader = pRenderer->createShader("texRidgedMulti");
	hSamplePos = shader->getUniformHandle("samplePos");
	hSampleSize = shader->getUniformHandle("sampleSize");
	hMatrix = shader->getUniformHandle("matrix");
	hOctaves = shader->getUniformHandle("octaves");
	hFrequency = shader->getUniformHandle("frequency");
}

void CRidgedMultiTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hSamplePos, samplePos);
	shader->setShaderValue(hSampleSize, sampleSize);
	shader->setShaderValue(hMatrix, matrix);
	shader->setShaderValue(hOctaves, octaves);
	shader->setShaderValue(hFrequency, frequency);

	pRenderer->renderToTextureQuad(mTarget);
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
	pRenderer->renderToTextureQuad(mTarget);
	
}

void CylinderTex::setFrequency(float freq) {
	this->frequency = freq;
}

void CylinderTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(frequency, out);
}

void CylinderTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(frequency, in);
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
	shader->setShaderValue(hRoughness, octaves);
	shader->setShaderValue(hSamplePos, samplePos);
	shader->setShaderValue(hSampleSize, sampleSize);

	pRenderer->renderToTextureQuad(mTarget);
}

void CTurbulenceTex::setPower(float power) {
	this->power = power;
}

void CTurbulenceTex::setOctaves(int octave) {
	this->octaves = octave;
}


void CTurbulenceTex::setSample(glm::vec3 & pos, glm::vec3 & size) {
	samplePos = pos;
	sampleSize = size;
}

void CTurbulenceTex::write(std::ofstream & out) {
	CNoiseTex::write(out);
	writeObject(power, out);
}

void CTurbulenceTex::read(std::ifstream & in) {
	CNoiseTex::read(in);
	readObject(power, in);
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

void CScaleBiasTex::setScale(float scale) {
	this->scale = scale;
}

void CScaleBiasTex::setBias(float bias) {
	this->bias = bias;
}

void CScaleBiasTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hScale, scale);
	shader->setShaderValue(hBias, bias);

	pRenderer->renderToTextureQuad(mTarget);
}

void CScaleBiasTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(scale, out);
	writeObject(bias, out);
}

void CScaleBiasTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(scale, in);
	readObject(bias, in);
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

	pRenderer->renderToTextureQuad(mTarget);
}




void CSeamlessTex::setPercentage(float percentage) {
	this->percentage = percentage;
}

void CSeamlessTex::setFalloff(float falloff) {
	this->falloff = falloff;
}

void CSeamlessTex::loadShader() {
	shader = pRenderer->createShader("texSeamless");
	hSource = shader->getUniformHandle("source");
	hFalloff = shader->getUniformHandle("falloff");
	hPercentage = shader->getUniformHandle("percentage");
}

void CSeamlessTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hFalloff, falloff);
	shader->setShaderValue(hPercentage, percentage);

	pRenderer->renderToTextureQuad(mTarget);
}

void CSeamlessTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(falloff, out);
	writeObject(percentage, out);
}

void CSeamlessTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(falloff, in);
	readObject(percentage, in);
}


void CScalePointTex::setScalePt(glm::vec3 & scale) {
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

	pRenderer->renderToTextureQuad(mTarget);
}

void CScalePointTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(scale, out);
}

void CScalePointTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(scale, in);
}

glm::vec3 & CScalePointTex::getScalePt() {
	return scale;
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

	pRenderer->renderToTextureQuad(mTarget);
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

	pRenderer->renderToTextureQuad(mTarget);
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



void CVoronoiTex::write(std::ofstream & out) {
	CNoiseTex::write(out);
	writeObject(distance, out);
	writeObject(randomHue, out);
}

void CVoronoiTex::read(std::ifstream & in) {
	CNoiseTex::read(in);
	readObject(distance, in);
	readObject(randomHue, in);
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

	pRenderer->attachTexture(2, source3->handle);
	shader->setShaderValue(hMap, 2);

	shader->setShaderValue(hLowerBound, lowerBound);
	shader->setShaderValue(hUpperBound, upperBound);
	shader->setShaderValue(hFalloff, falloff);

	pRenderer->renderToTextureQuad(mTarget);
}




void CSelectTex::setBounds(float lower, float upper) {
	lowerBound = lower;
	upperBound = upper;
}

void CSelectTex::setLowerBound(float lower) {
	lowerBound = lower;
}

void CSelectTex::setUpperBound(float upper) {
	upperBound = upper;
}

void CSelectTex::setFalloff(float falloff) {
	this->falloff = falloff;
}

void CSelectTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(lowerBound, out);
	writeObject(falloff, out);
}

void CSelectTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(lowerBound, in);
	readObject(falloff, in);
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

	pRenderer->renderToTextureQuad(mTarget);
}

CNullTex::CNullTex() {
	texGenType = texNull;
	nullTexture.resize(512, 512);

	mSource = &nullTexture;
	//mTarget = &nullTexture;
}

CGausTex::CGausTex() : CTexGen(texGaus) {
	name = "Gaus";
	size = 5;
	sigma = 3;
}

void CGausTex::loadShader() {
	shader = pRenderer->createShader("texGaus");
	hSource = shader->getUniformHandle("source");
	hSigma = shader->getUniformHandle("sigma");
	hSize = shader->getUniformHandle("blurSize");
}

void CGausTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	shader->setShaderValue(hSource, 0);

	shader->setShaderValue(hSigma, sigma);
	shader->setShaderValue(hSize, size);

	pRenderer->renderToTextureQuad(mTarget);
}

void CGausTex::setGausSize(int size) {
	this->size = size;
}

void CGausTex::setGausSigma(float sigma) {
	this->sigma = sigma;
}

void CGausTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(size, out);
	writeObject(sigma, out);
}

void CGausTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(size, in);
	readObject(sigma, in);
}
