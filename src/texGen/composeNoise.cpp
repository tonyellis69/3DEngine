#include "composeNoise.h"

using namespace glm;

void ComposeNoise::initTex() {
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

void ComposeNoise::compose() {
	CNoiseTex noiseTex;
	noiseTex.setTarget(&composedTex);
	noiseTex.setOctaves(octaves);
	noiseTex.setFrequency(frequency);
	noiseTex.render();
}

void ComposeNoise::updateOctaves(int octaves) {
	this->octaves = octaves;
	compose();
}

void ComposeNoise::updateFrequency(float freq) {
	this->frequency = freq;
	compose();
}
