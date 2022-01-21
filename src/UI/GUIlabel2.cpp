#include "GUIlabel2.h"

#include "UI/uiRender.h"

//#include "guiBase2.h"

using namespace glm;

CGUIlabel2::CGUIlabel2(int x, int y, int w, int h) : CguiBase(x,y,w,h) {
	
	textData.font = defaultFont;
	//textBuf.setSize(getWidth(), getHeight());

	type = uiLabel;
	setTextColour(style::uiDarkGrey);
	multiLine = false;
	drawBorder = false;
	renderOffset = i32vec2(0, 0);
	leftAlignIndent = 0;
	textAlign = tleft;

	drawText.setFont(defaultFont);
}

void CGUIlabel2::setFont(CFont* newFont) {
	textData.font = newFont;
	drawText.setFont(newFont);
}

void CGUIlabel2::setFont(const std::string& fontName) {
	setFont(fnt::get(fontName));
	drawText.setFont(fontName);
}

void CGUIlabel2::setText(const std::string& newText) {
	textData.text = newText;
	drawText.setText(newText);

	int oldWidth = lineRenderedWidth;
	if (resizeToFit) {
		calcLineRenderedWidth();
		if (lineRenderedWidth > oldWidth) {
			setWidth(lineRenderedWidth);
			//textBuf.setSize(getWidth(), getHeight());
		}
	}

	renderText();
}

void CGUIlabel2::setTextColour(UIcolour  colour) {
	setTextColour(vec4(colour.r, colour.g, colour.b, colour.a));
}

void CGUIlabel2::setTextColour(vec4  colour) {
	textData.style.colour = colour;
	drawText.setColour(colour);
	renderText();
}


void CGUIlabel2::setMultiLine(bool onOff) {
	multiLine = onOff;
	renderText();
}

void CGUIlabel2::setTextAlignment(TTextAlign align) {
	textAlign = align;
}

void CGUIlabel2::setLeftAlignIndent(int indent) {
	leftAlignIndent = indent;
}

TTextAlign CGUIlabel2::getJustification() {
	return textAlign;
}


void CGUIlabel2::DrawSelf() {
	//uiDraw::drawTexture(drawBox, textBuf.textTexture);
	drawText.draw();
	
	 if (drawBorder) {
		 uiDraw::drawBorder(drawBox, borderColour);
	}
}

/** Catch any resizing, to ensure the text buffer is resized. */
void CGUIlabel2::updateAppearance() {
	CGUIbase::updateAppearance();
	//textBuf.setSize(getWidth(), getHeight());
	drawText.setPosition(drawBox.pos.x, drawBox.pos.y);
	renderText();
}

int CGUIlabel2::getTextWidth() {
	return lineRenderedWidth;
}

/** Determine offset due to centering, etc. */
void CGUIlabel2::calcLineOffset() {
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
void CGUIlabel2::calcLineRenderedWidth() {
	lineRenderedWidth = drawText.getTextWidth();
}

/** Render the text according to current settings. */
void CGUIlabel2::renderText() {
	if (!textData.text.size())
		return;
	//textBuf.clearBuffer();
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
			//textBuf.renderTextAt(renderOffset.x, lineYpos, dataRec);
			lineYpos += textData.font->lineHeight;
			lineStart = nextLineStart;
		} while (nextLineStart < textData.text.size());
	}
	else {
		dataRec = { &textData.text,textData.font,textData.style.colour };
		//textBuf.renderTextAt(renderOffset.x, renderOffset.y, dataRec);
	}

	drawText.setPosition(drawBox.pos.x + renderOffset.x, drawBox.pos.y + renderOffset.y);
}

////TO DO: probably scrap whole multiline thing. Never use it.
/** Returns the point at which whitespace lets us wrap the text onto the next line. */
int CGUIlabel2::getNextLineStart(int lineStart) {
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




std::string & CGUIlabel2::getText() {
	return textData.text;
}




