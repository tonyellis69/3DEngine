#include "texGenUI.h"

void CTexGenUI::init(CBaseApp * app) {
	pApp = app;
	initGUI();
}

/** Create the interface for viewing textures. */
void CTexGenUI::initGUI() {
	image = new CGUIimage(0, 0, 512, 512);
	pApp->GUIroot.Add(image);
	//image->setVisible(false);

	paletteBar = new CGUIpaletteBar(700, 100, 280, 180);
	pApp->GUIroot.Add(paletteBar);
	paletteBar->loadPalette();

	paletteBar->setGUIcallback(this);

	GUInoiseCtrl = new CGUInoiseCtrl(700, 400, 350, 180);
	GUInoiseCtrl->setGUIcallback(this);
	pApp->GUIroot.Add(GUInoiseCtrl);
	//pApp->GUIroot.focusControl = GUInoiseCtrl;

}

/** Callback from the palette bar control. */
void CTexGenUI::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if (sender == paletteBar && msg.Msg == uiMsgUpdate) {
		texCompositor.updateColourGradient(paletteBar->colourGradient);
		//return;
	}

	if (sender == GUInoiseCtrl->octaveCtrl) {
		texCompositor.updateOctaves((int)msg.fValue);
	}

	if (sender == GUInoiseCtrl->freqCtrl) {
		texCompositor.updateFrequency(msg.fValue);
	}

	if (sender == GUInoiseCtrl->powerCtrl) {
		texCompositor.updatePower(msg.fValue);
	}

	if (sender == GUInoiseCtrl->scaleCtrl) {
		texCompositor.updateScale(msg.fValue);
	}

	if (sender == GUInoiseCtrl->biasCtrl) {
		texCompositor.updateBias(msg.fValue);
	}

	for (int axis = 0; axis < 3; axis++) {
		if (sender == GUInoiseCtrl->angle[axis]) {
			texCompositor.updateAngle(GUInoiseCtrl->getFullRotation());
		}
	}

	if (sender == GUInoiseCtrl && msg.Msg == uiMouseWheel) {
		texCompositor.scrollStack(msg.value);
		

	}


	display();
}

void CTexGenUI::hide(bool onOff) {
	image->setVisible(!onOff);
	paletteBar->setVisible(!onOff);
}

void CTexGenUI::compose() {
	texCompositor.initTex();
	//texColourGradient = texCompositor.texColourGradient;
	//paletteBar->setGradient(texColourGradient);

	ColourGradient jade = { 0, 24, 146, 102, 255,
		128, 78, 154, 115, 255,
		159, 128, 204, 165, 255,
		175, 78, 154, 115, 255,
		255, 29, 135, 102, 255 };

	CTexGen* multiF = texCompositor.createRidgedMFTex();
//	CTexGen* cylinder = texCompositor.createCylinderTex();
//	CTexGen* turb = texCompositor.createTurbulenceTex(cylinder);
//	CTexGen* scale = texCompositor.createScaleBiasTex(turb);
//	CTexGen* add = texCompositor.createAddTex(scale,multiF);
//	texCompositor.createColouriseTex(add, jade);

	texCompositor.compose();
	display();
}


void CTexGenUI::display() {
	/*if (texCompositor.colourise) {
		CColourTex colourTex;
		colourTex.setSource(texCompositor.currentTarget);
		colourTex.setTarget(&texCompositor.colouredTex);
		colourTex.setPalette(texColourGradient);
		colourTex.render();
		image->setTexture(texCompositor.colouredTex);
	}
	else */
		image->setTexture(*texCompositor.currentTarget);
}