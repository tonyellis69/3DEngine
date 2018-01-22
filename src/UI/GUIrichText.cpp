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
	mousePassthru = false;
	selectedHotObj = -1;
	updateDt = 0;
	correctOverrunDelay = 0.001f;
	overrunCorrect = false;
}

void CGUIrichText::DrawSelf() {

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
		newObj.hotTextId = 0; //assume we don't want to add to any preceeding hot text.
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
	textObjs.back().text += newText;
	textObjs.back().findNewlines();
	renderText();
	overrunCorrect = true;
}

/** Advance the first line that we display. */
void CGUIrichText::scrollDown() {
		textData = textObjs[firstVisibleObject];
		TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,0 };
		do {
			lineFragment = getNextLineFragment(lineFragment);
		} while (!lineFragment.finalFrag && !lineFragment.causesNewLine);
		firstVisibleObject = lineFragment.textObj;
		firstVisibleText = lineFragment.textPos + lineFragment.textLength;
		renderText();
}


/** Starting with the top visible object, work through the list, rendering each, until we overrun the
	bottom of the display area.*/
void CGUIrichText::renderText() {
	hotTextFrags.clear();
	overrun = false;
	textBuf.clearBuffer();
	i32vec2 offset(0);
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,0 };
	do {
		lineFragment = getNextLineFragment(lineFragment);
		TRichTextRec currentObj = textObjs[lineFragment.textObj];

		std::string renderLine = currentObj.text.substr(lineFragment.textPos, lineFragment.textLength);
		textBuf.setTextColour(currentObj.textColour);

		int renderStartY = offset.y;
		offset = textBuf.renderTextAt(lineFragment.renderStartX, renderStartY, renderLine);

		if (currentObj.hotTextId && renderLine[0] != '\n') {
			THotTextFragment hotFrag = { lineFragment.renderStartX, renderStartY, offset.x, offset.y + currentObj.font->lineHeight, lineFragment.textObj };
			hotFrag.text = renderLine;
			hotTextFrags.push_back(hotFrag);
		}

		if (lineFragment.causesNewLine) {
			offset = glm::i32vec2(0, offset.y + currentObj.font->lineHeight);
		}
		if (offset.y + currentObj.font->lineHeight > height) {
			//overrun = offset.y + currentObj.font->lineHeight - height;
			overrun = true;
			return;
		}
	}  while(!lineFragment.finalFrag);

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

/** Add clickable text to the end of the existing body copy. */
void CGUIrichText::appendHotText(std::string newText, int idNo) {
	TRichTextRec newObj = textObjs.back(); //clone existing style for now
	newObj.text.clear();
	newObj.hotTextId = idNo;
	textObjs.push_back(newObj);
	currentTextObj++;

	textObjs.back().text += newText;
	textObjs.back().findNewlines();
	renderText();
	overrunCorrect = true;
}



/** Check for mouse over hot text. */
void CGUIrichText::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	int oldSelectedHotObj = selectedHotObj;
	selectedHotObj = -1;
	for (auto hotTextFrag : hotTextFrags) {
		if (mouseY > hotTextFrag.renderStartY &&  mouseY < hotTextFrag.renderEndY
			&& mouseX > hotTextFrag.renderStartX && mouseX < hotTextFrag.renderEndX) {
			selectedHotObj = hotTextFrag.textObj;
			if (oldSelectedHotObj != selectedHotObj) {
				highlight(selectedHotObj);
			}
			break;
		}
	}
	
	if (oldSelectedHotObj > 0 && oldSelectedHotObj != selectedHotObj) 
		unhighlight(oldSelectedHotObj);
}

/** Highlight the given text object.*/
void CGUIrichText::highlight(int textObj) {
	textBuf.setTextColour(vec4(0,0,1,1));
	for (auto hotTextFrag : hotTextFrags) {
		if (hotTextFrag.textObj == textObj) {
			textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY, hotTextFrag.text);
		}
	}
}

/** Detect clicks on any menu items. */
void CGUIrichText::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (selectedHotObj > -1) {
		CMessage msg;
		msg.Msg = uiMsgHotTextClick;
		msg.value = textObjs[selectedHotObj].hotTextId;
		pDrawFuncs->handleUImsg(*this, msg);
	}
}


/** Check for losing mouse while hot text selected. */
void CGUIrichText::onMouseOff(const int mouseX, const int mouseY, int key) {
	if (selectedHotObj > 0) {
		unhighlight(selectedHotObj);
		selectedHotObj = -1;
	}
}

/** Unhighlight the given text object.*/
void CGUIrichText::unhighlight(int textObj) {
	glm::vec4 defaultColour = textObjs[textObj].textColour;
	textBuf.setTextColour(defaultColour);
	for (auto hotTextFrag : hotTextFrags) {
		if (hotTextFrag.textObj == textObj) {
			textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY, hotTextFrag.text);
		}
	}
}

/** Remove any hot text objects with this tag. */
void CGUIrichText::removeHotText(int tagNo) {
	for (auto it = textObjs.begin(); it != textObjs.end(); ) {
		if ((*it).hotTextId & tagNo) {
			it = textObjs.erase(it);
		}
		else
			++it;
	}
	currentTextObj = textObjs.size() - 1;
	renderText();
}

/** Scroll up by one line. */
void CGUIrichText::scrollUp() {
	int origFirstVisibleText = firstVisibleText;
	int origFirstVisibleObject = firstVisibleObject;

	TCharacterPos prevNewline = getPrevNewline(firstVisibleObject, firstVisibleText -1);//-1 ensures we skip a newline that caused this line
	firstVisibleText = prevNewline.pos;
	firstVisibleObject = prevNewline.textObj;

	//any word wraps between here and where we started?
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,0 };
	do {
		lineFragment = getNextLineFragment(lineFragment);
		//if causes new line, get details
		if (lineFragment.causesNewLine) {
			firstVisibleText = lineFragment.textPos;
			firstVisibleObject = lineFragment.textObj;
		}
	} while (lineFragment.textObj <= origFirstVisibleObject && (lineFragment.textPos + lineFragment.textLength) < origFirstVisibleText-1);

	renderText();
}

/** Return character at position previous to that given, winding back textObj if needed. */
int CGUIrichText::getPrevCharacter(int& textObj, int& pos) {
	if (pos == 0) {
		if (textObj == 0)
			return -1;
		textObj--;
		pos = textObjs[textObj].text.size() - 1;
	}
	else
		pos--;
	return textObjs[textObj].text[pos];
}

TCharacterPos CGUIrichText::getPrevNewline(int textObj, int pos) {
	do {
		if (pos <= 0) {
			if (textObj == 0)
				return TCharacterPos{ 0 };
			textObj--;
			pos = textObjs[textObj].text.size() - 1 + pos;
		}
		else
			pos--;
	} while (textObjs[textObj].text[pos] != '\n');
	return TCharacterPos{textObj, pos+1};
}


void CGUIrichText::update(float dT) {
	updateDt += dT;
	if (updateDt > correctOverrunDelay) {
		updateDt = 0;
		if (overrun && overrunCorrect) {
			scrollDown();
			overrunCorrect = overrun;
		}

	}
}



TRichTextRec::TRichTextRec() {
	hotTextId = 0;
}

//TO DO: scrap this 
/** Find all the linebreaks in this text object and record their positions. */
void TRichTextRec::findNewlines() {
	newLines.clear();
	for (int c = 0; c < text.size(); c++) {
		if (text[c] == '\n')
			newLines.push_back(c);
	}

}
