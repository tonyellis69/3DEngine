#include "composeTest.h"

using namespace glm;

ComposeTest::~ComposeTest() {
	for (auto texGen : texGens)
		delete texGen;
	//for (auto target : targets)
		//delete target;
}

void ComposeTest::initTex() {


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
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createRidgedMFTex() {
	CRidgedMultiTex* texGen = new CRidgedMultiTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createCylinderTex() {
	CylinderTex* texGen = new CylinderTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createBillowTex() {
	CBillowTex* texGen = new CBillowTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createVoronoiTex() {
	CVoronoiTex* texGen = new CVoronoiTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createTurbulenceTex(CTexGen* source) {
	CTurbulenceTex* texGen = new CTurbulenceTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source);
	return texGen;
}

CTexGen * ComposeTest::createColouriseTex(CTexGen* source, ColourGradient* colourGradient) {
	CColourTex* colourTex = new CColourTex();
	addToStack(colourTex); 
	colourTex->setTarget(createTarget());
	if (colourGradient)
		colourTex->setPalette(*colourGradient);
	if (source)
		colourTex->setSource(source);
	return colourTex;
}

CTexGen * ComposeTest::createScaleBiasTex(CTexGen * source) {
	CScaleBiasTex* texGen = new CScaleBiasTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source);
	return texGen;
}

CTexGen * ComposeTest::createSeamlessTex() {
	CSeamlessTex* texGen = new CSeamlessTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(&nullTexGen);
	return texGen;
}

CTexGen * ComposeTest::createScalePointTex(CTexGen * source) {
	CScalePointTex* texGen = new CScalePointTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source);
	return texGen;
}

CTexGen * ComposeTest::createAddTex(CTexGen * source1, CTexGen * source2) {
	CAddTex* texGen = new CAddTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source1);
	texGen->setSource2(source2);
	return texGen;
}

CTexGen * ComposeTest::createLayerTex(CTexGen * source1, CTexGen * source2) {
	CLayerTex* texGen = new CLayerTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source1);
	texGen->setSource2(source2);
	return texGen;
}

CTexGen * ComposeTest::createSelectTex(CTexGen * source1, CTexGen * source2, CTexGen * control) {
	CSelectTex* texGen = new CSelectTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(source1);
	texGen->setSource2(source2);
	texGen->setSource3(control);
	return texGen;
}

CTexGen * ComposeTest::createGausTex() {
	CGausTex* texGen = new CGausTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(&nullTexGen);
	return texGen;
}

void ComposeTest::addToStack(CTexGen* texGen) {
	//check for unique name
	string newName = texGen->name;
	for (auto existing : texGens) {
		if (existing->name.substr(0, newName.size()) == newName) {
			int suffix = 2;
			for (auto existing2 : texGens) {
				if (existing2->name.substr(0,newName.size()) == newName)
					suffix = std::max(existing2->count+1, suffix);
			}
			texGen->name = newName + "#" + to_string(suffix);;
			texGen->count = suffix;
			break;
		}
	}
	texGens.push_back(texGen);
}


/** Create a new texture to use as a rendering target. */
CRenderTexture * ComposeTest::createTarget() {
	currentTarget = new CRenderTexture(textureSize, textureSize);
	//targets.push_back(currentTarget);
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

void ComposeTest::updatePersistence(float persist) {
	currentTexGen->setPersistence(persist);
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

void ComposeTest::updateDistance(bool dist) {
	currentTexGen->setDistance(dist);
	compose();
}

void ComposeTest::updateRandomHue(bool hue) {
	currentTexGen->setRandomHue(hue);
	compose();
}

/** Change the colour gradient of the current tex gen, if it has one. */
void ComposeTest::updateColourGradient(ColourGradient& gradient) {
	currentTexGen->setPalette(gradient);
	//compose();
}

void ComposeTest::updateLower(float lower) {
	currentTexGen->setLowerBound(lower);
	compose();
}

void ComposeTest::updateUpper(float upper) {
	currentTexGen->setUpperBound(upper);
	compose();
}

void ComposeTest::updateFalloff(float falloff) {
	currentTexGen->setFalloff(falloff);
	compose();
}

void ComposeTest::updateSampleWidth(float width) {
	currentTexGen->setSampleWidth(width);
	compose();
}

void ComposeTest::updateSampleHeight(float height) {
	currentTexGen->setSampleHeight(height);
	compose();
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

/** Add this tex gen to the stack. */
void ComposeTest::addTexGen(CTexGen* texGen) {
	texGens.push_back(texGen);
}

void ComposeTest::deleteTexGen(int stackNo) {
	//check for about-to-be-invalid sources
	for (unsigned int gen = stackNo+1; gen < texGens.size(); gen++) {
		if (texGens[gen]->srcTex1 == texGens[stackNo])
			texGens[gen]->setSource(&nullTexGen);
		if (texGens[gen]->srcTex2 == texGens[stackNo])
			texGens[gen]->setSource2(&nullTexGen);
		if (texGens[gen]->srcTex3 == texGens[stackNo])
			texGens[gen]->setSource3(&nullTexGen);
	}

	delete texGens[stackNo];
	texGens.erase(texGens.begin() + stackNo);
	if (stackPosition >= stackNo)
		setCurrentLayer(stackNo - 1);
	
}
