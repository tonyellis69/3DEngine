#include "composeJade.h"

using namespace glm;

void ComposeJade::initTex() {
	sample1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	composedTex.resize(512, 512);
	colouredTex.resize(512, 512);

	texColourGradient = { 0,24, 146, 102, 255,
	128, 78, 154, 115, 255,
	159, 128, 204, 165, 255,
	175, 78, 154, 115, 255,
	255, 29, 135, 102, 255 };
}

void ComposeJade::compose() {
/*	CRidgedMultiTex multiTex;
	multiTex.setTarget(&sample1);
	multiTex.setOctaves(6);
	multiTex.setSample(vec3(0), vec3(2, 2, 0));
	multiTex.render();

	CylinderTex cylinderTex;
	cylinderTex.setTarget(&sample2);
	cylinderTex.setFrequency(2);
	cylinderTex.setAngles(vec3(90, 25, 5));
	cylinderTex.render();

	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample2);
	turbulenceTex.setTarget(&sample3);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setOctaves(5); //4 in libnoise, not sure why I need 5
	turbulenceTex.setPower(0.25);
	turbulenceTex.render();

	CScaleBiasTex scale;
	scale.setSource(&sample3);
	scale.setTarget(&sample2);
	scale.setScaleBias(0.25, 0);
	scale.render();

	CAddTex addTex;
	addTex.setSource(&sample1);
	addTex.setSource2(&sample2);
	addTex.setTarget(&composedTex);
	addTex.render();

	//CSeamlessTex seamlessTex;
	//seamlessTex.setSource(&sample3); 
	//seamlessTex.setTarget(&composedTex);
	//seamlessTex.render();

	*/
}
