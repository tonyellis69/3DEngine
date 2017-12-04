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
	text = newText;
	topLineStart = 0;
	textBuf.setText(newText);
}

/** Append newText to the currently displayed text. */
void CGUIconsole::appendText(std::string newText) {
	text += newText;
	//textBuf.appendText(newText);
	renderText();

	while (overrun > 0)
	{
		scroll(-1);
	}
}

/** Change the first line that we display. */
void CGUIconsole::scroll(int direction) {
	if (direction < 0) { //scrolling down through text
		topLineStart = getNextLineStart(topLineStart) ;
		renderText();
	}
}

/** Render the text according to current settings. */
void CGUIconsole::renderText() {
	if (!text.size())
		return;
	textBuf.clearBuffer();
	calcLineRenderedWidth();
	calcLineOffset();
	overrun = 0;
	int lineStart = topLineStart; int lineYpos = lineOffset.y;
	int nextLineStart;
	std::string renderLine;
	do {
		
		nextLineStart = getNextLineStart(lineStart);
		renderLine = text.substr(lineStart, nextLineStart - lineStart);
		textBuf.renderTextAt(lineOffset.x, lineYpos, renderLine);
		if ((lineYpos + font->lineHeight) > height) {
			overrun = (lineYpos + font->lineHeight) - height;
			return;
		}


		lineYpos += font->lineHeight;
		lineStart = nextLineStart;
		

	} while (nextLineStart < text.size());
	
}
