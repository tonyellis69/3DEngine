#pragma once

#include "..\BaseApp.h"
#include "GUItrackImage.h"
#include "..\UI\GUIpaletteBar.h"
#include "..\UI\GUImenu.h"
#include "..\UI\GUIcontainer.h"
#include "..\UI\GUIswatchContainer.h"
#include "..\UI\GUIpopMenu.h"

#include "texGen.h"
#include "GUInoiseCtrl.h"
#include "composeTest.h"

class CTexGenUI : public Icallback, public IPopupMenu {
public:
	void init(CBaseApp* app);
	void initGUI();
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void hide(bool onOff);
	void compose();
	void updateDisplayImage();
	void updateGUI();
	void save();
	void restore(std::string& filename);
	void clear();
	void fillStackMenu();
	void addTexGen(int itemNo);
	void deleteTexGen(int itemNo);
	void configureGUI(TexGenType texType);
	void fillRestoreMenu();
	void savePalette(std::string filename);
	void fillPaletteRestoreMenu();
	void restorePalette(std::string& filename);
	void highlightMouseColour(int x, int y);
	void unhighlightMouseColour();
	void popupMenuCallback(int choice);
	void trackLayerImage(int layer);
	void saveCurrentTexGenPNG();


	CBaseApp* pApp;
	CGUItrackImage* image;
	//CGUIpaletteBar* paletteBar;


	ComposeTest texCompositor;

	CGUInoiseCtrl* GUInoiseCtrl;

	CGUIcontainer* container;
	CGUIpanel* texGenListPanel;
	CGUImenu* texGenList;
	CGUIbutton2* saveButton;
	CGUItextbox2* fileName;
	CGUIdropdownMenu* restoreMenu;

	CGUImenu* addTexGenMenu;

	CNullTex nullTexGen;
	bool shadeSelectionMode;


	CTexGen* trackImage; //<Layer to use as image source.

	CGUIimage* thumbnailImage;

	CGUIbutton2* savePNGbtn;
};




