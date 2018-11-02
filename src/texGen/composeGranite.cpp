#include "composeGranite.h"

using namespace glm;

void ComposeGranite::initTex()
{
	sample1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	composedTex.resize(512, 512);
	colouredTex.resize(512, 512);



	texColourGradient = { 0, 0, 0, 0, 255,
	8, 0, 0, 0, 255,
	16, 216, 216, 242, 255,
	128, 191, 191, 191, 255,
	191, 210, 116, 125, 255,
	223, 210, 113, 98, 255,
	255, 255, 176, 192, 255 };
}

void ComposeGranite::compose() {
	// Primary granite texture.  This generates the "roughness" of the texture when lit by a light source.
/*	CBillowTex billowTex;
	billowTex.setTarget(&sample1);
	billowTex.setOctaves(6);
	billowTex.setFrequency(8);
	billowTex.setPersistence(0.625);
	billowTex.render();

	CVoronoiTex voronoiTex;
	voronoiTex.setTarget(&sample2);
	voronoiTex.setFrequency(16);
	voronoiTex.render();

	// Scale the small grain values so that they may be added to the base
// granite texture.  Voronoi polygons normally generate pits, so apply a
// negative scaling factor to produce bumps instead.
	CScaleBiasTex scale;
	scale.setSource(&sample2);
	scale.setTarget(&sample3);
	scale.setScaleBias(-0.5, 0.0);
	scale.render();

	// Combine the primary granite texture with the small grain texture.
	CAddTex addTex;
	addTex.setSource(&sample1);
	addTex.setSource2(&sample3);
	addTex.setTarget(&sample2);
	addTex.render();

	// Finally, perturb the granite texture to add realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample2);
	turbulenceTex.setTarget(&composedTex);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setOctaves(6);
	turbulenceTex.setPower(1.0 / 8.0);
	turbulenceTex.render();
	*/
}
