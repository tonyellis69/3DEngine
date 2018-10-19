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
	pApp->GUIroot.addModal(paletteBar);
	paletteBar->loadPalette();

	paletteBar->setGUIcallback(this);
}

/** Callback from the palette bar control. */
void CTexGenUI::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if (sender == paletteBar && msg.Msg == uiMsgUpdate) {
		texColourGradient = paletteBar->colourGradient;
		colourise();
	}
}

void CTexGenUI::hide(bool onOff) {
	image->setVisible(!onOff);
	paletteBar->setVisible(!onOff);
}

void CTexGenUI::compose() {
	texCompositor.initTex();
	texColourGradient = texCompositor.texColourGradient;
	paletteBar->setGradient(texColourGradient);

	texCompositor.compose();
	colourise();
}


void CTexGenUI::colourise() {

	CColourTex colourTex;
	colourTex.setSource(&texCompositor.composedTex);
	colourTex.setTarget(&texCompositor.colouredTex);
	colourTex.setPalette(texColourGradient);
	colourTex.render();

	image->setTexture(texCompositor.colouredTex);
}