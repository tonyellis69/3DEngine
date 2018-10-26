#include "composeWood.h"

using namespace glm;

void ComposeWood::initTex()
{
	sample1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	composedTex.resize(512, 512);
	colouredTex.resize(512, 512);



	texColourGradient = { 0, 189, 94, 4, 255,
	191, 144, 48, 6, 255,
	255, 60, 10, 8, 255 };
}

void ComposeWood::compose() {
	CylinderTex cylinderTex;
	cylinderTex.setTarget(&sample1);
	cylinderTex.setFrequency(16);
	cylinderTex.setTranslation(vec3(0, -1.48, 0));
	cylinderTex.setAngles(vec3(84, 0, 0));
	cylinderTex.render();


	// Perlin noise to use for the wood grain.
	CNoiseTex noiseTex;
	noiseTex.setTarget(&sample2);
	noiseTex.setOctaves(3);
	noiseTex.setFrequency(48);
	noiseTex.render();

	CScalePointTex scalePoint;
	scalePoint.setSource(&sample2);
	scalePoint.setTarget(&sample3);
	scalePoint.setScale(vec3(1, 0.25, 1));
	scalePoint.render();

	CScaleBiasTex scale;
	scale.setSource(&sample3);
	scale.setTarget(&sample2);
	scale.setScaleBias(0.25, 0.125);
	scale.render();

	CAddTex addTex;
	addTex.setSource(&sample1);
	addTex.setSource2(&sample2);
	addTex.setTarget(&sample3);
	addTex.render();

	// Slightly perturb the wood texture for more realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample3);
	turbulenceTex.setTarget(&sample1);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setOctaves(4);
	turbulenceTex.setPower(1.0 / 256.0);
	turbulenceTex.render();


	// Finally, perturb the wood texture to produce the final texture.
	CTurbulenceTex turbulenceTex2;
	turbulenceTex2.setSource(&sample1);
	turbulenceTex2.setTarget(&composedTex);
	turbulenceTex2.setFrequency(2);
	turbulenceTex2.setOctaves(4);
	turbulenceTex2.setPower(1.0 / 64.0);
	turbulenceTex2.render();

}
