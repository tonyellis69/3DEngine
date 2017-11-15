#pragma once

#include <string>

#include "glm\glm.hpp"

#include "..\TexFont.h"
#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

/** A class that maintains a rectangular buffer of rendered text for displaying. */

class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void setText(std::string& str);
	void setFont(CFont& newFont);
	void setTextColour(glm::vec4& newColour);
	void renderText();
	int nextLineBreak(int lineStart);
	void setMultiLine(bool onOff);
	void setHorizontalAlignment(TTextAlign align);
	TTextAlign getJustification();

	CRenderTexture textTexture; ///<The texture holding the rendered text.
	
private:
	void writeToTexture(CBuf& buf, float lineWidth);

	glm::i32vec2 size;
	CRenderer* pRenderer;
	std::string text;
	CFont* font;
	glm::vec4 textColour;

	TTextAlign TextAlign; ///<Records whether the text is centred, etc, in its area.
	bool multiLine; ///<If true, text is not height-centred.
	float glyphHeight;
};