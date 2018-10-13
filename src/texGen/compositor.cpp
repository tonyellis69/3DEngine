#include "compositor.h"


using namespace glm;

CGUIcolourPicker* Compositor::tmp = NULL;

Compositor::Compositor() {
	
}

void Compositor::init(CBaseApp * app) {
	pApp = app;
	initGUI();
}

void Compositor::compose() {
	sample.resize(512, 512);
	noise.resize(512, 512);
	noise1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	sample5.resize(512, 512);
	sample6.resize(512, 512);
	sample7.resize(512, 512);

	// Large slime bubble texture.
//	module::Billow largeSlime;
//	largeSlime.SetSeed(0);
//	largeSlime.SetFrequency(4.0);
//	largeSlime.SetLacunarity(2.12109375);
//	largeSlime.SetOctaveCount(1);
//	largeSlime.SetNoiseQuality(QUALITY_BEST);

	CBillowTex billowTex;
	billowTex.setTarget(&noise);
	billowTex.setOctaves(1);
	billowTex.setFrequency(4);
	billowTex.render();


	// Base of the small slime bubble texture.  This texture will eventually
// appear inside cracks in the large slime bubble texture.
	CBillowTex billowTex2;
	billowTex2.setTarget(&sample);
	billowTex2.setOctaves(1);
	billowTex2.setFrequency(24);
	billowTex2.render();

	// Scale and lower the small slime bubble values.
	CScaleBiasTex scale;
	scale.setSource(&sample);
	scale.setTarget(&sample2);
	scale.setScaleBias(0.5, -0.5);
	scale.render();

	// Create a map that specifies where the large and small slime bubble
// textures will appear in the final texture map.
	CRidgedMultiTex multiTex;
	multiTex.setTarget(&noise1);
	multiTex.setOctaves(3);
	multiTex.render();

	CSelectTex selectTex;
	selectTex.setSource(&noise);
	selectTex.setSource2(&sample2);
	selectTex.setControl(&noise1);
	selectTex.setTarget(&sample3);
	selectTex.setBounds(0.3125, 0.6875);
	selectTex.render();

	CColourTex colourTex;
	colourTex.setSource(&sample4);
	colourTex.setTarget(&sample5);
	colourTex.setPalette(&paletteBar->paletteTexture);
	colourTex.render();




	image->setTexture(sample3);

}

/** Create texures here. */
void Compositor::composeGranite() {
	sample.resize(512, 512);
	noise.resize(512, 512);
	noise1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	sample5.resize(512, 512);
	sample6.resize(512, 512);
	sample7.resize(512, 512);

	// Primary granite texture.  This generates the "roughness" of the texture
// when lit by a light source.
/*	module::Billow primaryGranite;
	primaryGranite.SetSeed(0);
	primaryGranite.SetFrequency(8.0);
	primaryGranite.SetPersistence(0.625);
	primaryGranite.SetLacunarity(2.18359375);
	primaryGranite.SetOctaveCount(6);
	primaryGranite.SetNoiseQuality(QUALITY_STD);
	*/

	CBillowTex billowTex;
	billowTex.setTarget(&noise);
	billowTex.setOctaves(6);
	billowTex.setFrequency(8);
	billowTex.setPersistence(0.625);
	billowTex.render();

	CVoronoiTex voronoiTex;
	voronoiTex.setTarget(&sample);
	voronoiTex.setFrequency(16);
	voronoiTex.render();

	// Scale the small grain values so that they may be added to the base
// granite texture.  Voronoi polygons normally generate pits, so apply a
// negative scaling factor to produce bumps instead.
	CScaleBiasTex scale;
	scale.setSource(&sample);
	scale.setTarget(&sample2);
	scale.setScaleBias(-0.5, 0.0);
	scale.render();

	// Combine the primary granite texture with the small grain texture.
	CAddTex addTex;
	addTex.setSource(&noise);
	addTex.setSource2(&sample2);
	addTex.setTarget(&sample3);
	addTex.render();

	// Finally, perturb the granite texture to add realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample3);
	turbulenceTex.setTarget(&sample4);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setRoughness(6);
	turbulenceTex.setPower(1.0 / 8.0);
	turbulenceTex.render();


	
	CColourTex colourTex;
	colourTex.setSource(&sample4);
	colourTex.setTarget(&sample5);
	colourTex.setPalette(&paletteBar->paletteTexture);
	colourTex.render();




	image->setTexture(sample5);

	//image->setTexture(noise);
}


