#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"
#include "..\UI\GUImenu.h"

#include "texGen.h"
#include "composeJade.h"
#include "composeWood.h"
#include "composeGranite.h"
#include "composeSlime.h"
#include "composeSky.h"
#include "GUInoiseCtrl.h"
#include "composeNoise.h"
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
	void restore();
	void clear();
	void fillMenu();
	void addTexGen(int itemNo);


	CBaseApp* pApp;
	CGUIimage* image;
	CGUIpaletteBar* paletteBar;


	ComposeTest texCompositor;

	CGUInoiseCtrl* GUInoiseCtrl;

	CGUImenu* menu;
	CGUImenu* addTexGenMenu;

	CNullTex nullTexGen;
};




