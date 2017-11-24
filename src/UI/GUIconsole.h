#pragma once

#include "GUIlabel2.h"


/** A more versatile multiline text display class. */
class CGUIconsole : public CGUIlabel2 {
public:
	CGUIconsole(int x, int y, int w, int h);

	void appendText(std::string newText);


};