#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"

#include "texGen.h"

/** A class for creating, viewing and experimenting with procedural textures. */
class Compositor {
public:
	Compositor();
	void init(CBaseApp* app);
	void compose();

//private:
	void initGUI();

	CBaseApp* pApp;
	CRenderTexture sample;
	CGUIimage* image;

	CGUIpaletteBar* palette;

};