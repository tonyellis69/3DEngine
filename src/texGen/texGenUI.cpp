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

	paletteBar = new CGUIpaletteBar(700, 10, 280, 180);
	pApp->GUIroot.Add(paletteBar);
	paletteBar->loadPalette();

	paletteBar->setGUIcallback(this);

	GUInoiseCtrl = new CGUInoiseCtrl(700, 430, 380, 360);
	GUInoiseCtrl->setGUIcallback(this);
	pApp->GUIroot.Add(GUInoiseCtrl);
	//pApp->GUIroot.focusControl = GUInoiseCtrl;

	texGenListPanel = new CGUIpanel(700, 210, 150, 210);
	pApp->GUIroot.Add(texGenListPanel);

	CGUIlabel2* listTitle = new CGUIlabel2(10, 10, 100, 20);
	listTitle->setText("Tex gen stack:");
	texGenListPanel->Add(listTitle);

	container = new CGUIcontainer(10, 30, 130, 175);
	texGenListPanel->Add(container);

	texGenList = new CGUImenu(0, 0, 130, 300);
	texGenList->setGUIcallback(this);
	//texGenListPanel->Add(texGenList);
	container->Add(texGenList);


	addTexGenMenu = new CGUImenu(700, 210, 200, 300);
	addTexGenMenu->setGUIcallback(this);
	addTexGenMenu->setVisible(false);
	addTexGenMenu->resizeHorizontal = true;
	addTexGenMenu->drawBorder = true;
	addTexGenMenu->addItem({ "Noise","Colourise","RidgedMF","Cylinder","Turbulence","ScaleBias",
		"Add","Seamless","ScalePoint","Billow","Voronoi","Select","Layer" });
	pApp->GUIroot.Add(addTexGenMenu);


}

/** Callback from the palette bar and noise control. */
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

	if (sender == GUInoiseCtrl->persistCtrl) {
		texCompositor.updatePersistence(msg.fValue);
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

	if (sender == GUInoiseCtrl->distCtrl) {
		texCompositor.updateDistance(msg.value);
	}

	if (sender == GUInoiseCtrl->randHueCtrl) {
		texCompositor.updateRandomHue(msg.value);
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

	if (sender == GUInoiseCtrl->lowerCtrl) {
		texCompositor.updateLower(msg.fValue);
	}

	if (sender == GUInoiseCtrl->upperCtrl) {
		texCompositor.updateUpper(msg.fValue);
	}

	if (sender == GUInoiseCtrl->falloffCtrl) {
		texCompositor.updateFalloff(msg.fValue);
	}

	if (sender == GUInoiseCtrl->widthCtrl) {
		texCompositor.updateSampleWidth(msg.fValue);
	}

	if (sender == GUInoiseCtrl->heightCtrl) {
		texCompositor.updateSampleHeight(msg.fValue);
	}

	//cycling through layers
	if (sender == GUInoiseCtrl && msg.Msg == uiMouseWheel) {
		texCompositor.scrollStack(msg.value);
		updateGUI();
	}

	if (sender == texGenList && msg.Msg == uiMsgMouseMove) {
		//texCompositor.setCurrentLayer(msg.value);
		//update the GUI with the settings for this layer
		//updateGUI();
	}

	if (sender == texGenList && msg.Msg == uiClick) {
		if (msg.value == texCompositor.texGens.size()) {
			i32vec2 pos = pApp->getMousePos();
			addTexGenMenu->setPos(pos.x,pos.y);
			addTexGenMenu->setVisible(true);
			addTexGenMenu->makeModal();
			return;
		}
		texCompositor.setCurrentLayer(msg.value);
		configureGUI(texCompositor.currentTexGen->texGenType);
		updateGUI();
	}

	if (sender == addTexGenMenu && msg.Msg == uiClick) {
		addTexGenMenu->makeUnModal();
		addTexGenMenu->setVisible(false);
		addTexGen(msg.value);
		return;
	}

	if (sender == addTexGenMenu && msg.Msg == uiClickOutside) {
		addTexGenMenu->makeUnModal();
		addTexGenMenu->setVisible(false);
		return;
	}

	if (sender == GUInoiseCtrl->src1Ctrl && msg.Msg == uiClick) {
		texCompositor.currentTexGen->setSource(texCompositor.texGens[msg.value]);
		texCompositor.currentTexGen->render();
	}


	display();

	if (sender == GUInoiseCtrl->saveButton) {
		save();
	}
}

void CTexGenUI::hide(bool onOff) {
	image->setVisible(!onOff);
	paletteBar->setVisible(!onOff);
	GUInoiseCtrl->setVisible(!onOff);
}

void CTexGenUI::compose() {
	texCompositor.initTex();

	//restore();

	texCompositor.setCurrentLayer(0);
	texCompositor.compose();
	updateGUI();
	fillMenu();
	display();
}


void CTexGenUI::display() {
	if (texCompositor.texGens.size() > 0)
		image->setTexture(*texCompositor.currentTarget);
}

/** Update the GUI with the control settings for the current tex gen layer. */
void CTexGenUI::updateGUI() {
	if (texCompositor.texGens.size() == 0)
		return;
	GUInoiseCtrl->octaveCtrl->setValue(texCompositor.currentTexGen->getOctaves());
	GUInoiseCtrl->freqCtrl->setValue(texCompositor.currentTexGen->getFrequency());
	GUInoiseCtrl->persistCtrl->setValue(texCompositor.currentTexGen->getPersistence());
	GUInoiseCtrl->powerCtrl->setValue(texCompositor.currentTexGen->getPower());
	GUInoiseCtrl->scaleCtrl->setValue(texCompositor.currentTexGen->getScale());
	GUInoiseCtrl->biasCtrl->setValue(texCompositor.currentTexGen->getBias());
	GUInoiseCtrl->distCtrl->Set = texCompositor.currentTexGen->getDistance();
	GUInoiseCtrl->randHueCtrl->Set = texCompositor.currentTexGen->getRandomHue();

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

	GUInoiseCtrl->lowerCtrl->setValue(texCompositor.currentTexGen->getLowerBound());
	GUInoiseCtrl->upperCtrl->setValue(texCompositor.currentTexGen->getUpperBound());
	GUInoiseCtrl->falloffCtrl->setValue(texCompositor.currentTexGen->getFalloff());

	GUInoiseCtrl->widthCtrl->setValue(texCompositor.currentTexGen->getSampleWidth());
	GUInoiseCtrl->heightCtrl->setValue(texCompositor.currentTexGen->getSampleHeight());

	if (texCompositor.currentTexGen->srcTex1)
		GUInoiseCtrl->src1Ctrl->setText(texCompositor.currentTexGen->srcTex1->name);

}

void CTexGenUI::save() {
	std::ofstream out(pApp->dataPath + "testTex.gen", std::ios::binary);

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
		if (texGen->srcTex3)
			writeTexIndex(texGen->srcTex3);
		texGen->write(out);

	}
	out.close();


}

