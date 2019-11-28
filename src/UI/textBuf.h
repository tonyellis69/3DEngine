#pragma once

#include <string>

#include "glm\glm.hpp"

#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

#include "text.h"





/** A class that maintains a rectangular buffer of rendered text for displaying. */
class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void renderTextAt(int x, int y, TLineFragDrawRec& drawData);
	void renderFadeInTextAt(int x, int y, TLineFragDrawRec& drawData, float fadeInX);
	void clearBuffer();
	void init(int clearBuffer);
	int addFragment(int x, int y, TLineFragDrawRec& drawData);
	void render();
	void render(float fadeInX);
	bool notEmpty();

	CRenderTexture textTexture; ///<The texture holding the rendered text.
	
	CRenderer* pRenderer;

	CBuf buf;
private:
	void writeToTexture2(CBuf & glyphQuads, float fadeInX);

	glm::i32vec2 size;

	std::vector<vBuf::T2DtexVert> textQuads;
	std::vector<unsigned int> textQuadsIndex;

	TLineFragDrawRec* mDrawData;
	glm::vec4 textColour;

	};

#define CLEAR_EXISTING_IMAGE true
#define KEEP_EXISTING_IMAGE false

