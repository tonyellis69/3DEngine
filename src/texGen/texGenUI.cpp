#include "texGenUI.h"

#include <iostream>
#include <filesystem>
#include <string>

using namespace glm;
namespace fs = std::experimental::filesystem;

void CTexGenUI::init(CBaseApp * app) {
	pApp = app;
	trackImage = NULL;
	initGUI();
	fillRestoreMenu();
	fillPaletteRestoreMenu();
	shadeSelectionMode = false;

	GUInoiseCtrl->swatchCtrl->assignDataFile(pApp->dataPath + "swatchGroups.txt");
	fileVersion = -1;
}

/** Create the interface for viewing textures. */
void CTexGenUI::initGUI() {
	image = new CGUItrackImage(0, 0, 512, 512);
	image->borderOn(false);
	image->setGUIcallback(this);
	pApp->GUIroot.Add(image);



	GUInoiseCtrl = new CGUInoiseCtrl(700, 430, 380, 360);
	GUInoiseCtrl->setGUIcallback(this);
	pApp->GUIroot.Add(GUInoiseCtrl);
	//pApp->GUIroot.focusControl = GUInoiseCtrl;

	texGenListPanel = new CGUIpanel(700, 80, 380, 290);
	pApp->GUIroot.Add(texGenListPanel);

	CGUIlabel2* listTitle = new CGUIlabel2(10, 10, 100, 20);
	listTitle->setText("Tex gen stack:");
	texGenListPanel->Add(listTitle);

	container = new CGUIcontainer(10, 30, 130, 175);
	texGenListPanel->Add(container);

	CGUIlabel2* lbl = new CGUIlabel2(150, 30, 70, 20);
	lbl->setText("Load file");
	lbl->setHorizontalAlignment(tright);
	texGenListPanel->Add(lbl);
	restoreMenu = new CGUIdropdownMenu(230, 30, 80, 20);
	restoreMenu->setText("[select]");
	restoreMenu->setGUIcallback(this);
	texGenListPanel->Add(restoreMenu);

	saveButton = new CGUIbutton2(150, 55, 70, 20);
	saveButton->setText("Save as");
	saveButton->setGUIcallback(this);
	texGenListPanel->Add(saveButton);
	fileName = new CGUItextbox2(230, 55, 80, 20);
	fileName->setText("TexGen1");
	texGenListPanel->Add(fileName);


	texGenList = new CGUImenu(0, 0, 130, 300);
	texGenList->setGUIcallback(this);
	container->Add(texGenList);

	thumbnailImage = new CGUIimage(150, 80, 200, 200);
	texGenListPanel->Add(thumbnailImage);


	addTexGenMenu = new CGUImenu(700, 210, 200, 300);
	addTexGenMenu->setGUIcallback(this);
	addTexGenMenu->setVisible(false);
	addTexGenMenu->resizeHorizontal = true;
	addTexGenMenu->drawBorder = true;
	addTexGenMenu->addItem({ "Noise","Colourise","RidgedMF","Cylinder","Turbulence","ScaleBias",
		"Add","Seamless","ScalePoint","Billow","Voronoi","Select","Layer","Gaus",
		"Rects", "Blocks" });
	pApp->GUIroot.Add(addTexGenMenu);





}

