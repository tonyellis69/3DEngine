#pragma once

#include "..\BaseApp.h"
#include "..\UI\GUIimage.h"
#include "..\UI\GUIpaletteBar.h"

#include "texGen.h"

/** A class for creating, viewing and experimenting with procedural textures. */
class Compositor : public Icallback {
public:
	Compositor() : colourise(true) {};
	virtual void initTex() {};
	virtual void compose() {};
	virtual void updateOctaves(int octaves) {};
	virtual void updateFrequency(float freq) {};
	virtual void updateAngle(glm::vec3 angle) {};

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
	bool colourise;
};