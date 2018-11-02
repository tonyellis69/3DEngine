#include "texGenUI.h"

#include <iostream>

using namespace glm;

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

	GUInoiseCtrl = new CGUInoiseCtrl(700, 400, 350, 300);
	GUInoiseCtrl->setGUIcallback(this);
	pApp->GUIroot.Add(GUInoiseCtrl);
	//pApp->GUIroot.focusControl = GUInoiseCtrl;

}

/** Callback from the palette bar control. */
void CTexGenUI::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if (sender == paletteBar && msg.Msg == uiMsgUpdate) {
		texCompositor.updateColourGradient(paletteBar->colourGradient);
		texCompositor.currentTexGen->render();
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
		if (sender == GUInoiseCtrl->angleCtrl[axis]) {
			texCompositor.updateAngle(GUInoiseCtrl->getFullRotation());
		}
		if (sender == GUInoiseCtrl->posCtrl[axis]) {
			texCompositor.updatePosition(GUInoiseCtrl->getFullPosition());
		}
		if (sender == GUInoiseCtrl->scalePtCtrl[axis]) {
			texCompositor.updateScalePt(GUInoiseCtrl->getScalePt());
		}
	}

	//cycling through layers
	if (sender == GUInoiseCtrl && msg.Msg == uiMouseWheel) {
		texCompositor.scrollStack(msg.value);
		//update the GUI with the settings for this layer
		updateGUI();
	}

	display();

	if (sender == GUInoiseCtrl->saveButton) {
		save();
	}
}

void CTexGenUI::hide(bool onOff) {
	image->setVisible(!onOff);
	paletteBar->setVisible(!onOff);
}

void CTexGenUI::compose() {
	texCompositor.initTex();

	/*
	ColourGradient jade = { 0, 24, 146, 102, 255,
		128, 78, 154, 115, 255,
		159, 128, 204, 165, 255,
		175, 78, 154, 115, 255,
		255, 29, 135, 102, 255 };

	CTexGen* multiF = texCompositor.createRidgedMFTex();
	CTexGen* cylinder = texCompositor.createCylinderTex();
	CTexGen* turb = texCompositor.createTurbulenceTex(cylinder);
	CTexGen* scale = texCompositor.createScaleBiasTex(turb);
	CTexGen* add = texCompositor.createAddTex(scale,multiF);
	texCompositor.createColouriseTex(add, &jade);
	*/


	/*
	ColourGradient wood = { 0, 189, 94, 4, 255,
	191, 144, 48, 6, 255,
	255, 60, 10, 8, 255 };
	CTexGen* cylinder = texCompositor.createCylinderTex();
	CTexGen* noise = texCompositor.createNoiseTex();
	CTexGen* scale = texCompositor.createScalePointTex(noise);
	CTexGen* scaleBias = texCompositor.createScaleBiasTex(scale);
	CTexGen* add = texCompositor.createAddTex(cylinder,scaleBias);
	CTexGen* turb1 = texCompositor.createTurbulenceTex(add);
	CTexGen* turb2 = texCompositor.createTurbulenceTex(turb1);
	texCompositor.createColouriseTex(turb2, &wood);
	*/
	restore();


	texCompositor.setCurrentLayer(0);
	updateGUI();
	texCompositor.compose();
	display();
}


void CTexGenUI::display() {
	if (texCompositor.texGens.size() > 0)
		image->setTexture(*texCompositor.currentTarget);
}

/** Update the GUI with the control settings for the current tex gen layer. */
void CTexGenUI::updateGUI() {
	GUInoiseCtrl->octaveCtrl->setValue(texCompositor.currentTexGen->getOctaves());
	GUInoiseCtrl->freqCtrl->setValue(texCompositor.currentTexGen->getFrequency());
	GUInoiseCtrl->powerCtrl->setValue(texCompositor.currentTexGen->getPower());
	GUInoiseCtrl->scaleCtrl->setValue(texCompositor.currentTexGen->getScale());
	GUInoiseCtrl->biasCtrl->setValue(texCompositor.currentTexGen->getBias());
	vec3 angle = texCompositor.currentTexGen->getAngle();
	GUInoiseCtrl->angleCtrl[0]->setValue(angle.x);
	GUInoiseCtrl->angleCtrl[1]->setValue(angle.y);
	GUInoiseCtrl->angleCtrl[2]->setValue(angle.z);

	vec3 pos = texCompositor.currentTexGen->getTranslation();
	GUInoiseCtrl->posCtrl[0]->setValue(pos.x);
	GUInoiseCtrl->posCtrl[1]->setValue(pos.y);
	GUInoiseCtrl->posCtrl[2]->setValue(pos.z);

	ColourGradient* texGenColourGrad = texCompositor.currentTexGen->getColourGradient();
	if (texGenColourGrad)
		paletteBar->setGradient(*texGenColourGrad);

}

void CTexGenUI::save() {
	std::ofstream out(pApp->dataPath + "woodTex.gen", std::ios::binary);

	auto writeTexIndex = [&](CTexGen* texGen) {
		int x;
		for (x = 0; x < texCompositor.texGens.size(); x++) {
			if (texGen == texCompositor.texGens[x]) {
				writeObject(x, out);
				return;
			}
		}
	};


	for (auto texGen : texCompositor.texGens) {
		writeObject(texGen->texGenType, out);
		if (texGen->srcTex1)
			writeTexIndex(texGen->srcTex1);
		if (texGen->srcTex2)
			writeTexIndex(texGen->srcTex2);
		texGen->write(out);

	}
	out.close();


}

void CTexGenUI::restore() {
	clear();

	std::ifstream in(pApp->dataPath + "woodTex.gen", std::ios::binary);
	TexGenType texGenType;

	auto getSourceByIndex = [&]() { int index; readObject(index, in); return texCompositor.texGens[index]; };
	auto getColourGradient = [&]() { ColourGradient gradIn; int tabCount; readObject(tabCount, in);
	int tab; i32vec4 colour;
	for (int x = 0; x < tabCount; x++) {
		readObject(tab, in); readObject(colour, in);
		gradIn.changeColour(tab, colour);
	}
	return gradIn;
	};

	CTexGen* src;
	while (readObject(texGenType, in)) {
		switch (texGenType) {
		case texColour:		src = getSourceByIndex();
			texCompositor.createColouriseTex(src, &getColourGradient()); break;
		case texNoise: texCompositor.createNoiseTex(); break;
		case texRidged: texCompositor.createRidgedMFTex(); break;
		case texCylinder: texCompositor.createCylinderTex(); break;
		case texTurbulence: texCompositor.createTurbulenceTex(getSourceByIndex()); break;
		case texScaleBias: texCompositor.createScaleBiasTex(getSourceByIndex()); break;
		case texAdd: texCompositor.createAddTex(getSourceByIndex(), getSourceByIndex()); break;
		case texScalePoint: texCompositor.createScalePointTex(getSourceByIndex()); break;
		}

		texCompositor.texGens.back()->read(in);
	}
	in.close();
	texCompositor.setCurrentLayer(0);
	texCompositor.compose();
	display();
}

void CTexGenUI::clear() {
	texCompositor.texGens.clear();
	texCompositor.targets.clear();
}
