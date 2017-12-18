#pragma once

#include <string>

#include "glm\glm.hpp"

#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

/** Stores the style details of a homogenous piece of text, ie, one with the same
font, colour, etc, together with the text itself. */
struct textRec {
	std::string text;
	CFont* font;
	glm::vec4 textColour;
	//others to follow
};

/** A class that maintains a rectangular buffer of rendered text for displaying. */
class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void setFont(CFont* newFont);
	void setTextColour(glm::vec4& newColour);
	glm::i32vec2 renderTextAt(int x, int y, std::string textLine);
	void clearBuffer();

	CRenderTexture textTexture; ///<The texture holding the rendered text.
	
private:
	void writeToTexture2(CBuf & glyphQuads);

	glm::i32vec2 size;
	CRenderer* pRenderer;
	textRec textData;


};