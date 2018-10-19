#include "compositor.h"


using namespace glm;





void Compositor::composeSky() {
	// Base of the water texture.  The Voronoi polygons generate the waves.  At
 // the center of the polygons, the values are at their lowest.  At the edges
 // of the polygons, the values are at their highest.  The values smoothly
 // change between the center and the edges of the polygons, producing a
 // wave-like effect.
	CVoronoiTex voronoiTex;
	voronoiTex.setTarget(&sample1);
	voronoiTex.setFrequency(8);
	voronoiTex.setDistance(true);
	voronoiTex.setRandomHue(false);
	voronoiTex.setSample(vec3(0), vec3(2, 6, 0));
	voronoiTex.render();


	// Smoothly perturb the water texture for more realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample1);
	turbulenceTex.setTarget(&sample2);
	turbulenceTex.setFrequency(8);
	turbulenceTex.setRoughness(1);
	turbulenceTex.setPower(1.0 / 32.0);
	turbulenceTex.render();

	//Produces the basic shape of soft, fluffy clouds.
	/*module::Billow cloudBase;
	cloudBase.SetSeed(2);
	cloudBase.SetFrequency(2.0);
	cloudBase.SetPersistence(0.375);
	cloudBase.SetLacunarity(2.12109375);
	cloudBase.SetOctaveCount(4);
	cloudBase.SetNoiseQuality(QUALITY_BEST); */

	CBillowTex billowTex;
	billowTex.setTarget(&sample1);
	billowTex.setOctaves(4);
	billowTex.setFrequency(2);
	billowTex.setPersistence(0.375);
	billowTex.render();

	// Perturb the cloud texture for more realism.
/*	module::Turbulence finalClouds;
	finalClouds.SetSourceModule(0, cloudBase);
	finalClouds.SetSeed(3);
	finalClouds.SetFrequency(16.0);
	finalClouds.SetPower(1.0 / 64.0);
	finalClouds.SetRoughness(2);*/

	CTurbulenceTex turbulenceTex2;
	turbulenceTex2.setSource(&sample1);
	turbulenceTex2.setTarget(&sample3);
	turbulenceTex2.setFrequency(16);
	turbulenceTex2.setRoughness(2);
	turbulenceTex2.setPower(1.0 / 64.0);
	turbulenceTex2.render();




	ColourGradient waterColour = { 0, 48, 64, 192, 255,
								191, 96, 192, 255, 255,
								255, 255, 255, 255, 255 };
	palette1.resize(256,0);
	palette1.setData(waterColour.getData());

	CColourTex colourTex;
	colourTex.setSource(&sample2);
	colourTex.setTarget(&sample1);
	colourTex.setPalette(waterColour);
	colourTex.render();

	ColourGradient cloudColour = { 0, 255, 255, 255, 0,
							64, 255,255, 255, 0,
							255, 255, 255, 255, 255 };
	palette2.resize(256, 0);
	palette2.setData(cloudColour.getData());

	CColourTex colourTex2;
	colourTex2.setSource(&sample3);
	colourTex2.setTarget(&sample2);
	colourTex2.setPalette(cloudColour);
	colourTex2.render();

	CLayerTex layerTex;
	layerTex.setSource(&sample1);
	layerTex.setSource2(&sample2);
	layerTex.setTarget(&composedTex);
	layerTex.render();

}

void Compositor::composeSlime() {
	CBillowTex billowTex;
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
	selectTex.setControl(&sample2);
	selectTex.setTarget(&sample4);
	selectTex.setBounds(0.3125, 0.6875);
	selectTex.setFalloff(0.5);
	selectTex.render();

	// Finally, perturb the slime texture to add realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample4);
	turbulenceTex.setTarget(&composedTex);
	turbulenceTex.setFrequency(8);
	turbulenceTex.setRoughness(2);
	turbulenceTex.setPower(1.0 / 32.0);
	turbulenceTex.render();
}

/** Create texures here. */
void Compositor::composeGranite() {
	// Primary granite texture.  This generates the "roughness" of the texture when lit by a light source.
	CBillowTex billowTex;
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
	turbulenceTex.setRoughness(6);
	turbulenceTex.setPower(1.0 / 8.0);
	turbulenceTex.render();
}


void Compositor::composeWood() {
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
	turbulenceTex.setRoughness(4);
	turbulenceTex.setPower(1.0 / 256.0);
	turbulenceTex.render();


	// Finally, perturb the wood texture to produce the final texture.
	CTurbulenceTex turbulenceTex2;
	turbulenceTex2.setSource(&sample1);
	turbulenceTex2.setTarget(&composedTex);
	turbulenceTex2.setFrequency(2);
	turbulenceTex2.setRoughness(4);
	turbulenceTex2.setPower(1.0 / 64.0);
	turbulenceTex2.render();
}










