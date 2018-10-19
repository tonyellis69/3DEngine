#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"

#include "texGen.h"
#include "composeJade.h"
#include "composeWood.h"

class CTexGenUI : public Icallback {
public:
	void init(CBaseApp* app);
	void initGUI();
	void GUIcallback(CGUIbase* sender, CMessage& msg);
	void hide(bool onOff);
	void compose();
	void colourise();


	CBaseApp* pApp;
	CGUIimage* image;
	CGUIpaletteBar* paletteBar;
	ColourGradient texColourGradient;

	ComposeWood texCompositor;
};