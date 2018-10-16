#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"
#include "..\UI\GUInumeric2.h"

#include "texGen.h"

/** A class for creating, viewing and experimenting with procedural textures. */
class Compositor : public Icallback {
public:
	Compositor();
	void init(CBaseApp* app);
	void compose();
	void composeJade();
	void composeWood();
	void composeGranite();
	void composeSlime();
	void hide(bool onOff);

//private:
	void initGUI();
	void GUIcallback(CGUIbase* sender, CMessage& msg);


	CBaseApp* pApp;
	CRenderTexture sample;
	CRenderTexture sample2;
	CRenderTexture sample3;
	CRenderTexture sample4;
	CRenderTexture sample5;
	CRenderTexture sample6;
	CRenderTexture sample7;
	CRenderTexture noise;
	CRenderTexture noise1;
	CRenderTexture palette1;
	CRenderTexture palette2;


	CGUIimage* image;

	CGUIpaletteBar* paletteBar;

	CGUInumeric2* num;
	static CGUIcolourPicker* tmp;
};