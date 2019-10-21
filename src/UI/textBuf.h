#pragma once

#include <string>

#include "glm\glm.hpp"

#include "font.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

struct TtextStyle {
	std::string name;
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

/** Holds the minimum data the textBuffer needs to draw: text, font, text colour. */
struct TLineFragDrawRec {
	std::string* text;
	CFont* font;
	glm::vec4 textColour;
};

/** A class that maintains a rectangular buffer of rendered text for displaying. */
class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void renderTextAt(int x, int y, TLineFragDrawRec& drawData);
	void renderFadeInTextAt(int x, int y, TLineFragDrawRec& drawData, float fadeInX);
	void clearBuffer();
	void init(bool clearBuffer);
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