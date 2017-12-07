#pragma once

#include "GUIlabel2.h"


/** A more versatile multiline text display class. */
class CGUIrichText : public CGUIlabel2 {
public:
	CGUIrichText(int x, int y, int w, int h);
	void DrawSelf();
	void setText(std::string newText);
	void appendText(std::string newText);
	void scroll(int direction);

	void renderText();
	int overrun;

	//std::string textObj;
	int topLineStart; ///<Position in text of the first character to display.
};