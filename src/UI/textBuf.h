#pragma once

#include <string>

#include "glm\glm.hpp"

#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

/** A class that maintains a rectangular buffer of rendered text for displaying. */
class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void setFont(CFont* newFont);
	void setTextColour(glm::vec4& newColour);
	void renderTextAt(int x, int y, std::string textLine);
	void clearBuffer();

	CRenderTexture textTexture; ///<The texture holding the rendered text.
	
private:
	void writeToTexture2(CBuf & glyphQuads);

	glm::i32vec2 size;
	CRenderer* pRenderer;
	CFont* font;
	glm::vec4 textColour;



};