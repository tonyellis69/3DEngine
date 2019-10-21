#include "texGen.h"
#include "glm\gtc\matrix_transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

#include<unordered_map>

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
	selectedShade = -1;
	selectionFalloff = 0.05f;
}

/** Set the 1D texture to use as a colour map. */
void CColourTex::setPalette(ColourGradient& newGradient) {

	palette.setPixelData(newGradient.getData());
	colourGradient = newGradient;
}

void CColourTex::loadShader() {
	shader = pRenderer->createShader("colourTex");
	hSource = shader->getUniformHandle("source");
	hPalette = shader->getUniformHandle("palette");
	hSelectionFalloff = shader->getUniformHandle("selectionFalloff");
	hSelectedShade = shader->getUniformHandle("selectedShade");
}

void CColourTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	pRenderer->attachTexture1D(1, palette.handle);

	shader->setShaderValue(hSource, 0);
    shader->setShaderValue(hPalette, 1);
	shader->setShaderValue(hSelectedShade, selectedShade);
	shader->setShaderValue(hSelectionFalloff, selectionFalloff);
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

/** Return the source texture colour at this point. */
glm::i32vec4 CColourTex::getSourceColour(int x, int y) {

	return mSource->getPixel(x, y);
}

void CColourTex::setSelectedShade(int shade) {
	selectedShade = shade;
}

void CColourTex::adjustFalloff(int delta) {
	selectionFalloff += float(delta) * 0.01f;
	selectionFalloff = clamp(selectionFalloff,0.001f, 0.1f);
	render();
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
	mTarget = nullTexture;
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

void CRectsTex::loadShader() {
	shader = pRenderer->createShader("texRects");
	hDepth = shader->getUniformHandle("depth");
	hVariance = shader->getUniformHandle("variance");
	hFreq = shader->getUniformHandle("freq");
	hStyle = shader->getUniformHandle("style");
	hOctaves = shader->getUniformHandle("octaves");
	hClusters = shader->getUniformHandle("clusters");

}

CRectsTex::CRectsTex() : CTexGen(texRects) { 
	name = "Rects"; 
	variance = 0.15f; depth = 3;
	frequency = 3; octaves = 4;
	style = 1; clusters = 0;
}


void CRectsTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hDepth, depth);
	shader->setShaderValue(hVariance, variance);
	shader->setShaderValue(hOctaves, octaves);
	shader->setShaderValue(hFreq, frequency);
	shader->setShaderValue(hStyle, style);
	shader->setShaderValue(hClusters, clusters);

	pRenderer->renderToTextureQuad(mTarget);
}

void CRectsTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(depth, out);
	writeObject(variance, out);
	writeObject(frequency, out);
	writeObject(octaves, out);
	writeObject(style, out);
	writeObject(clusters, out);
}

void CRectsTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(depth, in);
	readObject(variance, in);
	readObject(frequency, in);
	readObject(octaves, in);
	readObject(style, in);
	readObject(clusters, in);
}



CBlocksTex::CBlocksTex() : CTexGen(texBlocks) {
	name = "Blocks";
	stages = 2; iterations = 3; density = 1; scale = 2;
}

void CBlocksTex::loadShader() {
	shader = pRenderer->createShader("texBlocks");
	hStages = shader->getUniformHandle("stages");
	hIterations = shader->getUniformHandle("iterations");
	hDensity = shader->getUniformHandle("density");
	hScale = shader->getUniformHandle("scale");

}

void CBlocksTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hStages, stages);
	shader->setShaderValue(hIterations, iterations);
	shader->setShaderValue(hDensity, density);
	shader->setShaderValue(hScale, scale);
	
	pRenderer->renderToTextureQuad(mTarget);
}

void CBlocksTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(stages, out);
	writeObject(iterations, out);
	writeObject(density, out);
	writeObject(scale, out);
}

void CBlocksTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(stages, in);
	readObject(iterations, in);
	readObject(density, in);
	readObject(scale, in);
}


CoverTex::CoverTex() : CTexGen(texCover) {
	name = "Cover";
	iterations = 50; scale = 1;
}

