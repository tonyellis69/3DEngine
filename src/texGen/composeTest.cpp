#include "composeTest.h"

using namespace glm;

ComposeTest::~ComposeTest() {
	for (auto texGen : texGens)
		delete texGen;
	for (auto target : targets)
		delete target;
}

void ComposeTest::initTex() {
	//sample1.resize(512, 512);
//	sample2.resize(512, 512);
	//sample3.resize(512, 512);
	//sample4.resize(512, 512);
	//composedTex.resize(512, 512);
	//colouredTex.resize(512, 512);

	colourise = true;
	//octaves = 1;
	//frequency = 1;
	angle = vec3(0);
	textureSize = 512;
	stackPosition = 0;
}

void ComposeTest::compose() {

	//currentTexGen->setTarget(&composedTex);
	for (auto texGen : texGens)
		texGen->render();

}

CTexGen * ComposeTest::createNoiseTex() {
	CNoiseTex* texGen = new CNoiseTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createRidgedMFTex() {
	CRidgedMultiTex* texGen = new CRidgedMultiTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createCylinderTex() {
	CylinderTex* texGen = new CylinderTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createTurbulenceTex(CTexGen* source) {
	CTurbulenceTex* texGen = new CTurbulenceTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source);
	return texGen;
}

CTexGen * ComposeTest::createColouriseTex(CTexGen* source, ColourGradient* colourGradient) {
	CColourTex* colourTex = new CColourTex();
	texGens.push_back(colourTex);
	colourTex->setTarget(createTarget());
	colourTex->setPalette(*colourGradient);
	colourTex->setSource(source);
	/////////////currentTexGen = colourTex;
	return colourTex;
}

CTexGen * ComposeTest::createScaleBiasTex(CTexGen * source) {
	CScaleBiasTex* texGen = new CScaleBiasTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source);
	return texGen;
}

CTexGen * ComposeTest::createScalePointTex(CTexGen * source) {
	CScalePointTex* texGen = new CScalePointTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source);
	return texGen;
}

CTexGen * ComposeTest::createAddTex(CTexGen * source1, CTexGen * source2) {
	CAddTex* texGen = new CAddTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source1);
	texGen->setSource2(source2);
	return texGen;
}

CTexGen * ComposeTest::createLayerTex(CTexGen * source1, CTexGen * source2) {
	CLayerTex* texGen = new CLayerTex();
	texGens.push_back(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source1);
	texGen->setSource2(source2);
	return texGen;
}

/** Create a new texture to use as a rendering target. */
CRenderTexture * ComposeTest::createTarget() {
	currentTarget = new CRenderTexture(textureSize, textureSize);
	targets.push_back(currentTarget);
	return currentTarget;
}

void ComposeTest::updateOctaves(int octaves) {
	currentTexGen->setOctaves(octaves);
	compose();
}

void ComposeTest::updateFrequency(float freq) {
	currentTexGen->setFrequency(freq);
	compose();
}

void ComposeTest::updateAngle(glm::vec3 angle) {
	currentTexGen->setAngles(angle);
	compose();
}

void ComposeTest::updatePosition(glm::vec3 translation) {
	currentTexGen->setTranslation(translation);
	compose();
}

void ComposeTest::updateScalePt(glm::vec3 scale) {
	currentTexGen->setScalePt(scale);
	compose();
}

void ComposeTest::updatePower(float power) {
	currentTexGen->setPower(power);
	compose();
}

void ComposeTest::updateScale(float scale) {
	currentTexGen->setScale(scale);
	compose();
}

void ComposeTest::updateBias(float bias) {
	currentTexGen->setBias(bias);
	compose();
}

/** Change the colour gradient of the current tex gen, if it has one. */
void ComposeTest::updateColourGradient(ColourGradient& gradient) {
	currentTexGen->setPalette(gradient);
	//compose();
}

/** Make the next or previous texGen the current one. */
void ComposeTest::scrollStack(int direction) {
	if (direction > 0)
		stackPosition++;
	else
		stackPosition--;
	setCurrentLayer(stackPosition);
}

void ComposeTest::setCurrentLayer(int layerNo) {
	if (texGens.size() == 0)
		return;
	stackPosition = layerNo;
	if (stackPosition < 0)
		stackPosition = 0;
	else if (stackPosition >= texGens.size())
		stackPosition = texGens.size() - 1;

	currentTexGen = texGens[stackPosition];
	currentTarget = currentTexGen->getTarget();

}
