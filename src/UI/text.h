
#pragma once

#include <string>
#include <vector>
#include <map>

#include "glm/glm.hpp"

#include "font.h"

/** Collection of text style structs and other stuff, used all over. */

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

enum TNewLine { no, wordwrap, newline };
/** A partial or complete line of text, with its rendered (x) dimensions. */
struct TLineFragment {
	int textObj;
	int textPos;
	int textLength;
	int renderStartX;
	int renderStartY;
	int height;
	int renderEndX;
	TNewLine causesNewLine;
	bool finalFrag;
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

const unsigned int richSuspended = 2;
const unsigned int richMarked = 4;
const unsigned int richTemp = 8;
const unsigned int richFadeIn = 16;
const unsigned int richGap = 32;
const unsigned int richBookmark = 64;

struct TRichTextRec : textRec {
	TRichTextRec();
	unsigned int hotId;
	int gap;
	int lineRef;
	unsigned int flags;
	float period;

};

/** Holds the minimum data the textBuffer needs to draw: text, font, text colour. */
struct TLineFragDrawRec {
	std::string* text;
	CFont* font;
	glm::vec4 textColour;
};

struct TCharacterPos {
	int textObj;
	int pos;
	bool operator==(const TCharacterPos& rhs) { return textObj == rhs.textObj && pos == rhs.pos; }
};