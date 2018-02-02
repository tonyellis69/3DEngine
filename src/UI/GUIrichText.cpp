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
	correctOverrunDelay = 0.01f;
	overrunCorrect = false;
	yPixelOffset = 0;
	smoothScrollStep = 4;
	setMouseWheelMode(scroll);
	setHotTextColour(1, 1, 1, 1);
	setHotTextHighlightColour(0, 0, 1, 1);
}

void CGUIrichText::DrawSelf() {
	//cerr << "\ndraw texture";
	//if (!overrunCorrect)
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

/** Normal colour for unselected hot text. */
void CGUIrichText::setHotTextColour(float r, float g, float b, float a) {
	hotTextColour = glm::vec4(r, g, b, a);
}

void CGUIrichText::setHotTextHighlightColour(float r, float g, float b, float a) {
	hotTextHighlightColour = glm::vec4(r, g, b, a);
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
	updateText();
	overrunCorrect = true;
}

/** Make the line after the current top line the new top line. */
bool CGUIrichText::scrollDown() {
		textData = textObjs[firstVisibleObject];
		TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,no,0 };
		do {
			lineFragment = getNextLineFragment(lineFragment);
		} while (!lineFragment.finalFrag && !lineFragment.causesNewLine);

		firstVisibleObject = lineFragment.textObj;
		firstVisibleText = lineFragment.textPos + lineFragment.textLength;
		if (lineFragment.finalFrag)
			return false;

		updateText();
		return true;
}


/** Starting with the top visible object, work through the list, rendering each, until we overrun the
	bottom of the display area.*/
void CGUIrichText::renderText() {
	hotTextFrags.clear();
	overrun = false; overrunHotTextObj = -1;
	textBuf.init(true);
	i32vec2 offset(0, yPixelOffset);
	int currObjNo = firstVisibleObject;
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,no,0 };
	do {
		lineFragment = getNextLineFragment(lineFragment);
		if (currObjNo != lineFragment.textObj) {
			textBuf.render();
			textBuf.init(false);
			currObjNo = lineFragment.textObj;
		}
		TRichTextRec currentObj = textObjs[lineFragment.textObj];
		scrollHeight = currentObj.font->lineHeight;

		std::string renderLine = currentObj.text.substr(lineFragment.textPos, lineFragment.textLength);
		textBuf.setTextColour(currentObj.textColour);

		offset = textBuf.addFragment(lineFragment.renderStartX, offset.y, renderLine);
		
		if (currentObj.hotTextId && renderLine[0] != '\n') {
			THotTextFragment hotFrag = { lineFragment.renderStartX, offset.y, offset.x, offset.y + currentObj.font->lineHeight, lineFragment.textObj };
			hotFrag.text = renderLine;
			if (offset.y + currentObj.font->lineHeight > height)
				hotFrag.overrun = true;
			else
				hotFrag.overrun = false;
			hotTextFrags.push_back(hotFrag);
		}

		if (lineFragment.causesNewLine != no) {
			offset = glm::i32vec2(0, offset.y + currentObj.font->lineHeight );
		}
		if (offset.y + currentObj.font->lineHeight > height) {
			overrun = true;
			if (currentObj.hotTextId &&  overrunHotTextObj == -1)
				overrunHotTextObj = currObjNo;
			if (offset.y > height) {
				textBuf.render();
				return;
			}
		}



	}  while(!lineFragment.finalFrag);

	textBuf.render();
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
	//if (currentLineFrag.causesNewLine != no)
	//	nextLineFrag.renderStartX = 0;
	//else
//		nextLineFrag.renderStartX = currentLineFrag.renderEndX;
	switch(currentLineFrag.causesNewLine) {
		case(wordwrap): nextLineFrag.renderStartX = 0; break;
		case(newline): nextLineFrag.renderStartX = richTextData->firstLineIndent; break;
		case(no): nextLineFrag.renderStartX = currentLineFrag.renderEndX; break;
	}
	int renderX = nextLineFrag.renderStartX;

	int breakPoint = textStartPos; int breakPointX = renderX; unsigned int c;
	for (c = textStartPos; c < richTextData->text.size(); c++) {
		renderX += richTextData->font->table[richTextData->text[c]]->width;
		if (richTextData->text[c] == '\n') {
			c++;
			nextLineFrag.causesNewLine = newline;
			break;
		}
		if (renderX > width) {
			c = breakPoint ;
			renderX = breakPointX;
			nextLineFrag.causesNewLine = wordwrap;
			break;
		}
		if (isspace(richTextData->text[c])) {
			breakPoint = c+1;
			breakPointX = renderX;
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
	newObj.textColour = hotTextColour;
	newObj.firstLineIndent = 15;
	newObj.hotTextId = idNo;
	textObjs.push_back(newObj);
	currentTextObj++;

	textObjs.back().text += newText;
	textObjs.back().findNewlines();
	updateText();
	overrunCorrect = true;
}



/** Check for mouse over hot text. */
void CGUIrichText::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	int oldSelectedHotObj = selectedHotObj;
//	selectedHotObj = -1;
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
	textBuf.setTextColour(hotTextHighlightColour);
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

	return;

	if (selectedHotObj > 0) {
		unhighlight(selectedHotObj);
		selectedHotObj = -1;
	}
}

/** Unhighlight the given text object.*/
void CGUIrichText::unhighlight(int textObj) {
	//glm::vec4 defaultColour = textObjs[textObj].textColour;
	textBuf.setTextColour(hotTextColour);
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
	updateText();
}

