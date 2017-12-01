#include "GUIlabel2.h"

using namespace glm;

CGUIlabel2::CGUIlabel2(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	textBuf.setFont(defaultFont);
	textBuf.setSize(w, h);

	type = uiLabel;
	setTextColour(0, 0, 0, 1);
	multiLine = false;
	drawBorder = false;
	mousePassthru = true;
	pDrawFuncs->registerControl(*this);
	lineOffset = i32vec2(0, 0);
}

void CGUIlabel2::setFont(CFont* newFont) {
	textBuf.setFont(newFont);
	font = newFont;
}

void CGUIlabel2::setText(std::string newText) {
	//textBuf.setText(newText);
	text = newText;
	renderText();
}

void CGUIlabel2::setTextColour(float r, float g, float b, float a) {
	textColour.r = r; textColour.g = g; textColour.b = b; textColour.a = a;
	textBuf.setTextColour(textColour);
	//textBuf.renderText();
}

void CGUIlabel2::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}

void CGUIlabel2::setMultiLine(bool onOff) {
	textBuf.setMultiLine(onOff);
	multiLine = onOff;
	renderText();
}

void CGUIlabel2::setHorizontalAlignment(TTextAlign align) {
	textAlign = align;
	textBuf.setHorizontalAlignment(align);
	
}

TTextAlign CGUIlabel2::getJustification() {
	return textBuf.getJustification();
}


void CGUIlabel2::DrawSelf() {
	pDrawFuncs->drawTexture(*this, textBuf.textTexture);


	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}


}

void CGUIlabel2::borderOn(bool onOff) {
	drawBorder = onOff;
}

/** Set the dimensions and relative position of the control. */
void CGUIlabel2::SetPos(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = glm::i32vec2(x, y);
	drawBox.size = glm::i32vec2(w, h);
	textBuf.setSize(w, h);
	//textBuf.renderText();
	updateAppearance();
}

float CGUIlabel2::getTextWidth() {
	return textBuf.getTextWidth();
}

/** Determine offset due to centering, etc. */
void CGUIlabel2::calcLineOffset() {
	if (multiLine) 
		lineOffset.y = (-height / 2.0) + font->lineHeight;
	else
		lineOffset.y = font->lineHeight/2.0;

	if (textAlign == tcentred) {
		lineOffset.x = (width - lineRenderedWidth) / 2.0f;
	} else if (textAlign == tright) {
		lineOffset.x = width - lineRenderedWidth;
	}


}

/** Determine how wide the text line will be when rendered. */
void CGUIlabel2::calcLineRenderedWidth() {
	lineRenderedWidth = 0;
	for (int c = 0; c < text.size(); c++) {
		lineRenderedWidth += font->table[text[c]]->width;
	}
}

/** Render the text according to current settings. */
void CGUIlabel2::renderText() {
	if (!text.size())
		return;
	textBuf.clearBuffer();
	calcLineRenderedWidth();
	calcLineOffset();

	if (multiLine) {
		int lineStart = 0; int lineYpos = lineOffset.y;
		int nextLineStart ; 
		std::string renderLine;
		do {
			nextLineStart = getNextLineStart(lineStart);
			renderLine = text.substr(lineStart, nextLineStart - lineStart);
			textBuf.renderTextAt(lineOffset.x, lineYpos, renderLine);
			lineYpos += font->lineHeight;
			lineStart = nextLineStart;
		} while (nextLineStart < text.size());
	}
	else
		textBuf.renderTextAt(lineOffset.x, lineOffset.y,text);
}

/** Returns the point at which whitespace lets us wrap the text onto the next line. */
int CGUIlabel2::getNextLineStart(int lineStart) {
	int breakDist = text.size(); int dist = 0;
	//while there are characters, when we reach a word break record it, until we go over the allotted width;
	int c = lineStart;
	while (dist < width) { //TO DO: put c >= text.size() check here?
		if (text[c] == '\n') {
			return c + 1;
		}

		if (isspace(text[c]))
			breakDist = c + 1;
		dist += font->table[text[c]]->width;
		c++;
		if (c >= text.size())
			return c;
	}
	return breakDist;
}
