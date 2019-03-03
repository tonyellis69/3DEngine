#pragma once

#include <string>

#include "glm\glm.hpp"

#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

struct TtextStyle {
	std::string name;
	//CFont* font;
	std::string font;
	glm::vec4 colour;
	bool operator == (const TtextStyle &style) const {
		return this->font == style.font && this->colour == style.colour &&
			this->name == style.name;
	};
};

/** Stores the style details of a homogenous piece of text, ie, one with the same
font, colour, etc, together with the text itself. */
struct textRec {
	textRec() { firstLineIndent = 0; bold = false; }
	std::string text;
	CFont* font;
	//glm::vec4 textColour;
	TtextStyle style;
	int firstLineIndent;
	bool bold;
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
	void init(bool clearBuffer);
	int addFragment(int x, int y, std::string textLine);
	void render();

	CRenderTexture textTexture; ///<The texture holding the rendered text.
	
	CRenderer* pRenderer;

	CBuf buf;
private:
	void writeToTexture2(CBuf & glyphQuads);



	glm::i32vec2 size;

	textRec textData;

	//CBuf buf2;
	vector<vBuf::T2DtexVert> textQuads;
	vector<unsigned int> textQuadsIndex;

	
};