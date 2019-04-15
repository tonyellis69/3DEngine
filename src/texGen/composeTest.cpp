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

CTexGen * ComposeTest::createTurbulenceTex() {
	CTurbulenceTex* texGen = new CTurbulenceTex();
	addToStack(texGen);
	texGen->setSource(&nullTexGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createColouriseTex() {
	CColourTex* colourTex = new CColourTex();
	addToStack(colourTex); 
	colourTex->setSource(&nullTexGen);
	colourTex->setTarget(createTarget());
	return colourTex;
}

CTexGen * ComposeTest::createScaleBiasTex() {
	CScaleBiasTex* texGen = new CScaleBiasTex();
	addToStack(texGen);
	texGen->setSource(&nullTexGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createSeamlessTex() {
	CSeamlessTex* texGen = new CSeamlessTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(&nullTexGen);
	return texGen;
}

CTexGen * ComposeTest::createScalePointTex() {
	CScalePointTex* texGen = new CScalePointTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(&nullTexGen);
	return texGen;
}

CTexGen * ComposeTest::createAddTex() {
	CAddTex* texGen = new CAddTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(&nullTexGen);
	texGen->setSource2(&nullTexGen);
	return texGen;
}

CTexGen * ComposeTest::createLayerTex() {
	CLayerTex* texGen = new CLayerTex();
	addToStack(texGen);
	texGen->setSource(&nullTexGen);
	texGen->setSource2(&nullTexGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createSelectTex() {
	CSelectTex* texGen = new CSelectTex();
	addToStack(texGen);
	texGen->setSource(&nullTexGen);
	texGen->setSource2(&nullTexGen);
	texGen->setSource3(&nullTexGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createGausTex() {
	CGausTex* texGen = new CGausTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	texGen->setSource(&nullTexGen);
	return texGen;
}

CTexGen * ComposeTest::createRectsTex() {
	CRectsTex* texGen = new CRectsTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createBlocksTex() {
	CBlocksTex* texGen = new CBlocksTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createCoverTex() {
	CoverTex* texGen = new CoverTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createCutupTex() {
	CutupTex* texGen = new CutupTex();
	texGen->setSource(&nullTexGen);
	texGen->setSource2(&nullTexGen);
	addToStack(texGen);
	texGen->setTarget(createTarget());
	return texGen;
}

CTexGen * ComposeTest::createTerrainTex() {
	CTerrainTex* texGen = new CTerrainTex();
	addToStack(texGen);
	texGen->setTarget(createTarget());
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
	else if (stackPosition >= (int)texGens.size())
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

void ComposeTest::save(std::string filename) {
	std::ofstream out(filename, std::ios::binary);
	out << "t1"; //identifier

	for (auto texGen : texGens) //gives the overall composition of the stack
		writeObject(texGen->texGenType, out);
	writeObject(texNull, out);

	auto writeTexIndex = [&](CTexGen* texGen) {
		if (texGen == NULL) {
			writeObject((char)-1, out);	return;
		}
		for (char x = 0; x < (char)texGens.size(); x++) {
			if (texGen == texGens[x]) {
				writeObject(x, out);	return;
			}
		}

	};

	for (auto texGen : texGens) { //write index of sources
		writeTexIndex(texGen->srcTex1);
		writeTexIndex(texGen->srcTex2);
		writeTexIndex(texGen->srcTex3);
		texGen->write(out);
	}
	out.close();

}

void ComposeTest::restore(std::string filename) {
	std::ifstream in(filename, std::ios::binary);
	if (in.fail() || in.get() != 't') {
		cerr << "\nUnable to open tex file " << filename;
		return;
	}

	fileVersion = int(in.get());

	//find and load all the texgens
	TexGenType texGenType;
	while (readObject(texGenType, in)) {
		if (texGenType == texNull)
			break;
		switch (texGenType) {
		case texColour:		createColouriseTex(); break;
		case texNoise: createNoiseTex(); break;
		case texVoronoi: createVoronoiTex(); break;
		case texBillow: createBillowTex(); break;
		case texRidged: createRidgedMFTex(); break;
		case texCylinder: createCylinderTex(); break;
		case texTurbulence: createTurbulenceTex(); break;
		case texScaleBias: createScaleBiasTex(); break;
		case texSeamless:  createSeamlessTex(); break;
		case texAdd: createAddTex(); break;
		case texScalePoint: createScalePointTex(); break;
		case texSelect: createSelectTex(); break;
		case texLayer: createLayerTex(); break;
		case texGaus: createGausTex(); break;
		case texRects: createRectsTex(); break;
		case texBlocks: createBlocksTex(); break;
		case texCover: createCoverTex(); break;
		case texCutup: createCutupTex(); break;
		case texTerrain: createTerrainTex(); break;
		}
	}

	auto getColourGradient = [&]() { ColourGradient gradIn; int tabCount; readObject(tabCount, in);
	int tab; i32vec4 colour;
	for (int x = 0; x < tabCount; x++) {
		readObject(tab, in); readObject(colour, in);
		gradIn.changeColour(tab, colour);
	}
	return gradIn;
	};

	//read sources and data into texgens
	char x;
	for (auto texGen : texGens) {
		x = in.get();
		if (x != -1)
			texGen->setSource(texGens[x]);
		x = in.get();
		if (x != -1)
			texGen->setSource2(texGens[x]);
		x = in.get();
		if (x != -1)
			texGen->setSource3(texGens[x]);
		if (texGen->texGenType == texColour) {
			texGen->setPalette(getColourGradient());
		}
		else
			texGen->read(in);
	}
	in.close();

	setCurrentLayer(texGens.size() - 1);
	compose();
	compose(); //ugh. Ensures texgens with forward references are up-to-date

}

/** Return the end-product texture. */
CRenderTexture& ComposeTest::getComposedTexture() {
	return *texGens.back()->getTarget();
}

/** Save the current texGen's target image as a PNG file.*/
void ComposeTest::saveCurrentTexGenPNG(std::string filepath) {
	currentTexGen->getTarget()->savePNG(filepath);

}