/** Callback from the palette bar and noise control. */
void CTexGenUI::GUIcallback(CGUIbase * sender, CMessage & msg) {
	if (sender == GUInoiseCtrl->paletteBar && msg.Msg == uiMsgUpdate) {
		texCompositor.updateColourGradient(GUInoiseCtrl->paletteBar->colourGradient);
		texCompositor.currentTexGen->render();
		if (trackImage)
			trackImage->render();
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


	if (sender == texGenList && msg.Msg == uiMsgRMdown) {
		pApp->GUIroot.popupMenu({ "Delete", "Track/untrack image" },this);
		/////////deleteTexGen(msg.value);
		return;
	}

	if (sender == texGenList && msg.Msg == uiClick) {
		if (msg.value == texCompositor.texGens.size()) { //clicked on 'add TexGen'
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
		texCompositor.compose();
	}
	if (sender == GUInoiseCtrl->src2Ctrl && msg.Msg == uiClick) {
		texCompositor.currentTexGen->setSource2(texCompositor.texGens[msg.value]);
		texCompositor.compose();
	}
	if (sender == GUInoiseCtrl->src3Ctrl && msg.Msg == uiClick) {
		texCompositor.currentTexGen->setSource3(texCompositor.texGens[msg.value]);
		texCompositor.compose();
	}


	if (sender == saveButton) {
		save();
		return;
	}

	if (sender == restoreMenu && msg.Msg == uiClick) {
		restore(restoreMenu->getItemName(msg.value));
		return;
	}

	if (sender == GUInoiseCtrl->paletteBar) {
		if (msg.Msg == uiMsgSave) {
			savePalette(GUInoiseCtrl->paletteBar->fileNameBox->text);
		}
		if (msg.Msg == uiMsgRestore)
			restorePalette(GUInoiseCtrl->paletteBar->restoreMenu->getItemName(msg.value));
		return;
	}

	if (sender == GUInoiseCtrl->gausSizeCtrl) {
		texCompositor.currentTexGen->setGausSize(msg.fValue);
		texCompositor.compose();
	}
	if (sender == GUInoiseCtrl->gausSigmaCtrl) {
		texCompositor.currentTexGen->setGausSigma(msg.fValue);
		texCompositor.compose();
	}

	if (sender == GUInoiseCtrl->percentageCtrl) {
		texCompositor.currentTexGen->setPercentage(msg.fValue);
		texCompositor.compose();
	}

	if (sender == image && msg.Msg == uiMsgMouseMove) {
		//if current texgen is a colouriser... go to the colour-tracking routine!
		if (shadeSelectionMode && texCompositor.currentTexGen->texGenType == texColour)
			highlightMouseColour(msg.x, msg.y);
	}

	if (sender == image && msg.Msg == uiMsgMouseOff) {
		if (/*texCompositor.currentTexGen &&*/ texCompositor.currentTexGen->texGenType == texColour)
			unhighlightMouseColour();
	}

	if (sender == image && msg.Msg == uiMsgRMdown) {
		if (texCompositor.currentTexGen->texGenType == texColour) {
			shadeSelectionMode = !shadeSelectionMode;
			if (!shadeSelectionMode)
				unhighlightMouseColour();
			else
				highlightMouseColour(msg.x, msg.y);
		}
	}

	if (sender == image && msg.Msg == uiMouseWheel) {
		if (shadeSelectionMode && texCompositor.currentTexGen->texGenType == texColour) {
			static_cast<CColourTex*>(texCompositor.currentTexGen)->adjustFalloff(msg.value);
		}
	}

	if (sender == image && msg.Msg == uiMsgDoubleClick) {
		if (shadeSelectionMode && texCompositor.currentTexGen->texGenType == texColour) {
			if (msg.value == GLFW_MOD_CONTROL) {
				float falloff = static_cast<CColourTex*>(texCompositor.currentTexGen)->selectionFalloff;
				GUInoiseCtrl->paletteBar->createTabAtIndicator(falloff);
			}
			else
				GUInoiseCtrl->paletteBar->createTabAtIndicator(0.0f);
		}
	}

	if (texCompositor.currentTexGen->texGenType == texRects) {
		if (sender == GUInoiseCtrl->rectsDepthCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setDepth(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->rectsVarianceCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setVariance(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->freqCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setFrequency(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->octaveCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setOctaves(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->rectsStyleCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setStyle(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->clustersCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setClusters(msg.fValue);
			texCompositor.compose();
		}
	}

	if (texCompositor.currentTexGen->texGenType == texBlocks) {
		if (sender == GUInoiseCtrl->blocksStagesCtrl) {
			static_cast<CBlocksTex*>(texCompositor.currentTexGen)->setStages(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->blocksIterationsCtrl) {
			static_cast<CBlocksTex*>(texCompositor.currentTexGen)->setIterations(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->blocksDensityCtrl) {
			static_cast<CBlocksTex*>(texCompositor.currentTexGen)->setDensity(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->blocksScaleCtrl) {
			static_cast<CBlocksTex*>(texCompositor.currentTexGen)->setScale(msg.fValue);
			texCompositor.compose();
		}

	}
	updateDisplayImage();

}


void CTexGenUI::hide(bool onOff) {
	image->setVisible(!onOff);
	texGenListPanel->setVisible(!onOff);
	GUInoiseCtrl->setVisible(!onOff);
}

void CTexGenUI::compose() {
	texCompositor.initTex();

	//restore();

	texCompositor.setCurrentLayer(0);
	texCompositor.compose();
	updateGUI();
	fillStackMenu();
	updateDisplayImage();
}


void CTexGenUI::updateDisplayImage() {
	if (texCompositor.texGens.size() == 0)
		return;
	if (trackImage)
		image->setTexture(*trackImage->getTarget());
	else
		image->setTexture(*texCompositor.currentTarget);
	thumbnailImage->setTexture(*texCompositor.currentTarget);
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
		GUInoiseCtrl->paletteBar->setGradient(*texGenColourGrad);

	GUInoiseCtrl->lowerCtrl->setValue(texCompositor.currentTexGen->getLowerBound());
	GUInoiseCtrl->upperCtrl->setValue(texCompositor.currentTexGen->getUpperBound());
	GUInoiseCtrl->falloffCtrl->setValue(texCompositor.currentTexGen->getFalloff());

	GUInoiseCtrl->widthCtrl->setValue(texCompositor.currentTexGen->getSampleWidth());
	GUInoiseCtrl->heightCtrl->setValue(texCompositor.currentTexGen->getSampleHeight());

	if (texCompositor.currentTexGen->srcTex1)
		GUInoiseCtrl->src1Ctrl->setText(texCompositor.currentTexGen->srcTex1->name);
	if (texCompositor.currentTexGen->srcTex2)
		GUInoiseCtrl->src2Ctrl->setText(texCompositor.currentTexGen->srcTex2->name);
	if (texCompositor.currentTexGen->srcTex3)
		GUInoiseCtrl->src3Ctrl->setText(texCompositor.currentTexGen->srcTex3->name);

	GUInoiseCtrl->gausSizeCtrl->setValue(texCompositor.currentTexGen->getGausSize());
	GUInoiseCtrl->gausSigmaCtrl->setValue(texCompositor.currentTexGen->getGausSigma());

	GUInoiseCtrl->percentageCtrl->setValue(texCompositor.currentTexGen->getpercentageCtrl());

	if (texCompositor.currentTexGen->texGenType == texRects) {
		GUInoiseCtrl->rectsDepthCtrl->setValue(static_cast<CRectsTex*>(texCompositor.currentTexGen)->getDepth());
		GUInoiseCtrl->rectsVarianceCtrl->setValue(static_cast<CRectsTex*>(texCompositor.currentTexGen)->getVariance());
		GUInoiseCtrl->rectsStyleCtrl->setValue(static_cast<CRectsTex*>(texCompositor.currentTexGen)->getStyle());
		GUInoiseCtrl->clustersCtrl->setValue(static_cast<CRectsTex*>(texCompositor.currentTexGen)->getClusters());
	}
	if (texCompositor.currentTexGen->texGenType == texBlocks) {
		GUInoiseCtrl->blocksStagesCtrl->setValue(static_cast<CBlocksTex*>(texCompositor.currentTexGen)->getStages());
		GUInoiseCtrl->blocksIterationsCtrl->setValue(static_cast<CBlocksTex*>(texCompositor.currentTexGen)->getIterations());
		GUInoiseCtrl->blocksDensityCtrl->setValue(static_cast<CBlocksTex*>(texCompositor.currentTexGen)->getDensity());
		GUInoiseCtrl->blocksScaleCtrl->setValue(static_cast<CBlocksTex*>(texCompositor.currentTexGen)->getScale());
	}

}
/*
void CTexGenUI::save() {
	save2();
	std::ofstream out(pApp->dataPath + fileName->text + ".gen", std::ios::binary);

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

	fillRestoreMenu();
}
*/
void CTexGenUI::save() {
	std::ofstream out(pApp->dataPath + fileName->text + ".gen", std::ios::binary);
	out << "t1"; //identifier

	for (auto texGen : texCompositor.texGens) //gives the overall composition of the stack
		writeObject(texGen->texGenType, out);
	writeObject(texNull, out);

	auto writeTexIndex = [&](CTexGen* texGen) {
		if (texGen == NULL) {
			writeObject((char)-1, out);	return;
		}
		for (char x = 0; x < (char)texCompositor.texGens.size(); x++) {
			if (texGen == texCompositor.texGens[x]) {
				writeObject(x, out);	return;
			}
		}

	};

	for (auto texGen : texCompositor.texGens) { //write index of sources
		writeTexIndex(texGen->srcTex1);
		writeTexIndex(texGen->srcTex2);
		writeTexIndex(texGen->srcTex3);
		texGen->write(out);
	}
	out.close();

	fillRestoreMenu();
}


#if OLDRESTORE 
void CTexGenUI::restore(std::string& filename) {
	clear();
	int finalColourise = 0;
	trackImage = NULL;

	std::ifstream in(pApp->dataPath + filename + ".gen", std::ios::binary);
	TexGenType texGenType;

	struct TPatch { unsigned int idx;  unsigned int srcIdx; };
	vector<TPatch> patches;
	auto getSourceByIndex = [&]() { int index; readObject(index, in); 
		if (index >= texCompositor.texGens.size()) {
			patches.push_back({ texCompositor.texGens.size() ,(unsigned int)index });
			return (CTexGen*)&nullTexGen;
		}
		return texCompositor.texGens[index];
	};
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
			texCompositor.createColouriseTex(src, &getColourGradient()); 
			finalColourise = texCompositor.texGens.size() - 1;
			break;
		case texNoise: texCompositor.createNoiseTex(); break;
		case texVoronoi: texCompositor.createVoronoiTex(); break;
		case texBillow: texCompositor.createBillowTex(); break;
		case texRidged: texCompositor.createRidgedMFTex(); break;
		case texCylinder: texCompositor.createCylinderTex(); break;
		case texTurbulence: texCompositor.createTurbulenceTex(getSourceByIndex()); break;
		case texScaleBias: texCompositor.createScaleBiasTex(getSourceByIndex()); break;
		case texSeamless: src = getSourceByIndex(); texCompositor.createSeamlessTex()->setSource(src); break;
		case texAdd: texCompositor.createAddTex(getSourceByIndex(), getSourceByIndex()); break;
		case texScalePoint: texCompositor.createScalePointTex(getSourceByIndex()); break;
		case texSelect:
			src = getSourceByIndex(); src2 = getSourceByIndex(); src3 = getSourceByIndex();
			texCompositor.createSelectTex(src, src2, src3); break;
		case texLayer: texCompositor.createLayerTex(getSourceByIndex(), getSourceByIndex()); break;
		case texGaus: src = getSourceByIndex(); texCompositor.createGausTex()->setSource(src); break;
		case texRects: src = getSourceByIndex(); texCompositor.createRectsTex()->setSource(src); break;
		}

		texCompositor.texGens.back()->read(in);
	}
	in.close();

	texCompositor.compose();

	for (auto patch : patches) {
		CTexGen* texGen = texCompositor.texGens[patch.idx];
		if (texGen->srcTex1 == &nullTexGen)
			texGen->setSource(texCompositor.texGens[patch.srcIdx]);
		else if (texGen->srcTex2 == &nullTexGen)
			texGen->setSource2(texCompositor.texGens[patch.srcIdx]);
		else if (texGen->srcTex3 == &nullTexGen)
			texGen->setSource3(texCompositor.texGens[patch.srcIdx]);
		texGen->render();
	}

	texCompositor.setCurrentLayer(finalColourise);
	updateDisplayImage();

	fillStackMenu();
	fileName->setText(filename);
}
#else


void CTexGenUI::restore(std::string& filename) {
	clear();
	trackImage = NULL;

	std::ifstream in(pApp->dataPath + filename + ".gen", std::ios::binary);
	if (in.fail() || in.get() != 't') {
		cerr << "\nUnable to open tex file " << filename;
		return;
	}

	fileVersion = int(in.get());

	//find and load all the texgens
	TexGenType texGenType;
	while (readObject(texGenType, in)) {
		if (texGenType == texNull)
			break;
		switch (texGenType) {
		case texColour:		texCompositor.createColouriseTex(); break;
		case texNoise: texCompositor.createNoiseTex(); break;
		case texVoronoi: texCompositor.createVoronoiTex(); break;
		case texBillow: texCompositor.createBillowTex(); break;
		case texRidged: texCompositor.createRidgedMFTex(); break;
		case texCylinder: texCompositor.createCylinderTex(); break;
		case texTurbulence: texCompositor.createTurbulenceTex(); break;
		case texScaleBias: texCompositor.createScaleBiasTex(); break;
		case texSeamless:  texCompositor.createSeamlessTex(); break;
		case texAdd: texCompositor.createAddTex(); break;
		case texScalePoint: texCompositor.createScalePointTex(); break;
		case texSelect: texCompositor.createSelectTex(); break;
		case texLayer: texCompositor.createLayerTex(); break;
		case texGaus: texCompositor.createGausTex(); break;
		case texRects: texCompositor.createRectsTex(); break;
		case texBlocks: texCompositor.createBlocksTex(); break;
		}
	}

	auto getColourGradient = [&]() { ColourGradient gradIn; int tabCount; readObject(tabCount, in);
	int tab; i32vec4 colour;
	for (int x = 0; x < tabCount; x++) {
		readObject(tab, in); readObject(colour, in);
		gradIn.changeColour(tab, colour);
	}
	return gradIn;
	};

	//read sources and data into texgens
	char x;
	for (auto texGen : texCompositor.texGens) {
		x = in.get(); 
		if (x != -1)
			texGen->setSource(texCompositor.texGens[x]);
		x = in.get(); 
		if (x != -1)
			texGen->setSource2(texCompositor.texGens[x]);
		x = in.get(); 
		if (x != -1) 
			texGen->setSource3(texCompositor.texGens[x]);
		if (texGen->texGenType == texColour) {
			texGen->setPalette(getColourGradient());
		}
		else
			texGen->read(in);
	}
	in.close();

	texCompositor.compose();
	texCompositor.compose(); //ugh. Ensures texgens with forward references are up-to-date
	texCompositor.setCurrentLayer(texCompositor.texGens.size()-1);
	configureGUI(texCompositor.currentTexGen->texGenType);
	updateGUI();
	updateDisplayImage();
	fillStackMenu();
	fileName->setText(filename);
}
#endif

void CTexGenUI::clear() {
	texCompositor.texGens.clear();
	//texCompositor.targets.clear();
}

/** Fill the menu with the names of the tex gens on the stack.*/
void CTexGenUI::fillStackMenu() {
	texGenList->clear();
	GUInoiseCtrl->src1Ctrl->clear();
	GUInoiseCtrl->src2Ctrl->clear();
	GUInoiseCtrl->src3Ctrl->clear();
	int itemNo = 0;
	for (auto texGen : texCompositor.texGens) {
		texGenList->addItem({ texGen->name });
		GUInoiseCtrl->src1Ctrl->addItem({ texGen->name });
		GUInoiseCtrl->src2Ctrl->addItem({ texGen->name });
		GUInoiseCtrl->src3Ctrl->addItem({ texGen->name });
		if (trackImage == texGen)
			texGenList->setHighlight(itemNo, true);
		else
			itemNo++;
	}
	texGenList->addItem({ "[Add texgen]" });
}

/** Add the tex gen indicated to the stack. */
void CTexGenUI::addTexGen(int itemNo) {
	switch (itemNo) {
	case 0: texCompositor.createNoiseTex(); break;
	case 1: texCompositor.createColouriseTex(); break;
	case 2: texCompositor.createRidgedMFTex(); break;
	case 3: texCompositor.createCylinderTex(); break;
	case 4: texCompositor.createTurbulenceTex(); break;
	case 5: texCompositor.createScaleBiasTex(); break;
	case 6: texCompositor.createAddTex(); break;
	case 7: texCompositor.createSeamlessTex(); break;
	case 8: texCompositor.createScalePointTex(); break;
	case 9: texCompositor.createBillowTex(); break;
	case 10: texCompositor.createVoronoiTex(); break;
	case 11: texCompositor.createSelectTex(); break;
	case 12: texCompositor.createLayerTex(); break;
	case 13: texCompositor.createGausTex(); break;
	case 14: texCompositor.createRectsTex(); break;
	case 15: texCompositor.createBlocksTex(); break;
	}
	texCompositor.setCurrentLayer(texCompositor.texGens.size() - 1);
	configureGUI(texCompositor.currentTexGen->texGenType);
	updateGUI();
	fillStackMenu();
	texCompositor.compose();
	updateDisplayImage();
}

/** Remove the indicated tex gen from the stack. */
void CTexGenUI::deleteTexGen(int itemNo) {
	if (texCompositor.texGens.size() == 0 || itemNo == texCompositor.texGens.size())
		return;
	texCompositor.deleteTexGen(itemNo);
	fillStackMenu();
	texCompositor.compose();
	updateGUI();
	updateDisplayImage();
}

/** Configure the GUI to show only the controls for the given layer. */
void CTexGenUI::configureGUI(TexGenType texType) {
	//first clear all controls
	GUInoiseCtrl->hideAllRows();
	switch (texType) {
	case texNoise: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight" }); break;
	case texColour:  GUInoiseCtrl->activateRows({ "source1", "paletteBar", "swatches" }); break;
	case texRidged: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight" }); break;
	case texCylinder: GUInoiseCtrl->activateRows({ "freq","rotation","translation" }); break;
	case texTurbulence: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight",
		"source1" }); break;
	case texScaleBias: GUInoiseCtrl->activateRows({ "scaleBias","source1" }); break;
	case texAdd: GUInoiseCtrl->activateRows({ "source1&2" }); break;
	case texSeamless: GUInoiseCtrl->activateRows({ "source1","seamPercentFalloff" }); break;;
	case texScalePoint: GUInoiseCtrl->activateRows({ "source1", "scalePoint" }); break;
	case texBillow: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight" }); break;
	case texVoronoi: GUInoiseCtrl->activateRows({ "freq","sampWidthHeight", "distHue" }); break;
	case texSelect: GUInoiseCtrl->activateRows({ "lowerUpper","falloff","source1&2","source3" }); break;
	case texLayer: GUInoiseCtrl->activateRows({ "source1&2" }); break;
	case texGaus: GUInoiseCtrl->activateRows({ "source1", "gausSizeSigma"}); break;
	case texRects: GUInoiseCtrl->activateRows({ "rectsDepthVariance","octave","freq","styleClusters" }); break;
	case texBlocks: GUInoiseCtrl->activateRows({ "stagesIterations","densityScale" }); break;

	}
	GUInoiseCtrl->autoArrangeRows(0, 20);
}

/** Populate the restore menu with the names of all the texgen composition files that can be found. */
void CTexGenUI::fillRestoreMenu() {
	restoreMenu->clear();
	//search for ".gen" files
	for (auto& p : fs::directory_iterator(pApp->dataPath)) {
		std::string path = p.path().string();
		if (path.compare(path.size() - 4, 4, ".gen") == 0) {
			restoreMenu->addItem({ p.path().stem().string() });
		}
	}
}

/** Handle a paletteBar request to save the named palette. */
void CTexGenUI::savePalette(std::string filename) {
	std::ofstream out(pApp->dataPath + filename + ".pal", std::ios::binary);

	ColourGradient* colourGradient = &GUInoiseCtrl->paletteBar->colourGradient;
	int tabCount = colourGradient->tabs.size();
	writeObject(tabCount, out);
	for (auto tab : colourGradient->tabs) {
		writeObject(tab.first, out);
		writeObject(tab.second, out);
	}
	out.close();
	fillPaletteRestoreMenu();
}

void CTexGenUI::fillPaletteRestoreMenu() {
	GUInoiseCtrl->paletteBar->restoreMenu->clear();
	//search for ".pal" files
	for (auto& p : fs::directory_iterator(pApp->dataPath)) {
		std::string path = p.path().string();
		if (path.compare(path.size() - 4, 4, ".pal") == 0) {
			GUInoiseCtrl->paletteBar->restoreMenu->addItem({ p.path().stem().string() });
		}
	}
}

void CTexGenUI::restorePalette(std::string & filename){
	std::ifstream in(pApp->dataPath + filename + ".pal", std::ios::binary);

	ColourGradient gradIn; 
	int tabCount;
	readObject(tabCount, in);
	int tab; i32vec4 colour;
	for (int x = 0; x < tabCount; x++) {
		readObject(tab, in); readObject(colour, in);
		gradIn.changeColour(tab, colour);
	}
	GUInoiseCtrl->paletteBar->setGradient(gradIn);

	texCompositor.updateColourGradient(gradIn);
	texCompositor.currentTexGen->render();

}

/** Highlight the colour at the given point on the source texture of the current colourise texgen. */
void CTexGenUI::highlightMouseColour(int x, int y) {
	//ask the colourise texgen for the colour at this point
	i32vec4 colour = texCompositor.currentTexGen->getSourceColour(x, y);
	
	//tell the paletteBar to highlight this colour on the gradient
	GUInoiseCtrl->setIndicatorPosition(colour.r);

	//tell colouriser to display a faded-out version of the texture
	static_cast<CColourTex*>(texCompositor.currentTexGen)->setSelectedShade(colour.r);
	texCompositor.currentTexGen->render();
}

void CTexGenUI::unhighlightMouseColour() {
	GUInoiseCtrl->setIndicatorPosition(-1);
	static_cast<CColourTex*>(texCompositor.currentTexGen)->setSelectedShade(-1);
	texCompositor.currentTexGen->render();
}

void CTexGenUI::popupMenuCallback(int choice) {
	int layer = texGenList->focusItem;
	if (layer == -1)
		return;
	if (choice == 0) { //delete
		if (trackImage == texCompositor.texGens[layer])
			trackImage = NULL;
		deleteTexGen(layer);
	}
	if (choice == 1)
		trackLayerImage(layer);
		
}

/** Toggle whether we track this layer as the main image, and highlight it in the layer stack. */
void CTexGenUI::trackLayerImage(int layer) {
	if (trackImage == texCompositor.texGens[layer]) {
		trackImage = NULL;
		texGenList->setHighlight(layer, false);
	}
	else {
		for (int trackedNo = 0; trackedNo < texCompositor.texGens.size(); trackedNo++)
			if (trackImage == texCompositor.texGens[trackedNo]) {
				texGenList->setHighlight(trackedNo, false); 
				break;
			}
		trackImage = texCompositor.texGens[layer];
		texGenList->setHighlight(layer, true);
	}
}