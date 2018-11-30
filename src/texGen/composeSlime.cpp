#include "composeSlime.h"

using namespace glm;

void ComposeSlime::initTex()
{
	sample1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	composedTex.resize(512, 512);
	colouredTex.resize(512, 512);

	texColourGradient = { 0, 160, 64, 42, 255,
	127, 64, 192, 64, 255,
	255, 128, 255, 128, 255 };
}

void ComposeSlime::compose() {
/*	CBillowTex billowTex;
	billowTex.setTarget(&sample1);
	billowTex.setOctaves(1);
	billowTex.setFrequency(4);
	billowTex.render();

	// Base of the small slime bubble texture.  This texture will eventually
// appear inside cracks in the large slime bubble texture.
	CBillowTex billowTex2;
	billowTex2.setTarget(&sample2);
	billowTex2.setOctaves(1);
	billowTex2.setFrequency(24);
	billowTex2.render();

	// Scale and lower the small slime bubble values.
	CScaleBiasTex scale;
	scale.setSource(&sample2);
	scale.setTarget(&sample3);
	scale.setScaleBias(0.5, -0.5);
	scale.render();

	// Create a map that specifies where the large and small slime bubble
// textures will appear in the final texture map.
	CRidgedMultiTex multiTex;
	multiTex.setTarget(&sample2);
	multiTex.setOctaves(3);
	multiTex.render();

	CSelectTex selectTex;
	selectTex.setSource(&sample1);
	selectTex.setSource2(&sample3);
	selectTex.setSource3(&sample2);
	selectTex.setTarget(&sample4);
	selectTex.setBounds(0.3125, 0.6875);
	selectTex.setFalloff(0.5);
	selectTex.render();

	// Finally, perturb the slime texture to add realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample4);
	turbulenceTex.setTarget(&composedTex);
	turbulenceTex.setFrequency(8);
	turbulenceTex.setOctaves(2);
	turbulenceTex.setPower(1.0 / 32.0);
	turbulenceTex.render();
	*/
}
