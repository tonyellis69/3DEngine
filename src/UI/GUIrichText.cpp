#include "GUIrichText.h"

#include "glm\glm.hpp"

using namespace glm;

CGUIrichText::CGUIrichText(int x, int y, int w, int h) : CGUIlabel2(x,y,w,h) {
	firstVisibleText = 0;
	TRichTextRec defaultStyle;
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
		TRichTextRec newObj = textObjs[currentTextObj];
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

/** Append newText to the current body of text. */
void CGUIrichText::appendText(std::string newText) {
	textObjs[currentTextObj].text += newText;
	textObjs[currentTextObj].findNewlines();
	renderText();

	while (overrun > 0) {
		scroll(-1);
	}
}

/** Change the first line that we display. */
void CGUIrichText::scroll(int direction) {
	if (direction < 0) { //scrolling down through text
		textData = textObjs[firstVisibleObject];
		TTextChunk textChunk;
		TTextPos positionInText = { firstVisibleObject, firstVisibleText, 0 };
		do {
			textChunk = getNextTextChunk(positionInText);
			positionInText = textChunk.nextTextPos;
		} while (positionInText.textObj < textObjs.size() && !textChunk.causesNewLine);
		firstVisibleText = textChunk.nextTextPos.textStart;
		firstVisibleObject = textChunk.nextTextPos.textObj;
		renderText();
	}
}

/** Returns the point at which whitespace or \n lets us wrap the text onto the next line. */
int CGUIrichText::getNextLineStart(int lineStart, int& textPos) {
	int breakDist = 0;
	//while there are characters, when we reach a word break record it, until we go over the allotted width;
	int c = lineStart;
	while (textPos < width) { //TO DO: put c >= text.size() check here?
		if (textData.text[c] == '\n') {
			if (c + 1 >= textData.text.size())
				return 0;
			else
				return c +1;  
		}

		if (isspace(textData.text[c]))
			breakDist = c + 1;
		textPos += textData.font->table[textData.text[c]]->width;
		c++;
		if (c >= textData.text.size())
			return 0;// c;
	}
	return breakDist;
}


/** Starting with the top visible object, work through the list, rendering each, until we overrun the
	bottom of the display area.*/
void CGUIrichText::renderText() {
	overrun = 0;
	textBuf.clearBuffer();
	i32vec2 offset(0);
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,0 };
	do {
		lineFragment = getNextLineFragment(lineFragment);
		textRec currentObj = textObjs[lineFragment.textObj];

		std::string renderLine = currentObj.text.substr(lineFragment.textPos, lineFragment.textLength);
		textBuf.setTextColour(currentObj.textColour);

		offset = textBuf.renderTextAt(lineFragment.renderStartX, offset.y, renderLine);
		if (lineFragment.causesNewLine) {
			offset = glm::i32vec2(0, offset.y + currentObj.font->lineHeight);
		}
		if (offset.y + currentObj.font->lineHeight > height) {
			overrun = offset.y + currentObj.font->lineHeight - height;
			return;
		}
	}  while(!lineFragment.finalFrag);
}

/** Returns the next block of drawable text for this position. */
TTextChunk CGUIrichText::getNextTextChunk(TTextPos& textPos) {
	TTextChunk textChunk;
	TTextPos nextTextPos;

	textData = textObjs[textPos.textObj]; //TO DO, retire/replace with richTextData
	TRichTextRec richTextData = textObjs[textPos.textObj];

	int textSize = textData.text.size();

	int glyphStartX = textPos.renderStartX;
	int nextTextStart = getNextLineStart(textPos.textStart, glyphStartX);

	//we either reached the end of the text object, a linebreak or a word-wrap.

	//did we hit a newline character along the way?



	if (nextTextStart == 0) { //we've outstripped this object but may still be on the same line
		nextTextPos.textObj = textPos.textObj + 1;
		nextTextPos.textStart = 0;
		if (textData.text.back() == '\n') //slightly messy test for \n at end of text
			textChunk.causesNewLine = true;
		else
			textChunk.causesNewLine = false;
		textChunk.textLength = textSize - textPos.textStart;
	}
	else {
		nextTextPos.textObj = textPos.textObj;
		nextTextPos.textStart = nextTextStart;
		textChunk.causesNewLine = true;
		textChunk.textLength = nextTextStart - textPos.textStart;
	}
	nextTextPos.renderStartX = glyphStartX;
	textChunk.nextTextPos = nextTextPos;
	return textChunk;
}

/** Return the text fragment following the given one. */
TLineFragment CGUIrichText::getNextLineFragment(const TLineFragment& currentLineFrag) {
	TLineFragment nextLineFrag{}; //initialise all to 0
	int textObj = currentLineFrag.textObj;
	
	unsigned int textStartPos = currentLineFrag.textPos + currentLineFrag.textLength;
	if (textStartPos >= textObjs[textObj].text.size()) {
		textObj++;
		textStartPos = 0;
	}
	nextLineFrag.textObj = textObj;
	nextLineFrag.textPos = textStartPos;

	TRichTextRec* richTextData = &textObjs[textObj];
	if (currentLineFrag.causesNewLine)
		nextLineFrag.renderStartX = 0;
	else
		nextLineFrag.renderStartX = currentLineFrag.renderEndX;
	int renderX = nextLineFrag.renderStartX;

	int breakPoint = textStartPos; int breakPointX; unsigned int c;
	for (c = textStartPos; c < richTextData->text.size(); c++) {
		renderX += richTextData->font->table[richTextData->text[c]]->width;
		if (renderX > width) {
			c = breakPoint ;
			renderX = breakPointX;
			nextLineFrag.causesNewLine = true;
			break;
		}
		if (isspace(richTextData->text[c])) {
			breakPoint = c+1;
			breakPointX = renderX;
		}
		if (richTextData->text[c] == '\n') {
			c++;
			nextLineFrag.causesNewLine = true;
			break;
		}
	}

	nextLineFrag.textLength = c - textStartPos;
	nextLineFrag.renderEndX = renderX;
	if (textObj + 1 == textObjs.size() && c == richTextData->text.size() )
		nextLineFrag.finalFrag = true;
	return nextLineFrag;
}


/** Find all the linebreaks in this text object and record their positions. */
void TRichTextRec::findNewlines() {
	newLines.clear();
	for (int c = 0; c < text.size(); c++) {
		if (text[c] == '\n')
			newLines.push_back(c);
	}
}