void CoverTex::loadShader() {
	shader = pRenderer->createShader("texCover");
	hScale = shader->getUniformHandle("scale");
	hIterations = shader->getUniformHandle("iterations");
}

void CoverTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	shader->setShaderValue(hScale, scale);
	shader->setShaderValue(hIterations, iterations);

	pRenderer->renderToTextureQuad(mTarget);
}

void CoverTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(iterations, out);
	writeObject(scale, out);
}

void CoverTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(iterations, in);
	readObject(scale, in);
}


CutupTex::CutupTex() : CTexGen(texCutup) {
	name = "Cutup";
}

void CutupTex::loadShader() {
	shader = pRenderer->createShader("texCutup");
	hSource = shader->getUniformHandle("source");
	hSource2 = shader->getUniformHandle("source2");
}

void CutupTex::render() {
	loadShader();
	pRenderer->setShader(shader);
	pRenderer->attachTexture(0, mSource->handle);
	pRenderer->attachTexture(1, source2->handle);

	shader->setShaderValue(hSource, 0);
	shader->setShaderValue(hSource2, 1);

	pRenderer->renderToTextureQuad(mTarget);
}


CTerrainTex::CTerrainTex() : CTexGen(texTerrain) {
	name = "Terrain";
	setGridSize(100);
	showDetails = true;
	featureCount = 4; featureSize = 0.85f;
	randomSeed = 42;

	margin = 2;
	startOffset = 0.02;
	endOffset = 0.8;// 0.98;

}

void CTerrainTex::loadShader() {
	shader = pRenderer->createShader("texTerrain");
	hSource = shader->getUniformHandle("source");
	hShowDetails = shader->getUniformHandle("showDetails");
	hGridSize = shader->getUniformHandle("gridSize");
}

void CTerrainTex::render() {
	eng.seed(randomSeed);
	loadShader();
	pRenderer->setShader(shader);

	placeStartEnd();

	placeFeatures();
	plotShortestPath();
	
	uploadGridData();

	pRenderer->attachTexture(0, gridTexture.handle);
	shader->setShaderValue(hSource, 0);

	shader->setShaderValue(hShowDetails, showDetails);
	shader->setShaderValue(hGridSize, gridSize);

	pRenderer->renderToTextureQuad(mTarget);
}

/** Fill the buffer with verts representing the grid cells. */
void CTerrainTex::uploadGridData() {

	gridTexture.setArrayData(grid.data());
	
}

/** Randomly place the start and end tiles at opposite sides. */
void CTerrainTex::placeStartEnd() {
	std::fill(grid.begin(),grid.end(),0.0f);

	std::uniform_int_distribution<> gridWidth{ 0,gridSize - 1 };
	std::uniform_int_distribution<> oneOrZero{ 0,1 };

	int axis = oneOrZero(eng);
	int side = oneOrZero(eng);

	
	startPoint[axis] = gridWidth(eng); 
	startPoint[1 - axis] = side ? 0 : gridSize - 1;
	endPoint[axis] = (startPoint[axis] +  (gridSize/2)) % gridSize;         //gridWidth(eng);
	endPoint[1 -axis] = side ? gridSize - 1 : 0;

	//float 
	endPoint = i32vec2((gridSize-1) *  endOffset);
	startPoint = i32vec2((gridSize) * startOffset);

	//endPoint = i32vec2(2, 0);
	//startPoint = i32vec2(2, gridSize-1);

	grid[startPoint.x + startPoint.y*gridSize] = 1.0;
	grid[endPoint.x + endPoint.y*gridSize] = 2.0;
}

/** Place cells that either attract or repel the pathfinder. */
void CTerrainTex::placeFeatures() {
	features.clear();
	std::uniform_int_distribution<> gridWidth{ 0,gridSize-1 };
	for (int x = 0; x < featureCount; x++) {
			i32vec2 repellor(gridWidth(eng),gridWidth(eng));
			features.push_back(repellor);
			grid[repellor.x + repellor.y*gridSize] = -1.0f;
	}
}

/** Place a simple barrier for the pathfinder to negotiate. */
void CTerrainTex::placeBarrier() {
	for (int x = 3; x < 7; x++)
		grid[x + 4 * gridSize] = -1.0f;
	for (int y = 4; y < 8; y++)
		grid[7 + y * gridSize] = -1.0f;
}

