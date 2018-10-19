#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"

#include "texGen.h"

/** A class for creating, viewing and experimenting with procedural textures. */
class Compositor : public Icallback {
public:
	virtual void initTex() {};
	virtual void compose() {};
	void composeJade();
	void composeWood();
	void composeGranite();
	void composeSlime();
	void composeSky();

	


	CRenderTexture sample;
	CRenderTexture sample1;
	CRenderTexture sample2;
	CRenderTexture sample3;
	CRenderTexture sample4;

	CRenderTexture palette1;
	CRenderTexture palette2;

	CRenderTexture composedTex;
	CRenderTexture colouredTex;

	ColourGradient texColourGradient;
};