/** Make the line previous to the current top line the new top line. */
bool CGUIrichText::scrollUp() {
	int origFirstVisibleText = firstVisibleText;
	int origFirstVisibleObject = firstVisibleObject;

	TCharacterPos prevNewline = getPrevNewline(firstVisibleObject, firstVisibleText - 1);//-1 ensures we skip a newline that caused this line
	firstVisibleText = prevNewline.pos;
	firstVisibleObject = prevNewline.textObj;

	//any word wraps between here and where we started?
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,no,0 };
		
	//while current position < original position   //current pos = linefrag
		//final position = current position			// final pos = first*
		//current position = next wordwrap
		while (lineFragment.textObj < origFirstVisibleObject ||
			(lineFragment.textObj == origFirstVisibleObject && lineFragment.textPos < (origFirstVisibleText-1))) {

			if (lineFragment.causesNewLine != no) {
				firstVisibleText = lineFragment.textPos;
				firstVisibleObject = lineFragment.textObj;
			}

			lineFragment = getNextLineFragment(lineFragment); //TO DO should always be next wordwrap
		}

	if (firstVisibleObject == origFirstVisibleObject &&  firstVisibleText == origFirstVisibleText)
		return false;

	updateText();
	return true;
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
	if (updateDt > correctOverrunDelay)
	{
		updateDt = 0;
		if (overrun && overrunCorrect) {
			smoothScroll(-smoothScrollStep);
			overrunCorrect = overrun;
		} 
		/*else
		if (overrun && (overrunHotTextObj == selectedHotObj != -1) ) {
			overrunHotTextObj = -1;
			smoothScroll(-scrollHeight);
			
		}*/
	}

}


bool CGUIrichText::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	int direction = wheelDelta > 0 ? 1 : -1;
	if (mouseWheelMode == scroll) {
		if (!attemptHotTextScroll(direction))
			smoothScroll(direction * scrollHeight);
	}
	else {
		hotTextScroll(direction);
	}
	return true;
}

/** If we're scrolling the right way to convert to a hot text scroll, do so. */
bool CGUIrichText::attemptHotTextScroll(int direction) {
	if (selectedHotObj == -1 || hotTextFrags.size() < 2)
		return false;
	if ( (direction < 0 && selectedHotObj == hotTextFrags[0].textObj) ||
		(direction > 0 && selectedHotObj == hotTextFrags.back().textObj) ) {
		mouseWheelMode = hotText;
		hotTextScroll(direction);
		return true;
	}
	return false;
}

void CGUIrichText::smoothScroll(int pixels) {
	if (pixels < 0) { //we're scrolling down to the bottom of the text
		if (!overrun) //no more text to expose
			return;
		yPixelOffset += pixels;
		int lines = yPixelOffset / -scrollHeight;
		for (int line = 0; line < lines; line++) {
			if (overrun) {
				yPixelOffset += scrollHeight;
				scrollDown();
			}
		}
	}
	else //we're scrolling up
	{
		yPixelOffset += pixels;
		if (yPixelOffset >= 0) {  //have we dropped below top of screen?	
			yPixelOffset -= scrollHeight;  //attempt to add a new line to fill the space
			if (scrollUp()) {
				return;
			}
			else
				yPixelOffset = 0;
		}
	}
	updateText();
}


void CGUIrichText::updateText() {
	renderText();
	updateHotTextSelection();
}

/** Change between scrolling mode and hot text selection mode. */
void CGUIrichText::setMouseWheelMode(TMouseWheelMode mode) {
	mouseWheelMode = mode;
	if (mode == scroll) {
		selectedHotObj = -1;
	}
}

/** Ensure hot text is selected, if it's onscreen, and that we enter hot text scroll mode. */
void CGUIrichText::updateHotTextSelection() {
	if (hotTextFrags.size() == 0) { //no hot text
		selectedHotObj = -1;
		if (mouseWheelMode == hotText) {
			mouseWheelMode = scroll;
		}
	}
	else { //visible hot text
		if (selectedHotObj != -1)
			highlight(selectedHotObj);
		else {
			selectedHotObj = hotTextFrags[0].textObj;
			highlight(selectedHotObj);
		}
		mouseWheelMode = hotText;
	}
}

/** Scroll among hot text items, or out of them. */
void CGUIrichText::hotTextScroll(int direction) {
	int nextHotOb = -1;
	if (direction < 0) { //down/right	
		for (auto hotTextFrag : hotTextFrags) {
			if (hotTextFrag.textObj > selectedHotObj) {
				nextHotOb = hotTextFrag.textObj;
				break;
			}
		}
	} 
	else { //up/left
			for (auto hotTextFrag : hotTextFrags) {
				if (hotTextFrag.textObj < selectedHotObj) {
					nextHotOb = hotTextFrag.textObj;
				}
			}
	}

	if (nextHotOb == -1) { //escape from hot text selection mode.
	
		mouseWheelMode = scroll;
		return;
	}


	unhighlight(selectedHotObj);
	selectedHotObj = nextHotOb;
	highlight(selectedHotObj);
	/////////////////////////////////////////
	//check here if the hot text we've selected is overruning
	if (overrunHotTextObj == selectedHotObj) {
		overrunHotTextObj = -1;
		smoothScroll(-scrollHeight);
	}
}

/** Select the topmost visible hot text. */
void CGUIrichText::selectTopHotText() {
	if (hotTextFrags.size() > 0) {
		selectedHotObj = hotTextFrags[0].textObj;
		highlight(selectedHotObj);
		mouseWheelMode = hotText;
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