void Compositor::composeWood() {
	sample.resize(512, 512);
	noise.resize(512, 512);
	noise1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	sample5.resize(512, 512);
	sample6.resize(512, 512);
	sample7.resize(512, 512);


	CylinderTex cylinderTex;
	cylinderTex.setTarget(&sample);
	cylinderTex.setFrequency(16);
	cylinderTex.setTranslation(vec3(0, -1.48, 0));
	cylinderTex.setAngles(vec3(84, 0, 0));
	cylinderTex.render();


	// Perlin noise to use for the wood grain.
	CNoiseTex noiseTex;
	noiseTex.setTarget(&noise);
	noiseTex.setOctaves(3);
	noiseTex.setFrequency(48);
	//noiseTex.setSample(vec3(0), vec3(2, 2, 0));
	noiseTex.render();

	//scale scales the *values* of the source
	//scalepoint seems to scale the sampling
	CScalePointTex scalePoint;
	scalePoint.setSource(&noise);
	scalePoint.setTarget(&sample2);
	scalePoint.setScale(vec3(1, 0.25, 1));
	scalePoint.render();

	CScaleBiasTex scale;
	scale.setSource(&sample2);
	scale.setTarget(&sample3);
	scale.setScaleBias(0.25, 0.125);
	scale.render();

	CAddTex addTex;
	addTex.setSource(&sample);
	addTex.setSource2(&sample3);
	addTex.setTarget(&sample4);
	addTex.render();

	// Slightly perturb the wood texture for more realism.
	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&sample4);
	turbulenceTex.setTarget(&sample5);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setRoughness(4);
	turbulenceTex.setPower(1.0 / 256.0);
	turbulenceTex.render();


	// Finally, perturb the wood texture to produce the final texture.
	CTurbulenceTex turbulenceTex2;
	turbulenceTex2.setSource(&sample5);
	turbulenceTex2.setTarget(&sample6);
	turbulenceTex2.setFrequency(2);
	turbulenceTex2.setRoughness(4);
	turbulenceTex2.setPower(1.0 / 64.0);
	turbulenceTex2.render();

/*	CSeamlessTex seamlessTex;
	seamlessTex.setSource(&sample6); //was4
	seamlessTex.setTarget(&sample5);
	seamlessTex.render();
	*/

	CColourTex colourTex;
	colourTex.setSource(&sample6);
	colourTex.setTarget(&sample7);
	colourTex.setPalette(&paletteBar->paletteTexture);
	colourTex.render();

	


	image->setTexture(sample7);
	
	//image->setTexture(noise);
}

void Compositor::composeJade() {
	sample.resize(512, 512);
	noise.resize(512, 512);
	noise1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);
	sample5.resize(512, 512);

	CRidgedMultiTex multiTex;
	multiTex.setTarget(&noise1);
	multiTex.setOctaves(6);
	multiTex.setSample(vec3(0), vec3(2, 2, 0));
	multiTex.render();


	/*
	CNoiseTex noiseTex;
	noiseTex.setTarget(&noise);
	noiseTex.setOctaves(1);
	noiseTex.setSample(vec3(0), vec3(2, 2, 0));
	noiseTex.render();
	*/

	CylinderTex cylinderTex;
	cylinderTex.setTarget(&noise);
	cylinderTex.setFrequency(2);
	cylinderTex.setAngles(vec3( 90,25,5 ));
	cylinderTex.render();

	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&noise);
	turbulenceTex.setTarget(&sample);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setRoughness(5);
	//TO DO: try with roughness 4, sample size of 2x2
	turbulenceTex.setPower(0.25);
	turbulenceTex.render();

	CScaleBiasTex scale;
	scale.setSource(&sample);
	scale.setTarget(&sample2);
	scale.setScaleBias(0.25, 0);
	scale.render();

	CAddTex addTex;
	addTex.setSource(&noise1);
	addTex.setSource2(&sample2);
	addTex.setTarget(&sample4);
	addTex.render();

	CSeamlessTex seamlessTex;
	seamlessTex.setSource(&sample4); //was4
	seamlessTex.setTarget(&sample5);
	seamlessTex.render();

	CColourTex colourTex;
	colourTex.setSource(&sample5);
	colourTex.setTarget(&sample3);
	colourTex.setPalette(&paletteBar->paletteTexture);
	colourTex.render();




	image->setTexture(sample3);

	//image->setTexture(noise);
}

void Compositor::hide(bool onOff) {
	image->setVisible(!onOff);
	paletteBar->setVisible(!onOff);
}

/** Create the interface for viewing textures. */
void Compositor::initGUI() {
	image = new CGUIimage(0, 0, 512, 512);
	pApp->GUIroot.Add(image);
	//image->setVisible(false);

	paletteBar = new CGUIpaletteBar(700, 100, 280, 180);
	//paletteBar->setGradientTexture(paletteGradient);
	pApp->GUIroot.addModal(paletteBar);
	paletteBar->loadPalette();

	paletteBar->setGUIcallback(this);
}

/** Callback from the palette bar control. */
void Compositor::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if (sender == paletteBar && msg.Msg == uiMsgUpdate) {
		compose();

	}

}




