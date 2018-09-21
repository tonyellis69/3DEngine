#include "compositor.h"

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
	CRenderTexture noise(512, 512);

	CNoiseTex noiseTex;
	noiseTex.setTarget(&noise);
	noiseTex.render();

	CColourTex colourTex;
	colourTex.setSource(&noise);
	colourTex.setTarget(&sample);
	colourTex.render();

	image->setTexture(sample);
}

/** Create the interface for viewing textures. */
void Compositor::initGUI() {
	image = new CGUIimage(0, 0, 512, 512);
	pApp->GUIroot.Add(image);
	//image->setVisible(false);

	paletteGradient.resize(256, 0);
	palette = new CGUIpaletteBar(700, 100, 280, 80);
	palette->setGradientTexture(paletteGradient);
	pApp->GUIroot.addModal(palette);



//	num = new CGUInumeric2(800, 300, 100, 22);
	//pApp->GUIroot.addModal(num);
}


