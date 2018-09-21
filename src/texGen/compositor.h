#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"
#include "..\UI\GUInumeric2.h"

#include "texGen.h"

/** A class for creating, viewing and experimenting with procedural textures. */
class Compositor {
public:
	Compositor();
	void init(CBaseApp* app);
	void compose();

//private:
	void initGUI();

static	void colourPickerCallback(CGUIbase* sender, CMessage & msg);

	CBaseApp* pApp;
	CRenderTexture sample;
	CGUIimage* image;

	CGUIpaletteBar* palette;
	CRenderTexture paletteGradient;
	CGUInumeric2* num;
	static CGUIcolourPicker* tmp;
};