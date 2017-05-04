
#include "colour.h"
#include "skyDome.h"

using namespace glm;

CSkyDome::CSkyDome() {
	levels[skyHorizon] = 0;
	levels[skyNearHorizon] = 0.09f;
	levels[skyLowerThird] = 0.18f;
	levels[skyDominant] = 0.84f;

	setHorizonColour(glm::rgbColor(vec3(199, 0.1, 0.97f)));
	setNearHorizonColour(glm::rgbColor(vec3(240, 0.62f, 1.0f)));
	setLowerThirdColour(glm::rgbColor(vec3(240, 0.83f, 0.76f)));
	setDominantColour(glm::rgbColor(vec3(216, 0.77f, 0.71f)));

	//setHorizonColour(vec3(1, 1, 1));
	//setDominantColour(vec3(0, 0, 1));

	dome = NULL;
}

CSkyDome::~CSkyDome() {
	if (dome != NULL)
		delete dome;
}

void CSkyDome::setHorizonColour(glm::vec3 & colour) {
	heightColours[skyHorizon] = colour;
}

void CSkyDome::setNearHorizonColour(glm::vec3 & colour) {
	heightColours[skyNearHorizon] = colour;
}

void CSkyDome::setLowerThirdColour(glm::vec3 & colour) {
	heightColours[skyLowerThird] = colour;
}

void CSkyDome::setDominantColour(glm::vec3 & colour) {
	heightColours[skyDominant] = colour;
}

void CSkyDome::setModel(CModel * model) {
	dome = model;
}

