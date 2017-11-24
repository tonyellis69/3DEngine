#include "GUIconsole.h"

#include "glm\glm.hpp"

using namespace glm;

CGUIconsole::CGUIconsole(int x, int y, int w, int h) : CGUIlabel2(x,y,w,h) {
	/*xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	textBuf.setFont(defaultFont);
	textBuf.setSize(w, h);

	type = uiLabel;
	setTextColour(0, 0, 0, 1);

	drawBorder = false;
	mousePassthru = true;
	pDrawFuncs->registerControl(*this);
	*/
}

/** Append newText to the currently displayed text. */
void CGUIconsole::appendText(std::string newText) {
	textBuf.appendText(newText);
}
