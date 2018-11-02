#include "composeSky.h"

using namespace glm;

void ComposeSky::initTex()
{
	sample1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	composedTex.resize(512, 512);
	colouredTex.resize(512, 512);

/*	texColourGradient = { 0, 160, 64, 42, 255,
	127, 64, 192, 64, 255,
	255, 128, 255, 128, 255 }; */

	colourise = false;
}

void ComposeSky::compose() {
	/*
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
	turbulenceTex.setOctaves(1);
	turbulenceTex.setPower(1.0 / 32.0);
	turbulenceTex.render();

	//Produces the basic shape of soft, fluffy clouds.
	
	

	CBillowTex billowTex;
	billowTex.setTarget(&sample1);
	billowTex.setOctaves(4);
	billowTex.setFrequency(2);
	billowTex.setPersistence(0.375);
	billowTex.render();

	// Perturb the cloud texture for more realism.


	CTurbulenceTex turbulenceTex2;
	turbulenceTex2.setSource(&sample1);
	turbulenceTex2.setTarget(&sample3);
	turbulenceTex2.setFrequency(16);
	turbulenceTex2.setOctaves(2);
	turbulenceTex2.setPower(1.0 / 64.0);
	turbulenceTex2.render();




	ColourGradient waterColour = { 0, 48, 64, 192, 255,
								191, 96, 192, 255, 255,
								255, 255, 255, 255, 255 };
	palette1.resize(256, 0);
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
	*/
}