void CTexGenUI::restore() {
	clear();

	std::ifstream in(pApp->dataPath + "testTex.gen", std::ios::binary);
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

	CTexGen* src, *src2, *src3;
	while (readObject(texGenType, in)) {
		switch (texGenType) {
		case texColour:		src = getSourceByIndex();
			texCompositor.createColouriseTex(src, &getColourGradient()); break;
		case texNoise: texCompositor.createNoiseTex(); break;
		case texVoronoi: texCompositor.createVoronoiTex(); break;
		case texBillow: texCompositor.createBillowTex(); break;
		case texRidged: texCompositor.createRidgedMFTex(); break;
		case texCylinder: texCompositor.createCylinderTex(); break;
		case texTurbulence: texCompositor.createTurbulenceTex(getSourceByIndex()); break;
		case texScaleBias: texCompositor.createScaleBiasTex(getSourceByIndex()); break;
		case texAdd: texCompositor.createAddTex(getSourceByIndex(), getSourceByIndex()); break;
		case texScalePoint: texCompositor.createScalePointTex(getSourceByIndex()); break;
		case texSelect:
			src = getSourceByIndex(); src2 = getSourceByIndex(); src3 = getSourceByIndex();
			texCompositor.createSelectTex(src, src2, src3); break;
		case texLayer: texCompositor.createLayerTex(getSourceByIndex(), getSourceByIndex()); break;
		}

		texCompositor.texGens.back()->read(in);
	}
	in.close();
	texCompositor.setCurrentLayer(0);
	texCompositor.compose();
	display();

	fillMenu();
}

void CTexGenUI::clear() {
	texCompositor.texGens.clear();
	texCompositor.targets.clear();
}

/** Fill the menu with the names of the tex gens on the stack.*/
void CTexGenUI::fillMenu() {
	texGenList->clear();
	GUInoiseCtrl->src1Ctrl->clear();
	for (auto texGen : texCompositor.texGens) {
		texGenList->addItem({ texGen->name });
		GUInoiseCtrl->src1Ctrl->addItem({ texGen->name });
	}
	texGenList->addItem({ "[Add texgen]" });
}

/** Add the tex gen indicated to the stack. */
void CTexGenUI::addTexGen(int itemNo) {
	switch (itemNo) {
	case 0: texCompositor.createNoiseTex(); break;
	case 1: texCompositor.createColouriseTex(&nullTexGen, NULL); break;
	case 2: texCompositor.createRidgedMFTex(); break;
	case 3: texCompositor.createCylinderTex(); break;
	case 4: texCompositor.createTurbulenceTex(&nullTexGen); break;
	case 5: texCompositor.createScaleBiasTex(&nullTexGen); break;
	case 6: texCompositor.createAddTex(&nullTexGen, &nullTexGen); break;
	case 8: texCompositor.createScalePointTex(&nullTexGen); break;
	case 9: texCompositor.createBillowTex(); break;
	case 10: texCompositor.createVoronoiTex(); break;
	case 11: texCompositor.createSelectTex(&nullTexGen, &nullTexGen, &nullTexGen); break;
	case 12: texCompositor.createLayerTex(&nullTexGen, &nullTexGen); break;

	}
	texCompositor.setCurrentLayer(texCompositor.texGens.size() - 1);
	configureGUI(texCompositor.currentTexGen->texGenType);
	updateGUI();
	fillMenu();
	texCompositor.compose();
	display();
}

/** Configure the GUI to show only the controls for the given layer. */
void CTexGenUI::configureGUI(TexGenType texType) {
	//first clear all controls
	GUInoiseCtrl->hideAllRows();
	switch (texType) {
	case texNoise: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight"}); break;
	case texColour: break;
	case texRidged: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight" }); break;
	case texCylinder: GUInoiseCtrl->activateRows({ "freq","rotation" }); break;
	case texTurbulence: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight",
		"source1"}); break;
	case texScaleBias: GUInoiseCtrl->activateRows({ "scaleBias" }); break;
	case texAdd: break;
	case texSeamless: break;
	case texScalePoint: GUInoiseCtrl->activateRows({ "scalePoint" }); break;
	case texBillow: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight" }); break;
	case texVoronoi: GUInoiseCtrl->activateRows({ "freq","sampWidthHeight", "distHue"}); break;
	case texSelect: GUInoiseCtrl->activateRows({ "lowerUpper","falloff" }); break;
	case texLayer: break;
	}
	GUInoiseCtrl->activateRows({ "save" });
	GUInoiseCtrl->autoArrangeRows(0, 20);
}
