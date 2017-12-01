#include "GUIconsole.h"

#include "glm\glm.hpp"

using namespace glm;

CGUIconsole::CGUIconsole(int x, int y, int w, int h) : CGUIlabel2(x,y,w,h) {
	UIcolour tint = { 0,0,0,0.2f };
	setBackColour1(tint);
	setBackColour2(tint);
	textBuf.setDrawBorder(10, 10);
	topLineStart = 0;
}

void CGUIconsole::DrawSelf() {

	pDrawFuncs->drawCtrlRect(*this);

	pDrawFuncs->drawTexture(*this, textBuf.textTexture);

	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}
}

void CGUIconsole::setText(std::string newText) {
	textObj = newText;
	topLineStart = 0;
	textBuf.setText(newText);
}

/** Append newText to the currently displayed text. */
void CGUIconsole::appendText(std::string newText) {
	textObj += newText;
	textBuf.appendText(newText);
	//if (textBuf.getOverrun() > 0)
	while (textBuf.getOverrun() > 0)
	{
		scroll(-1);
	}
}

/** Change the first line that we display. */
void CGUIconsole::scroll(int direction) {
	if (direction < 0) { //scrolling down through text
		//find next line break 
		topLineStart += textBuf.nextLineBreak(0) ;
		textBuf.setText(textObj.substr(topLineStart, string::npos));
	}

}