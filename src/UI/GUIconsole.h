#pragma once

#include "GUIlabel2.h"


/** A more versatile multiline text display class. */
class CGUIconsole : public CGUIlabel2 {
public:
	CGUIconsole(int x, int y, int w, int h);
	void DrawSelf();
	void setText(std::string newText);
	void appendText(std::string newText);
	void scroll(int direction);

	std::string textObj;
	int topLineStart; ///<Position in text of the first character to display.
};