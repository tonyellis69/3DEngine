#pragma once

#include <string>

#include "glm\glm.hpp"

//#include "..\TexFont.h"
#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

/** A class that maintains a rectangular buffer of rendered text for displaying. */

class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void setText(std::string& str);
	void setFont(CFont* newFont);
	void setTextColour(glm::vec4& newColour);
//	void renderText();
	//int nextLineBreak(int lineStart);
	//std::string getText();
	//void appendText(std::string newText);
	void setDrawBorder(int x, int y);
	void renderTextAt(int x, int y, std::string textLine);
	void clearBuffer();

	CRenderTexture textTexture; ///<The texture holding the rendered text.
	
private:
	//void writeToTexture(CBuf& buf, float lineWidth);
	void writeToTexture2(CBuf & glyphQuads);

	glm::i32vec2 size;
	CRenderer* pRenderer;
	//std::string text;
	CFont* font;
	glm::vec4 textColour;


	float glyphHeight;

	
	glm::i32vec2 drawBorder;
	glm::i32vec2 drawSpace;

};