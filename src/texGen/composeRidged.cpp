#include "composeRidged.h"

using namespace glm;

void ComposeRidgedMF::initTex() {
	sample1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	composedTex.resize(512, 512);
	colouredTex.resize(512, 512);

	colourise = true;
	octaves = 1;
	frequency = 1;
}

void ComposeRidgedMF::compose() {
	CRidgedMultiTex multiTex;
	multiTex.setTarget(&composedTex);
	multiTex.setOctaves(octaves);
	multiTex.setFrequency(frequency);
	multiTex.setSample(vec3(0), vec3(2, 2, 0));
	multiTex.render();
}

void ComposeRidgedMF::updateOctaves(int octaves) {
	this->octaves = octaves;
	compose();
}

void ComposeRidgedMF::updateFrequency(float freq) {
	this->frequency = freq;
	compose();
}
