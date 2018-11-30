#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"
#include "..\UI\GUImenu.h"
#include "..\UI\GUIcontainer.h"

#include "texGen.h"
#include "composeWood.h"
#include "composeGranite.h"
#include "composeSlime.h"
#include "composeSky.h"
#include "GUInoiseCtrl.h"
#include "composeRidged.h"
#include "composeTest.h"

class CTexGenUI : public Icallback {
public:
	void init(CBaseApp* app);
	void initGUI();
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void hide(bool onOff);
	void compose();
	void display();
	void updateGUI();
	void save();
	void restore(std::string& filename);
	void clear();
	void fillStackMenu();
	void addTexGen(int itemNo);
	void configureGUI(TexGenType texType);
	void fillRestoreMenu();
	void savePalette(std::string filename);
	void fillPaletteRestoreMenu();
	void restorePalette(std::string& filename);


	CBaseApp* pApp;
	CGUIimage* image;
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
};