/** Find the shortest path from the start to the end, and highlight those cells. */
void CTerrainTex::plotShortestPath() {
	std::uniform_real_distribution<float> rnd{ 0,0.1f }; //0.01 for minor swerve

	std::map<int, int> parents;
	parents[startPoint.x + startPoint.y * gridSize] = startPoint.x + startPoint.y * gridSize;

	std::map<int, float> weights;
	float maxWeight = 0;

	//create the 'open list' - nodes whose neighbourhood we want to explore, ordered by 
	//how easy they are to get to from the start point
	typedef std::pair<float, int> TNode; //first: cell id, second: cost
	std::priority_queue<TNode, std::vector<TNode>, std::greater<TNode>> openList;
	std::map<int, float> distSoFar;

	//add the start point to the set
	openList.emplace(0.0f, startPoint.x + startPoint.y * gridSize);
	distSoFar[startPoint.x + startPoint.y*gridSize] = 0;

	float greatestDistance = 0;
	float gridDiagonal = length(vec2(gridSize));

	//while the set of nodes to explore isn't empty... 
	while (!openList.empty()) {
		//remove the node with the smallest distance from the open list and make it current
		auto it = openList.top();
		int currentNode = it.second;
		openList.pop();

		//is it the end point? break
		if (!showDetails && currentNode == endPoint.x + endPoint.y * gridSize)
			 break;

		//for each neighbour...
		i32vec2 neighbours[4] = { {0,-1},{1,0},{0,1},{-1,0} };
		for (int n = 0; n < 4; n++) {
			i32vec2 neighbour = i32vec2(currentNode % gridSize, currentNode / gridSize)
				+ neighbours[n];
			int neighbourIndex = neighbour.x + neighbour.y * gridSize;
			if (neighbour.x >= margin && neighbour.x < gridSize-margin && neighbour.y >= margin && neighbour.y < gridSize-margin) {

				//find the greatest repulsion this neighbour suffers from nearby features, if any
				float repulsion = 0.0f;
				for (auto feature : features) {
					float featureDist = glm::distance(vec2(feature), vec2(neighbour)) / gridDiagonal; //max = 1
					float featureProximity = 1.0f - featureDist; //close = 1
					if (featureProximity < featureSize)
						featureProximity = 0.0f; //neighbour is outside feature area of effect
					repulsion = std::max(featureProximity, repulsion);
				}
				repulsion *= 0.25;

				//find the heuristic cost of this neighbour, based on its distance to the endpoint
				float heuristic = glm::distance(vec2(endPoint), vec2(neighbour)) / gridDiagonal; 
				heuristic += repulsion; //weighted by nearness to any feature
				heuristic += rnd(eng); //and a random jiggle

				float distanceToNeighbour = distSoFar[currentNode];
				float newDistance = distanceToNeighbour + heuristic; //the cost of getting to this neighbour

				//if this neighbour is a new node, or this is a shorter route to it, 
				//add it to the list of nodes to explore
				if (distSoFar.find(neighbourIndex) == distSoFar.end() || distanceToNeighbour < distSoFar[neighbourIndex]) {
					openList.emplace(newDistance, neighbourIndex);
					distSoFar[neighbourIndex] = distanceToNeighbour;

					greatestDistance = std::max(greatestDistance, newDistance);
					parents[neighbourIndex] = currentNode;
					weights[neighbourIndex] = heuristic;
					maxWeight = std::max(heuristic, maxWeight);
				}
			}
		}

	}


	if (showDetails) {
		for (auto weight : weights) {
			if (grid[weight.first] == 0)
				grid[weight.first] = weight.second;
		}
	}
	else {
		std::fill(grid.begin(), grid.end(), 0.0f);
		grid[startPoint.x + startPoint.y*gridSize] = 1.0;
		grid[endPoint.x + endPoint.y*gridSize] = 1.0;
	}

	
	int drawNode = parents[endPoint.x + endPoint.y * gridSize];
	while (drawNode != startPoint.x + startPoint.y * gridSize && drawNode != -1) {
		if (showDetails)
			grid[drawNode] = 3.0f;
		else
			grid[drawNode] = 1.0f;
		drawNode = parents[drawNode];
	}

}

