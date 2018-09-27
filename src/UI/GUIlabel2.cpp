#include "GUIlabel2.h"

using namespace glm;

CGUIlabel2::CGUIlabel2(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y);
	width = w; height = h;
	//drawBox.pos = i32vec2(x, y); 
	drawBox.size = i32vec2(w, h);
	textureHeight = h;
	textureWidth = w;
	
	textData.font = defaultFont;
	textBuf.setFont(defaultFont);
	textBuf.setSize(textureWidth, textureHeight);

	type = uiLabel;
	setTextColour(0, 0, 0, 1);
	multiLine = false;
	drawBorder = false;
	mousePassthru = true;
	//pDrawFuncs->registerControl(*this);
	renderOffset = i32vec2(0, 0);
	
}

void CGUIlabel2::setFont(CFont* newFont) {
	textBuf.setFont(newFont);
	textData.font = newFont;
}

void CGUIlabel2::setText(std::string newText) {
	textData.text = newText;
	renderText();
}

void CGUIlabel2::setTextColour(float r, float g, float b, float a) {
	textData.style.colour = glm::vec4(r, g, b, a);
	textBuf.setTextColour(textData.style.colour);
	renderText();
}

void CGUIlabel2::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}

void CGUIlabel2::setMultiLine(bool onOff) {
	multiLine = onOff;
	renderText();
}

void CGUIlabel2::setHorizontalAlignment(TTextAlign align) {
	textAlign = align;
}

TTextAlign CGUIlabel2::getJustification() {
	return textAlign;
}


void CGUIlabel2::DrawSelf() {
	//if (textData.text.size()) //TO DO: blank text is sometimes corrupted, not sure why
		pDrawFuncs->drawTexture(drawBox, textBuf.textTexture);
	
	 if (drawBorder) {
		//pDrawFuncs->drawCtrlBorder(*this);
		 pDrawFuncs->drawBorder2(drawBox, (glm::vec4&)borderColour);
	}
}



/** Set the dimensions and relative position of the control. */
void CGUIlabel2::SetPos(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y); 
	width = w; height = h;

	drawBox.pos = glm::i32vec2(x, y);
	drawBox.size = glm::i32vec2(w, h);
	
	updateAppearance();
	//textureWidth = drawBox.size.x; //TO DO, any offset adjustment goes here
	//textureHeight = drawBox.size.y;

	textBuf.setSize(textureWidth, textureHeight);
	renderText();
}

void CGUIlabel2::updateAppearance() {
	CGUIbase::updateAppearance();
	//assume dimensions may have changed, eg, if this label was set to span
	textureWidth = drawBox.size.x; //TO DO, any offset adjustment goes here
	textureHeight = drawBox.size.y;
	textBuf.setSize(textureWidth, textureHeight);
	renderText();
}

float CGUIlabel2::getTextWidth() {
	return lineRenderedWidth;
}

/** Determine offset due to centering, etc. */
void CGUIlabel2::calcLineOffset() {
	calcLineRenderedWidth();
	if (multiLine)
		renderOffset.y = 0;
	else
		renderOffset.y = (textureHeight - textData.font->lineHeight) / 2.0f;

	if (textAlign == tcentred) {
		renderOffset.x = (textureWidth - lineRenderedWidth) / 2.0f;
	} else if (textAlign == tright) {
		renderOffset.x = textureWidth - lineRenderedWidth;
	}


}

/** Determine how wide the text line will be when rendered. */
void CGUIlabel2::calcLineRenderedWidth() {
	lineRenderedWidth = 0;
	for (int c = 0; c < textData.text.size(); c++) {
		lineRenderedWidth += textData.font->table[textData.text[c]]->width;
	}
}

/** Render the text according to current settings. */
void CGUIlabel2::renderText() {
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
int CGUIlabel2::getNextLineStart(int lineStart) {
	int breakDist = textData.text.size(); int dist = 0;
	//while there are characters, when we reach a word break record it, until we go over the allotted width;
	int c = lineStart;
	while (dist < textureWidth) { //TO DO: put c >= text.size() check here?
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


void CGUIlabel2::resize(int w, int h) {
	width = w; height = h;
	if (w < 1) //because bad things happen if we make the texture 0 in height or width
		w = 1;
	if (h < 1)
		h = 1;
	drawBox.size = glm::i32vec2(w, h);
	updateAppearance();
	renderText();
}


