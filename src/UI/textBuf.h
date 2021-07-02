#pragma once

#include <string>

#include "glm\glm.hpp"

#include "font.h"
#include "..\renderer\renderer.h"

//#include "text.h"
struct TtextStyle {
	std::string name;
	std::string font;
	glm::vec4 colour;
	bool operator == (const TtextStyle& style) const {
		return this->font == style.font && this->colour == style.colour &&
			this->name == style.name;
	};
};


struct TtextTheme {
	std::string themeName;
	//std::vector<TtextStyle> styles;
	std::map<std::string, TtextStyle> styles;

};


/** Holds the minimum data the textBuffer needs to draw: text, font, text colour. */
struct TLineFragDrawRec {
	std::string* text;
	CFont* font;
	glm::vec4 textColour;
};

/** Stores the style details of a homogenous piece of text, ie, one with the same
font, colour, etc, together with the text itself. */
struct textRec {
	textRec() { firstLineIndent = 0; }
	std::string text;
	CFont* font;
	TtextStyle style;
	int firstLineIndent;
	//others to follow
};

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

	CBuf2 buf;
private:
	void writeToTexture2(CBuf2 & glyphQuads, float fadeInX);

	glm::i32vec2 size;

	std::vector<vBuf::T2DtexVert> textQuads;
	std::vector<unsigned int> textQuadsIndex;

	TLineFragDrawRec* mDrawData;
	glm::vec4 textColour;

	};

#define CLEAR_EXISTING_IMAGE true
#define KEEP_EXISTING_IMAGE false

