#include "GUIlabel.h"

using namespace glm;



CGUIlabel::CGUIlabel(std::string & text) {
	//OK! Essentials such as the default font, colour, etc to use should already be available
	//via a pointer to the stylesheet
	//so we ask for these essentials and set them up before we do anything like try 
	//to calculate how big 'text' will be.
	textData.font = styleSheet->defaultFont;
	setTextColour(styleSheet->defaultFontColour);

	setText(text);
	//????????????????? next
}

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
}

void CGUIlabel::setFont(CFont* newFont) {
	textBuf.setFont(newFont);
	textData.font = newFont;
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
	textBuf.setTextColour(textData.style.colour);
	renderText();
}


void CGUIlabel::setMultiLine(bool onOff) {
	multiLine = onOff;
	renderText();
}

void CGUIlabel::setHorizontalAlignment(TTextAlign align) {
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
		pDrawFuncs->drawTexture(drawBox, textBuf.textTexture);
	
	 if (drawBorder) {
		//pDrawFuncs->drawCtrlBorder(*this);
		 pDrawFuncs->drawBorder2(drawBox, (glm::vec4&)borderColour);
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

	if (multiLine) {
		int lineStart = 0; int lineYpos = renderOffset.y;
		int nextLineStart ; 
		std::string renderLine;
		do {
			nextLineStart = getNextLineStart(lineStart);
			renderLine = textData.text.substr(lineStart, nextLineStart - lineStart);
			textBuf.renderTextAt(renderOffset.x, lineYpos, renderLine);
			lineYpos += textData.font->lineHeight;
			lineStart = nextLineStart;
		} while (nextLineStart < textData.text.size());
	}
	else
		textBuf.renderTextAt(renderOffset.x, renderOffset.y, textData.text);
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




