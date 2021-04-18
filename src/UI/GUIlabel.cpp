#include "GUIlabel.h"

#include "UI/uiRender.h"

using namespace glm;



CGUIlabel::CGUIlabel(const std::string&  text) {
	//OK! Essentials such as the default font, colour, etc to use should already be available
	//via a pointer to the stylesheet
	//so we ask for these essentials and set them up before we do anything like try 
	//to calculate how big 'text' will be.

	applyStyleSheet();

	setText(text);
	resizeToFit();

	type = uiLabel;
	multiLine = false;
	renderOffset = i32vec2(0, 0);
	leftAlignIndent = 0;
	textAlign = tleft;



	drawBorder = false;
}

CGUIlabel::CGUIlabel(const std::string& text, unsigned int styleWord) : CGUIlabel(text) {
	setPositionStyles(styleWord);
}

/*
CGUIlabel::CGUIlabel(int x, int y, int w, int h) : CGUIbase(x,y,w,h) {
	
	
	textData.font = defaultFont;
	textBuf.setFont(defaultFont);
	textBuf.setSize(getWidth(), getHeight());

	type = uiLabel;
	setTextColour(vec4(0, 0, 0, 1));
	multiLine = false;
	drawBorder = false;
	mousePassthru = true;
	renderOffset = i32vec2(0, 0);
	leftAlignIndent = 0;
	textAlign = tleft;
}*/

void CGUIlabel::applyStyleSheet() {
	setFont(style::defaultFont);
	setTextColour(style::defaultFontColour);
	setBorderColour(style::defaultBorderColour);
}

void CGUIlabel::setFont(CFont* newFont) {
	textData.font = newFont;
	int fontSize = textData.font->lineHeight + 2 * style::labelTextSurround;
	resizeMin = i32vec2(fontSize);
}

void CGUIlabel::setText(std::string newText) {
	textData.text = newText;
	renderText();
}


void CGUIlabel::setTextColour(UIcolour  colour) {
	setTextColour(vec4(colour.r, colour.g, colour.b, colour.a));
}

void CGUIlabel::setTextColour(vec4  colour) {
	textData.style.colour = colour;
	renderText();
}


void CGUIlabel::setMultiLine(bool onOff) {
	multiLine = onOff;
	renderText();
}

void CGUIlabel::setTextAlignment(TTextAlign align) {
	textAlign = align;
}

void CGUIlabel::setLeftAlignIndent(int indent) {
	leftAlignIndent = indent;
}

TTextAlign CGUIlabel::getJustification() {
	return textAlign;
}


void CGUIlabel::DrawSelf() {
	//if (textData.text.size()) //TO DO: blank text is sometimes corrupted, not sure why
		uiDraw::drawTexture(drawBox, textBuf.textTexture);
	
	 if (drawBorder) {
		//pDrawFuncs->drawCtrlBorder(*this);
		 uiDraw::drawBorder(drawBox, (glm::vec4&)borderColour);
	}
}

/** Catch any resizing, to ensure the text buffer is resized. */
void CGUIlabel::updateAppearance() {
	CGUIbase::updateAppearance();
	textBuf.setSize(getWidth(), getHeight());
	renderText();
}

int CGUIlabel::getTextWidth() {
	return lineRenderedWidth;
}

/** Determine offset due to centering, etc. */
void CGUIlabel::calcLineOffset() {
	calcLineRenderedWidth();
	if (multiLine)
		renderOffset.y = 0;
	else
		renderOffset.y = (getHeight() - textData.font->lineHeight) / 2;

	if (textAlign == tcentred) {
		renderOffset.x = (getWidth() - lineRenderedWidth) / 2.0f;
	} else if (textAlign == tright) {
		renderOffset.x = getWidth() - lineRenderedWidth;
	}
	else if (textAlign == tleft) {
		renderOffset.x = leftAlignIndent;
	}


}

/** Determine how wide the text line will be when rendered. */
void CGUIlabel::calcLineRenderedWidth() {
	lineRenderedWidth = 0;
	for (int c = 0; c < textData.text.size(); c++) {
		lineRenderedWidth += textData.font->table[textData.text[c]]->width;
	}
}

/** Render the text according to current settings. */
void CGUIlabel::renderText() {
	if (!textData.text.size())
		return;
	textBuf.clearBuffer();
	calcLineOffset();

	TLineFragDrawRec dataRec;
	if (multiLine) {
		int lineStart = 0; int lineYpos = renderOffset.y;
		int nextLineStart ; 
		std::string renderLine;
		do {
			nextLineStart = getNextLineStart(lineStart);
			renderLine = textData.text.substr(lineStart, nextLineStart - lineStart);
			dataRec = { &renderLine,textData.font,textData.style.colour };
			textBuf.renderTextAt(renderOffset.x, lineYpos, dataRec);
			lineYpos += textData.font->lineHeight;
			lineStart = nextLineStart;
		} while (nextLineStart < textData.text.size());
	}
	else {
		dataRec = { &textData.text,textData.font,textData.style.colour };
		textBuf.renderTextAt(renderOffset.x, renderOffset.y, dataRec);
	}
}

/** Returns the point at which whitespace lets us wrap the text onto the next line. */
int CGUIlabel::getNextLineStart(int lineStart) {
	int breakDist = textData.text.size(); int dist = 0;
	//while there are characters, when we reach a word break record it, until we go over the allotted width;
	int c = lineStart;
	while (dist < getWidth()) { //TO DO: put c >= text.size() check here?
		if (textData.text[c] == '\n') {
			return c + 1;
		}

		if (isspace(textData.text[c]))
			breakDist = c + 1;
		dist += textData.font->table[textData.text[c]]->width;
		c++;
		if (c >= textData.text.size())
			return c;
	}
	return breakDist;
}




std::string & CGUIlabel::getText() {
	return textData.text;
}


/** Resize this control to fit its text, its preset minimum size or max size. */
void CGUIlabel::resizeToFit() {
	//min and max size can both be set by the user to avoid the control spoiling
	//the aesthetic of their UI, but they start with default values.
	//Find the font height, then calculate the width needed for the given text
	i32vec2 newSize;
	newSize.y = textData.font->lineHeight + 2* style::labelTextSurround;
	calcLineRenderedWidth();
	newSize.x = lineRenderedWidth + 2 * style::labelTextSurround;
	newSize = clamp(newSize, resizeMin, resizeMax);
	resize(newSize.x, newSize.y);
}




