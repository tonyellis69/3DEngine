#include "GUIrichText.h"

#include "glm\glm.hpp"

using namespace glm;

CGUIrichText::CGUIrichText(int x, int y, int w, int h) : CGUIlabel2(x,y,w,h) {
	firstVisibleText = 0;
	textRec defaultStyle;
	defaultStyle.font = defaultFont;
	defaultStyle.textColour = vec4(0,0,0,1);
	textObjs.push_back(defaultStyle);
	currentTextObj = 0;
	firstVisibleObject = 0;
}

void CGUIrichText::DrawSelf() {

	//pDrawFuncs->drawCtrlRect(*this);

	pDrawFuncs->drawTexture(*this, textBuf.textTexture);

	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}
}

void CGUIrichText::setFont(CFont* newFont) {
	textBuf.setFont(newFont);
	textData.font = newFont;
	textObjs[currentTextObj].font = newFont;
}

/** Set the current text drawing colour. */
void CGUIrichText::setTextColour(float r, float g, float b, float a) {
	vec4 newTextColour = glm::vec4(r, g, b, a);
	if (textObjs[currentTextObj].textColour == newTextColour)
		return;

	if (textObjs[currentTextObj].text.size() > 0) {
		textRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	textObjs[currentTextObj].textColour = vec4(r, g, b, a);
}

void CGUIrichText::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}

/** Set the text of the current text object.*/
void CGUIrichText::setText(std::string newText) {
//	textData.text = newText;
	firstVisibleText = 0;
	textObjs[currentTextObj].text = newText;
}

/** Append newText to the currently displayed text. */
void CGUIrichText::appendText(std::string newText) {
	//textData.text += newText;//scrap????????????????
	//textBuf.appendText(newText);
	textObjs[currentTextObj].text += newText;
	renderText();

	while (overrun > 0) {
		scroll(-1);
	}
}

/** Change the first line that we display. */
void CGUIrichText::scroll(int direction) {
	if (direction < 0) { //scrolling down through text
		textData = textObjs[firstVisibleObject];
		int textPos = 0;
		int nextLineStart = getNextLineStart(firstVisibleText, textPos) ;
		while (nextLineStart >= textData.text.size()) {
			firstVisibleObject++;
			textData = textObjs[firstVisibleObject];
			nextLineStart = getNextLineStart(0, textPos);
		}
		firstVisibleText = nextLineStart;
		renderText();
	}
}

/** Returns the point at which whitespace lets us wrap the text onto the next line. */
int CGUIrichText::getNextLineStart(int lineStart, int& textPos) {
	int breakDist = textData.text.size(); 
	//while there are characters, when we reach a word break record it, until we go over the allotted width;
	int c = lineStart;
	while (textPos < width) { //TO DO: put c >= text.size() check here?
		if (textData.text[c] == '\n') {
			return c +1;
		}

		if (isspace(textData.text[c]))
			breakDist = c + 1;
		textPos += textData.font->table[textData.text[c]]->width;
		c++;
		if (c >= textData.text.size())
			return c;
	}
	return breakDist;
}

/** Sarting with the top visible object, work through the list, rendering them.*/
void CGUIrichText::renderText() {
	cerr << "\nStart renderText\n";

	i32vec2 lastGlyphPos(0);
	int currentTextStart = firstVisibleText;
	textBuf.clearBuffer();
	for (int i = firstVisibleObject; i < textObjs.size(); i++) {
		textData = textObjs[i];
		if (!textData.text.size())
			return;
		textBuf.setTextColour(textData.textColour);
		
		renderOffset = lastGlyphPos;

		overrun = 0;
		int nextLineStart;
		std::string renderLine;
		do {
			int textPos = renderOffset.x;
			nextLineStart = getNextLineStart(currentTextStart, textPos);
			renderLine = textData.text.substr(currentTextStart, nextLineStart - currentTextStart);
			cerr << renderLine;
			lastGlyphPos = textBuf.renderTextAt(renderOffset.x, renderOffset.y, renderLine);

			renderOffset.y += textData.font->lineHeight;
			if (renderOffset.y > height) {
				cerr << "***overrun***";
				overrun = renderOffset.y - height;
				return;
			}		
			
			currentTextStart = nextLineStart;
			renderOffset.x = 0;
		} while (nextLineStart < textData.text.size());
		currentTextStart = 0;
		if (renderLine[0] == '\n')
			lastGlyphPos.y += textData.font->lineHeight;
	}
	
}
