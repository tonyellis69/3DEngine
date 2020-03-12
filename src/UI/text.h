
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
	int height;
	int renderEndX;
	TNewLine causesNewLine;
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
	TRichTextRec() {
		hotId = 0;
		flags = 0;
		period = 0;
		gap = 0;
		lineRef = 0;
	}

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

//enum TRichTextPtrStatus { rtUnset};
class TCharacterPos {
public:
	TCharacterPos() {
		textObj = 0; pos = 0;
	}
	TCharacterPos(int textObj, int textPos);
	void setPos(int textObj, int textPos);

	int textObj;
	int pos;

	bool operator==(const TCharacterPos& rhs) { 
		return textObj == rhs.textObj && pos == rhs.pos; 
	}
	bool operator<(const TCharacterPos& rhs);
	bool operator>(const TCharacterPos& rhs);
};

class TPagePos {
public:
	TPagePos() { textPos = { 0,0 }; screenPos = { 0,0 }; }
	void setScreenX(int x) {
		screenPos.x = x;
		
	}
	void setScreenY(int y) {
		screenPos.y = y;
	}
	TCharacterPos textPos;
	glm::i32vec2 screenPos;

private:
	

};