void CTerrainTex::setGridSize(int size) {
	gridSize = size;
	grid.resize(gridSize * gridSize);
	gridTexture.resize(gridSize, gridSize);
}

void CTerrainTex::write(std::ofstream & out) {
	CTexGen::write(out);
	writeObject(gridSize, out);
	writeObject(showDetails, out);
	writeObject(featureCount, out);
	writeObject(featureSize, out);
	writeObject(randomSeed, out);
}

void CTerrainTex::read(std::ifstream & in) {
	CTexGen::read(in);
	readObject(gridSize, in);
	readObject(showDetails, in);
	readObject(featureCount, in);
	readObject(featureSize, in);
	readObject(randomSeed, in);
}


/** Return the start point as a fraction of the texture area. */
glm::vec2 CTerrainTex::getStartPoint() {
	vec2 halfPixel = vec2( 0.5f / gridSize);
	return vec2(startOffset) + halfPixel;// vec2(0.005);
}

/** Return the end point as a fraction of the texture area. */
glm::vec2 CTerrainTex::getEndPoint() {
	vec2 halfPixel = vec2(0.5f / gridSize);
	vec2 dist = vec2(endPoint) / float(gridSize) + halfPixel;
	return dist - getStartPoint();
}




#ifdef DIKJSTRA

void CTerrainTex::plotShortestPath() {
	//initialise all nodes
	
	std::vector<float> distances(gridSize*gridSize, 99999.0f);
	distances[startPoint.x + startPoint.y * gridSize] = 0;
	std::vector<int> parents(gridSize*gridSize, -1);
	
	//create the unexplored set
	typedef std::pair<float, int> TNode;
	
	std::priority_queue<TNode, std::vector<TNode>, greater<TNode> > queue;
	
	//add the start point to the set
	TNode startNode = { 0.0f, startPoint.x + startPoint.y*gridSize };
	queue.push(startNode);

	float greatestDistance = 0;
	//while the unexplored set isn't empty... 
	
	while (!queue.empty()) {

		//remove the node with the smallest distance from the set and make it current
		TNode currentNode = queue.top();
		queue.pop();

		//is it the end point? break
		if (currentNode.second == endPoint.x + endPoint.y * gridSize)
			break;

		//for each neighbour...
		i32vec2 neighbours[4] = { {0,-1},{1,0},{0,1},{-1,0} };
		//std::random_shuffle(std::begin(neighbours), std::end(neighbours));
		for (int n = 0; n < 4; n++) {
			i32vec2 neighbour = i32vec2(currentNode.second % gridSize, currentNode.second /gridSize)
				+ neighbours[n];
			int neighbourIndex = neighbour.x + neighbour.y * gridSize;
			if (neighbour.x >= 0 && neighbour.x < gridSize && neighbour.y >= 0 && neighbour.y < gridSize
				&& grid[neighbourIndex] >= 0) {
				float distanceWeight = 1;// glm::distance(vec2(endPoint), vec2(neighbour));
			
				for (auto feature : features) {
					float repulsion = 1.0f / glm::distance(vec2(feature), vec2(neighbour));
					cerr << "\ndistance weight " << distanceWeight << " repulsion " << repulsion;
					distanceWeight += repulsion;
				}

				float newDistance = currentNode.first + distanceWeight;

				if (newDistance < distances[neighbourIndex]) {
					distances[neighbourIndex] = newDistance;
					queue.push({ newDistance, neighbourIndex });
					parents[neighbourIndex] = currentNode.second;
					greatestDistance = std::max(greatestDistance, newDistance);
				}
			}
		}

	}
	
	//colour tiles
	for (int index = 0; index < gridSize * gridSize; index++) {
		if (distances[index] < 99999 && grid[index] == 0)
			 grid[index] = (float)distances[index] / (greatestDistance+1);
	}
	
	int drawNode = parents[endPoint.x + endPoint.y * gridSize];
	while (drawNode != startPoint.x + startPoint.y * gridSize && drawNode != -1) {
		grid[drawNode] = 0.99f;
		drawNode = parents[drawNode];
	}
	
}	

#endif
