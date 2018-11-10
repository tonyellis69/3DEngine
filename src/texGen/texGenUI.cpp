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

	menu = new CGUImenu(700, 210, 200, 300);
	menu->setGUIcallback(this);
	pApp->GUIroot.Add(menu);


	addTexGenMenu = new CGUImenu(700, 210, 200, 300);
	addTexGenMenu->setGUIcallback(this);
	addTexGenMenu->setVisible(false);
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

	if (sender == menu && msg.Msg == uiMsgMouseMove) {
		//texCompositor.setCurrentLayer(msg.value);
		//update the GUI with the settings for this layer
		//updateGUI();
	}

	if (sender == menu && msg.Msg == uiClick) {
		if (msg.value == texCompositor.texGens.size()) {
			i32vec2 pos = pApp->getMousePos();
			addTexGenMenu->setPos(pos.x,pos.y);
			addTexGenMenu->setVisible(true);
			addTexGenMenu->makeModal();
			return;
		}
		texCompositor.setCurrentLayer(msg.value);
		updateGUI();
	}

	if (sender == addTexGenMenu && msg.Msg == uiClick) {
		addTexGenMenu->makeUnModal();
		addTexGenMenu->setVisible(false);
		addTexGen(msg.value);
	}

	if (sender == addTexGenMenu && msg.Msg == uiClickOutside) {
		addTexGenMenu->makeUnModal();
		addTexGenMenu->setVisible(false);
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


	/*
	ColourGradient granite = { 0, 0, 0, 0, 255,
	8, 0, 0, 0, 255,
	16, 216, 216, 242, 255,
	128, 191, 191, 191, 255,
	191, 210, 116, 125, 255,
	223, 210, 113, 98, 255,
	255, 255, 176, 192, 255 };
	CTexGen* billow = texCompositor.createBillowTex();
	CTexGen* voronoi = texCompositor.createVoronoiTex();
	CTexGen* scaleBias = texCompositor.createScaleBiasTex(voronoi);
	CTexGen* add = texCompositor.createAddTex(billow, scaleBias);
	CTexGen* turb1 = texCompositor.createTurbulenceTex(add);
	texCompositor.createColouriseTex(turb1, &granite);
	*/

	
	//restore();
	
	/*
	ColourGradient slime = { 0, 160, 64, 42, 255,
	127, 64, 192, 64, 255,
	255, 128, 255, 128, 255 };
	CTexGen* billow = texCompositor.createBillowTex();
	CTexGen* billow2 = texCompositor.createBillowTex();
	CTexGen* scaleBias = texCompositor.createScaleBiasTex(billow2);
	CTexGen* multiF = texCompositor.createRidgedMFTex();
	CTexGen* select = texCompositor.createSelectTex(billow,scaleBias, multiF);
	CTexGen* turb1 = texCompositor.createTurbulenceTex(select);
	texCompositor.createColouriseTex(turb1, &slime);
	*/

	/*
	CTexGen* voronoi = texCompositor.createVoronoiTex();
	CTexGen* turb1 = texCompositor.createTurbulenceTex(voronoi);
	CTexGen* billow = texCompositor.createBillowTex();
	CTexGen* turb2 = texCompositor.createTurbulenceTex(billow);
	ColourGradient waterColour = { 0, 48, 64, 192, 255,
								191, 96, 192, 255, 255,
								255, 255, 255, 255, 255 };
	CTexGen* water = texCompositor.createColouriseTex(turb1, &waterColour);

	ColourGradient cloudColour = { 0, 255, 255, 255, 0,
							64, 255,255, 255, 0,
							255, 255, 255, 255, 255 };
	CTexGen* cloud = texCompositor.createColouriseTex(turb2, &cloudColour);
	texCompositor.createLayerTex(water, cloud);
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

}

void CTexGenUI::save() {
	std::ofstream out(pApp->dataPath + "skyTex.gen", std::ios::binary);

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

	std::ifstream in(pApp->dataPath + "skyTex.gen", std::ios::binary);
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
	menu->clear();
	for (auto texGen : texCompositor.texGens) {
		menu->addItem({ texGen->name });
	}
	menu->addItem({ "[Add texgen]" });
}

/** Add the tex gen indicated to the stack. */
void CTexGenUI::addTexGen(int itemNo) {
	switch (itemNo) {
	case 0: texCompositor.createNoiseTex(); break;
	case 1: texCompositor.createColouriseTex(&nullTexGen, NULL); break;
	case 2: texCompositor.createRidgedMFTex(); break;
	case 3: texCompositor.createCylinderTex(); break;
	case 4: texCompositor.createTurbulenceTex(&nullTexGen); break;
	}
	fillMenu();
}
