#include "compositor.h"


using namespace glm;

CGUIcolourPicker* Compositor::tmp = NULL;

Compositor::Compositor() {
	
}

void Compositor::init(CBaseApp * app) {
	pApp = app;
	initGUI();
}

/** Create texures here. */
void Compositor::compose() {
	sample.resize(512, 512);
	noise.resize(512, 512);
	noise1.resize(512, 512);
	sample2.resize(512, 512);
	sample3.resize(512, 512);
	sample4.resize(512, 512);

	CRidgedMultiTex multiTex;
	multiTex.setTarget(&noise1);
	multiTex.setOctaves(6);
	multiTex.setSample(vec3(0),vec3(2, 2, 0));
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
	cylinderTex.setAngles({ 90,25,5 });
	cylinderTex.render();

	CTurbulenceTex turbulenceTex;
	turbulenceTex.setSource(&noise);
	turbulenceTex.setTarget(&sample);
	turbulenceTex.setFrequency(4);
	turbulenceTex.setRoughness(5);
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

	CColourTex colourTex;
	colourTex.setSource(&sample4);
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




