#include "texGenUI.h"

#include <iostream>
#include <experimental/filesystem>
#include <string>
#include <iomanip>

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
	
}

/** Create the interface for viewing textures. */
void CTexGenUI::initGUI() {
	image = new CGUItrackImage(0, 0, 512, 512);
	image->setBorderOn(false);
	image->setGUIcallback(this);
	pApp->GUIroot.add(image);


	//got here

	GUInoiseCtrl = new CGUInoiseCtrl(700, 430, 380, 360);

	GUInoiseCtrl->setGUIcallback(this);

	pApp->GUIroot.add(GUInoiseCtrl);
	//pApp->GUIroot.focusControl = GUInoiseCtrl;

	texGenListPanel = new CGUIpanel(700, 80, 380, 290);
	pApp->GUIroot.add(texGenListPanel);

	CGUIlabel2* listTitle = new CGUIlabel2(10, 10, 100, 20);
	
	listTitle->setText("Tex gen stack:");

	texGenListPanel->add(listTitle);
	

	container = new CGUIcontainer(10, 30, 130, 175);
	texGenListPanel->add(container);

	//didn't get here

	CGUIlabel2* lbl = new CGUIlabel2(150, 30, 70, 20);
	lbl->setText("Load file");
	lbl->setTextAlignment(tright);
	texGenListPanel->add(lbl);
	restoreMenu = new CGUIdropdownMenu(230, 30, 80, 20);
	restoreMenu->setText("[select]");
	restoreMenu->setGUIcallback(this);
	texGenListPanel->add(restoreMenu);

	saveButton = new CGUIbutton2(150, 55, 70, 20);
	saveButton->setText("Save as");
	saveButton->setGUIcallback(this);
	texGenListPanel->add(saveButton);
	fileName = new CGUItextbox2(230, 55, 80, 20);
	fileName->setText("TexGen1");
	texGenListPanel->add(fileName);


	texGenList = new CGUImenu(0, 0, 130, 300);
	texGenList->setGUIcallback(this);
	container->add(texGenList);

	thumbnailImage = new CGUIimage(150, 80, 200, 200);
	texGenListPanel->add(thumbnailImage);


	addTexGenMenu = new CGUImenu(700, 210, 200, 300);
	addTexGenMenu->setGUIcallback(this);
	addTexGenMenu->setVisible(false);
	addTexGenMenu->resizeHorizontal = true;
	addTexGenMenu->drawBorder = true;
	addTexGenMenu->addItem({ "Noise","Colourise","RidgedMF","Cylinder","Turbulence","ScaleBias",
		"add","Seamless","ScalePoint","Billow","Voronoi","Select","Layer","Gaus",
		"Rects", "Blocks", "Cover", "Cutup", "Terrain" });
	pApp->GUIroot.add(addTexGenMenu);

	savePNGbtn = new CGUIbutton2(10, 230, 100, 30);
	savePNGbtn->setText("Save PNG file");
	savePNGbtn->setGUIcallback(this);
	texGenListPanel->add(savePNGbtn);




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

	if (sender == savePNGbtn && msg.Msg == uiClick) {
		saveCurrentTexGenPNG();
		//TO DO: some kind of "Done!" dialogue box here
		return;
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
			addTexGenMenu->setLocalPos(pos.x,pos.y);
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
		texCompositor.currentTexGen->setGausSize((int)msg.fValue);
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
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setDepth((int)msg.fValue);
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
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setOctaves((int)msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->rectsStyleCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setStyle((int)msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->clustersCtrl) {
			static_cast<CRectsTex*>(texCompositor.currentTexGen)->setClusters(msg.fValue);
			texCompositor.compose();
		}
	}

	if (texCompositor.currentTexGen->texGenType == texBlocks) {
		if (sender == GUInoiseCtrl->blocksStagesCtrl) {
			static_cast<CBlocksTex*>(texCompositor.currentTexGen)->setStages((int)msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->blocksIterationsCtrl) {
			static_cast<CBlocksTex*>(texCompositor.currentTexGen)->setIterations((int)msg.fValue);
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

	if (texCompositor.currentTexGen->texGenType == texCover) {
		if (sender == GUInoiseCtrl->coverIterationsCtrl) {
			static_cast<CoverTex*>(texCompositor.currentTexGen)->setIterations((int)msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->coverScaleCtrl) {
			static_cast<CoverTex*>(texCompositor.currentTexGen)->setScale(msg.fValue);
			texCompositor.compose();
		}
	}

	if (texCompositor.currentTexGen->texGenType == texTerrain) {
		if (sender == GUInoiseCtrl->terrainGridSizeCtrl) {
			static_cast<CTerrainTex*>(texCompositor.currentTexGen)->setGridSize((int)msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->terrainDetailsCtrl) {
			static_cast<CTerrainTex*>(texCompositor.currentTexGen)->setDetails(msg.value);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->terrainFeatureCountCtrl) {
			static_cast<CTerrainTex*>(texCompositor.currentTexGen)->setFeatureCount((int)msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->terrainFeatureSizeCtrl) {
			static_cast<CTerrainTex*>(texCompositor.currentTexGen)->setFeatureSize(msg.fValue);
			texCompositor.compose();
		}
		if (sender == GUInoiseCtrl->seedCtrl) {
			static_cast<CTerrainTex*>(texCompositor.currentTexGen)->setRandomSeed((int)msg.fValue);
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

	if (texCompositor.currentTexGen->texGenType == texCover) {
		GUInoiseCtrl->coverIterationsCtrl->setValue(static_cast<CoverTex*>(texCompositor.currentTexGen)->getIterations());
		GUInoiseCtrl->coverScaleCtrl->setValue(static_cast<CoverTex*>(texCompositor.currentTexGen)->getScale());
	}

	if (texCompositor.currentTexGen->texGenType == texTerrain) {
		GUInoiseCtrl->terrainGridSizeCtrl->setValue(static_cast<CTerrainTex*>(texCompositor.currentTexGen)->getGridSize());
		GUInoiseCtrl->terrainDetailsCtrl->Set = (static_cast<CTerrainTex*>(texCompositor.currentTexGen)->getDetails());
		GUInoiseCtrl->terrainFeatureCountCtrl->setValue(static_cast<CTerrainTex*>(texCompositor.currentTexGen)->getFeatureCount());
		GUInoiseCtrl->terrainFeatureSizeCtrl->setValue(static_cast<CTerrainTex*>(texCompositor.currentTexGen)->getFeatureSize());
		GUInoiseCtrl->seedCtrl->setValue(static_cast<CTerrainTex*>(texCompositor.currentTexGen)->getRandomSeed());
	}



}

void CTexGenUI::save() {
	texCompositor.save(pApp->dataPath + fileName->text + ".gen");
	fillRestoreMenu();
}




void CTexGenUI::restore(std::string& filename) {
	clear();
	trackImage = NULL;

	texCompositor.restore(pApp->dataPath + filename + ".gen");

	texCompositor.setCurrentLayer(texCompositor.texGens.size()-1);
	configureGUI(texCompositor.currentTexGen->texGenType);
	updateGUI();
	updateDisplayImage();
	fillStackMenu();
	fileName->setText(filename);
}


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
	texGenList->addItem({ "[add texgen]" });
}

/** add the tex gen indicated to the stack. */
void CTexGenUI::addTexGen(int itemNo) {
	switch (itemNo) {
	case 0: texCompositor.createNoiseTex(); break;
	case 1: texCompositor.createColouriseTex(); break;
	case 2: texCompositor.createRidgedMFTex(); break;
	case 3: texCompositor.createCylinderTex(); break;
	case 4: texCompositor.createTurbulenceTex(); break;
	case 5: texCompositor.createScaleBiasTex(); break;
	case 6: texCompositor.createaddTex(); break;
	case 7: texCompositor.createSeamlessTex(); break;
	case 8: texCompositor.createScalePointTex(); break;
	case 9: texCompositor.createBillowTex(); break;
	case 10: texCompositor.createVoronoiTex(); break;
	case 11: texCompositor.createSelectTex(); break;
	case 12: texCompositor.createLayerTex(); break;
	case 13: texCompositor.createGausTex(); break;
	case 14: texCompositor.createRectsTex(); break;
	case 15: texCompositor.createBlocksTex(); break;
	case 16: texCompositor.createCoverTex(); break;
	case 17: texCompositor.createCutupTex(); break;
	case 18: texCompositor.createTerrainTex(); break;
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
	case texadd: GUInoiseCtrl->activateRows({ "source1&2" }); break;
	case texSeamless: GUInoiseCtrl->activateRows({ "source1","seamPercentFalloff" }); break;;
	case texScalePoint: GUInoiseCtrl->activateRows({ "source1", "scalePoint" }); break;
	case texBillow: GUInoiseCtrl->activateRows({ "octavePower", "freqPersist","sampWidthHeight" }); break;
	case texVoronoi: GUInoiseCtrl->activateRows({ "freq","sampWidthHeight", "distHue" }); break;
	case texSelect: GUInoiseCtrl->activateRows({ "lowerUpper","falloff","source1&2","source3" }); break;
	case texLayer: GUInoiseCtrl->activateRows({ "source1&2" }); break;
	case texGaus: GUInoiseCtrl->activateRows({ "source1", "gausSizeSigma"}); break;
	case texRects: GUInoiseCtrl->activateRows({ "rectsDepthVariance","octave","freq","styleClusters" }); break;
	case texBlocks: GUInoiseCtrl->activateRows({ "stagesIterations","densityScale" }); break;
	case texCover: GUInoiseCtrl->activateRows({ "iterationsScale" }); break;
	case texCutup: GUInoiseCtrl->activateRows({ "source1&2" }); break;
	case texTerrain: GUInoiseCtrl->activateRows({ "sizeDetails","featuresSize","randomSeed"}); break;

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
		for (unsigned int trackedNo = 0; trackedNo < texCompositor.texGens.size(); trackedNo++)
			if (trackImage == texCompositor.texGens[trackedNo]) {
				texGenList->setHighlight(trackedNo, false); 
				break;
			}
		trackImage = texCompositor.texGens[layer];
		texGenList->setHighlight(layer, true);
	}
}

/** Save the target image of the current texGen as a PNG file. */
void CTexGenUI::saveCurrentTexGenPNG() {
	//determine the filename
	string requestedFilename = fileName->getText() + ".png";
	//does it exist?
	//create list of filenames
	vector<string> filenames;
	for (auto& p : fs::directory_iterator(pApp->dataPath)) {
		if (p.path().extension().string().compare(".png") == 0)
			filenames.push_back(p.path().filename().string());
	}
	
	string newFilename = requestedFilename;
	unsigned int insertPos = requestedFilename.find_last_of(".");
	int uniqueNum = 1;
	//while we can find filename in this directory...
	while (find(filenames.begin(), filenames.end(), newFilename) != filenames.end()) {
		stringstream number;
		number << std::setw(4) << std::setfill('0');
		number << uniqueNum;
		string textNum = "#" + number.str();
		newFilename = requestedFilename;
		newFilename.insert(insertPos, textNum);
		uniqueNum++;
	}

	texCompositor.saveCurrentTexGenPNG(pApp->dataPath + newFilename);

}
