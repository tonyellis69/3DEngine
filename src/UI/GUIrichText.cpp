#include "GUIrichText.h"

#include "glm\glm.hpp"

using namespace glm;

CGUIrichText::CGUIrichText(int x, int y, int w, int h) : CGUIlabel2(x,y,w,h) {
	styles = NULL;
	defaultTextStyle = { "default","defaultFont",glm::vec4(0,0,0,1) };

	TRichTextRec defaultStyleObj;
	defaultStyleObj.style = currentTextStyle;
	textObjs.push_back(defaultStyleObj);
	currentTextObj = 0;
	setTextStyle(defaultTextStyle);

	firstVisibleText = 0;
	firstVisibleObject = 0;
	mousePassthru = false;
	selectedHotObj = -1;
	updateDt = 0;
	correctOverrunDelay = 0.001f;
	overrunCorrect = false;
	resizeMode = resizeByWidthMode;
	yPixelOffset = 0;
	smoothScrollStep = 8;
	setMouseWheelMode(scroll);
	hotTextStyle = { "default","defaultFont",glm::vec4(0,0,0.5,1) };
	selectedHotTextStyle = { "default","defaultFont",glm::vec4(0,0,1.0,1) };

	mouseMode = true;
	maxHeight = 1000;
	longestLine = 0;
}

void CGUIrichText::DrawSelf() {
	pDrawFuncs->drawTexture(drawBox, textBuf.textTexture);

	if (drawBorder) {
		pDrawFuncs->drawBorder2(drawBox, (vec4&)borderColour);
	}
}

void CGUIrichText::setFont(CFont* newFont) {
	if (textObjs[currentTextObj].text.size() > 0) { //don't change current obj if it already has text
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		newObj.hotMsgId = 0; //assume we don't want to add to any preceeding hot text.
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	currentTextStyle.font = newFont->name;
	textObjs[currentTextObj].style = currentTextStyle;
}


CFont * CGUIrichText::getFont() {
	return pDrawFuncs->getFont(currentTextStyle.font);
}

/** Set the current text drawing colour. */
void CGUIrichText::setTextColour(float r, float g, float b, float a) {
	if (textObjs[currentTextObj].style.colour == vec4(r, g, b, a))
		return;

	if (textObjs[currentTextObj].text.size() > 0) { //don't change current obj if it already has text
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		newObj.hotMsgId = 0; //assume we don't want to add to any preceeding hot text.
		newObj.hotObjId = 0;
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	textObjs[currentTextObj].style.colour = vec4(r, g, b, a);
}

/** Set the bold style for appending text, ie, on or off. */
void CGUIrichText::setAppendStyleBold(bool isOn) {
	if (textObjs[currentTextObj].bold == isOn)
		return;

	if (textObjs[currentTextObj].text.size() > 0) {  //don't change current obj if it already has text
		//replace with addTextlessEndObj
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	textObjs[currentTextObj].bold = isOn;
	if (isOn)
		textObjs[currentTextObj].style.colour = vec4(0, 0, 1, 1); ///FAKE!!! DO NOT KEEP
	else
		textObjs[currentTextObj].style.colour = vec4(1, 1, 1, 1);
}


/** Set hot text style for appending text on or off. */
void CGUIrichText::setAppendStyleHot(bool isOn, int msgId, int objId) {
	if (textObjs[currentTextObj].hotMsgId == msgId && textObjs[currentTextObj].hotObjId == objId)
		return;

	if (textObjs[currentTextObj].text.size() > 0) {  //don't change current obj if it already has text
													 //replace with addTextlessEndObj
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	textObjs[currentTextObj].hotMsgId = msgId;
	textObjs[currentTextObj].hotObjId = objId;

	if (msgId || objId)
		//textObjs[currentTextObj].style.colour = hotTextColour;
		textObjs[currentTextObj].style = hotTextStyle;
	else
		//textObjs[currentTextObj].style.colour = currentTextStyle.colour;
		textObjs[currentTextObj].style = currentTextStyle;
}


void CGUIrichText::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}



/** Set the style for any text appended after this. */
void CGUIrichText::setTextStyle(TtextStyle & newStyle) {
	currentTextStyle = newStyle;
	if (textObjs[currentTextObj].style == newStyle)
		return;

	if (textObjs[currentTextObj].text.size() > 0) {  //don't change current obj if it already has text //replace with addTextlessEndObj
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	textObjs[currentTextObj].style = newStyle;
}

void CGUIrichText::setTextStyle(std::string styleName) {
	if (styles == NULL)
		return;
	for (auto style : *styles) {
		if (style.name == styleName) {
			setTextStyle(style);
			return;
		}
	}
}

/** Provide a pointer to a set of text styles to use */
void CGUIrichText::setTextStyles(std::vector<TtextStyle>* styleList) {
	styles = styleList;

	///////////////////
	//cache hot/selected styles if supplied
	for (auto style : *styles) {
		if (style.name == "hot")
			hotTextStyle = style;
		else if (style.name == "hotSelected")
			selectedHotTextStyle = style;
	}
}

/** Set the text of the current text object.*/
void CGUIrichText::setText(std::string newText) {
	firstVisibleText = 0;
	textObjs[currentTextObj].text = newText;
}

/** Append newText to the current body of text. */
void CGUIrichText::appendText(std::string newText) {

	textObjs.back().text += newText;
	updateText();
	overrunCorrect = true; //TO DO: why do this here?		
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
	overrun = 0; overrunHotTextObj = -1; underrun = 0; longestLine = 0;
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
		CFont* currentFont = pDrawFuncs->getFont(currentObj.style.font);
		scrollHeight = currentFont->lineHeight;
		if (longestLine < lineFragment.renderEndX)
			longestLine = lineFragment.renderEndX;

		std::string renderLine = currentObj.text.substr(lineFragment.textPos, lineFragment.textLength);
		textBuf.setTextColour(currentObj.style.colour);
		textBuf.setFont(currentFont);

		offset = textBuf.addFragment(lineFragment.renderStartX, offset.y, renderLine);
		
		if ( (currentObj.hotMsgId || currentObj.hotObjId) && renderLine[0] != '\n') {
			THotTextFragment hotFrag = { lineFragment.renderStartX, offset.y, offset.x, offset.y + currentFont->lineHeight, lineFragment.textObj };
			hotFrag.text = renderLine;
			if (offset.y + currentFont->lineHeight > getHeight())
				hotFrag.overrun =  true;
			else
				hotFrag.overrun = false;
			hotTextFrags.push_back(hotFrag);
		}

		if (lineFragment.causesNewLine != no) {
			offset = glm::i32vec2(0, offset.y + currentFont->lineHeight );
		}
		if (offset.y + currentFont->lineHeight > getHeight()) {
			overrun = offset.y + currentFont->lineHeight - getHeight(); //was true;
			if ((currentObj.hotMsgId || currentObj.hotObjId) &&  overrunHotTextObj == -1)
				overrunHotTextObj = currObjNo; //TO DO: check for this in hotTextScroll instead
			if (offset.y > getHeight()) { //overrun texture, so no sense writing any more
				textBuf.render();
				return;
			}
		}


		underrun = getHeight() - (offset.y + currentFont->lineHeight);
	}  while(!lineFragment.finalFrag);

	textBuf.render();
}


/** Return the text fragment following the given one. */
TLineFragment CGUIrichText::getNextLineFragment(const TLineFragment& currentLineFrag) {
	TLineFragment nextLineFrag{}; //initialise all to 0
	int textObj = currentLineFrag.textObj;
	
	unsigned int textStartPos = currentLineFrag.textPos + currentLineFrag.textLength;
	if (textStartPos >= textObjs[textObj].text.size()) {
		if (textObj + 1 == textObjs.size()) {
			nextLineFrag.finalFrag = true;
			return nextLineFrag;
		}
		textObj++;
		textStartPos = 0;
	}
	nextLineFrag.textObj = textObj;
	nextLineFrag.textPos = textStartPos;

	TRichTextRec* richTextData = &textObjs[textObj];
	CFont* currentFont = pDrawFuncs->getFont(richTextData->style.font);

	//catch full stops, etc, in the next object that should wrap this fragment onto the next line
	int lookAheadCharWidth = 0;
	if (textObj + 1 < textObjs.size()) {
		char lookAheadChar = textObjs[textObj + 1].text[0];
		if (ispunct(lookAheadChar))
			lookAheadCharWidth = currentFont->table[lookAheadChar]->width;
	}


	switch(currentLineFrag.causesNewLine) {
		case(wordwrap): nextLineFrag.renderStartX = 0; break;
		case(newline): nextLineFrag.renderStartX = richTextData->firstLineIndent; break;
		case(no): nextLineFrag.renderStartX = currentLineFrag.renderEndX; break;
	}
	int renderX = nextLineFrag.renderStartX;

	int breakPoint = textStartPos; int breakPointX = renderX; unsigned int c;
	for (c = textStartPos; c < richTextData->text.size(); c++) {
		renderX += currentFont->table[richTextData->text[c]]->width;
		if (richTextData->text[c] == '\n') {
			c++;
			nextLineFrag.causesNewLine = newline;
			break;
		}
		if (isspace(richTextData->text[c]) || richTextData->text[c] == '-') {
			breakPoint = c + 1;
			breakPointX = renderX;
			//break; //pretty sure this works without break as long as we go to next clause
		}
		if (renderX > getWidth() || renderX + lookAheadCharWidth > getWidth()) {
			
			if (breakPointX > 0) {
				c = breakPoint;
				renderX = breakPointX;
				nextLineFrag.causesNewLine = wordwrap;
				break;
			}

		}


	}

	nextLineFrag.textLength = c - textStartPos;
	nextLineFrag.renderEndX = renderX;
	if (textObj + 1 == textObjs.size() && c == richTextData->text.size() )
		nextLineFrag.finalFrag = true;
	return nextLineFrag;
}

/** Add clickable text to the end of the existing body copy. */
/*
void CGUIrichText::appendHotText(std::string newText, int msgId, int objId) {
	TRichTextRec newObj = textObjs.back(); //clone existing style for now
	newObj.text.clear();
	newObj.style.colour = hotTextColour;
	newObj.firstLineIndent = 15; //TO DO wtf?
	newObj.hotMsgId = msgId;
	newObj.hotObjId = objId;
	textObjs.push_back(newObj);
	currentTextObj++;

	textObjs.back().text += newText;
	updateText();
	overrunCorrect = true;
}*/



/** Check for mouse over hot text. */
void CGUIrichText::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	int oldSelectedHotObj = selectedHotObj;
	if (mouseMode)
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
	
	if (oldSelectedHotObj > -1 && oldSelectedHotObj != selectedHotObj) 
		unhighlight(oldSelectedHotObj);
}

/** Highlight the given text object.*/
void CGUIrichText::highlight(int textObj) {
	//textBuf.setTextColour(hotTextHighlightColour);
	textBuf.setTextColour(selectedHotTextStyle.colour);
	for (auto hotTextFrag : hotTextFrags) {
		if (hotTextFrag.textObj == textObj) {
			textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY , hotTextFrag.text);
		}
	}
}

/** Detect clicks on any menu items. */
void CGUIrichText::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (selectedHotObj > -1) {
		CMessage msg;
		msg.Msg = uiMsgHotTextClick;
		msg.x = mouseX; msg.y = mouseY;
		msg.value = textObjs[selectedHotObj].hotMsgId;
		msg.value2 = textObjs[selectedHotObj].hotObjId;
		pDrawFuncs->handleUImsg(*this, msg);
	}
}


/** Check for losing mouse while hot text selected. */
void CGUIrichText::onMouseOff(const int mouseX, const int mouseY, int key) {
	if (!mouseMode)
		return;

	if (selectedHotObj >= 0) {
		unhighlight(selectedHotObj);
		selectedHotObj = -1;
	}
}

/** Unhighlight the given text object.*/
void CGUIrichText::unhighlight(int textObj) {
	//textBuf.setTextColour(hotTextColour);
	textBuf.setTextColour(hotTextStyle.colour);
	for (auto hotTextFrag : hotTextFrags) {
		if (hotTextFrag.textObj == textObj) {
			textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY, hotTextFrag.text);
		}
	}
}

/** Remove any hot text objects with this tag. */
void CGUIrichText::removeHotText(int hotMsgId) {
	for (auto it = textObjs.begin(); it != textObjs.end(); ) {
		if ((*it).hotMsgId == hotMsgId) {
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

	int lastNewLineObj = firstVisibleObject;
	int lastNewLinePos = firstVisibleText;

	//any word wraps between here and where we started?
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,no,0 };
	do {	
		if (lineFragment.causesNewLine == wordwrap) {
			lastNewLinePos = lineFragment.textPos + lineFragment.textLength;
			lastNewLineObj = lineFragment.textObj;
		}

		lineFragment = getNextLineFragment(lineFragment); //TO DO should always be next wordwrap

	} while (lineFragment.textObj < origFirstVisibleObject ||
	  (lineFragment.textObj == origFirstVisibleObject && (lineFragment.textPos + lineFragment.textLength) < (origFirstVisibleText - 1)));

	if (firstVisibleObject == origFirstVisibleObject &&  firstVisibleText == origFirstVisibleText)
		return false;

	firstVisibleObject = lastNewLineObj;
	firstVisibleText = lastNewLinePos;

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
			//if (overrunMode == overScrollMode)
				smoothScroll(-smoothScrollStep);
			//else
			//	resizeToFit();
			overrunCorrect = overrun;
		} 
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
	if (!mouseMode)
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

/** Convert all existing hot text objects to standard style. */
void CGUIrichText::purgeHotText() {
	for (auto &textObj : textObjs) {
		if (textObj.hotMsgId || textObj.hotObjId) {
			textObj.hotMsgId = 0;
			textObj.hotObjId = 0;
			textObj.style.colour = currentTextStyle.colour;
		}
	}
}

/** Convert hot text with the given id to standard style. */
void CGUIrichText::purgeHotText(int msgId, int objId) {
	if (msgId == NULL && objId == NULL) {
		purgeHotText();
		return;
	}
	for (auto &textObj : textObjs) {
		if (textObj.hotMsgId == msgId && textObj.hotObjId == objId) {
			textObj.hotMsgId = 0;
			textObj.hotObjId = 0;
			textObj.style.colour = currentTextStyle.colour;
		}
	}
}

/** Remove all text from the control. */
void CGUIrichText::clear() {
	textObjs.clear();
	firstVisibleText = 0;
	TRichTextRec defaultStyle;
	defaultStyle.style = currentTextStyle;
	textObjs.push_back(defaultStyle);
	currentTextObj = 0;
	firstVisibleObject = 0;
	selectedHotObj = -1;
	updateText();
}

/** Unselect any selected hot text. */
void CGUIrichText::clearSelection() {
	if (selectedHotObj == -1)
		return;
	unhighlight(selectedHotObj);
	selectedHotObj = -1;
}



void CGUIrichText::appendMarkedUpText(string text) {
	bool bold = false; bool hot = false;
	enum TStyleChange { styleNone, styleBold, styleHot, styleStyle };

	std::string writeTxt = text;
	std::string remainingTxt = text;
	std::string styleName;
	TStyleChange styleChange;
	while (remainingTxt.size()) {
		styleChange = styleNone;
		int cut = 0; int msgId = 0; int objId = 0;
		size_t found = remainingTxt.find('\\');
		if (found != std::string::npos) {
			cut = 1;
			if (remainingTxt[found + 1] == 'b') {
				styleChange = styleBold;
				bold = !bold;
				cut = 2;
			}

			if (remainingTxt[found + 1] == 'h') {
				hot = !hot;
				styleChange = styleHot;
				if (remainingTxt[found + 2] == '{') {
					size_t end = remainingTxt.find("}", found);
					std::string id = remainingTxt.substr(found + 3, end - (found + 3));
					size_t sz;
					msgId = std::stoi(id,&sz);
					objId = std::stoi(id.substr(sz+1,std::string::npos));
					cut = 4 + id.size();
				}
				else {
					cut = 2;
				}
			}
			//other markup checks here
			if (remainingTxt.substr(found + 1, 6) == "style{") {
				styleChange = styleStyle;
				size_t end = remainingTxt.find("}", found);
				styleName = remainingTxt.substr(found + 7, end - (found + 7));
				cut = 8 + styleName.size();
			}


		}

		writeTxt = remainingTxt.substr(0, found);
		remainingTxt = remainingTxt.substr(writeTxt.size() + cut, std::string::npos);
		//TO DO: writeTxt can be empty, in which case don't append
		appendText(writeTxt);

		if (styleChange == styleBold)
			setAppendStyleBold(bold);
		if (styleChange == styleHot)
			setAppendStyleHot(hot, msgId, objId);
		if (styleChange == styleNone)
			setTextStyle(currentTextStyle);
		if (styleChange == styleStyle)
			setTextStyle(styleName);

		if (textObjs.size() > 30) {
		//	textObjs.erase(textObjs.begin());
		//	currentTextObj--;
		}

	}
}

void CGUIrichText::updateAppearance() {
	CGUIbase::updateAppearance();
	//assume dimensions may have changed, eg, if this control was set to span
	//TO DO: something's wrong, we come here a million times in textMode
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	textBuf.setSize(getWidth(), getHeight());
	renderText();

}



/** Resize the control vertically if text is overrunning or underrunning. */
void CGUIrichText::resizeToFit() {
	if (resizeMode == resizeByRatioMode) {
		resizeByRatio();
	}
	else
		resizeByWidth();

	if (parent) {
		CMessage msg = { uiMsgChildResize,0,0,0,0 };
		parent->message(this, msg);
	}
}

void CGUIrichText::resizeByRatio() {
	float ratio = 2;// 1.618;
	int heightModifier = pDrawFuncs->getFont(textObjs[currentTextObj].style.font)->lineHeight;
	int newHeight = getHeight(); int newWidth = getWidth();
	bool previouslyOverrun = false;

	while (overrun || underrun) {
		if (overrun) {
			previouslyOverrun = true;
			newHeight = getHeight() + heightModifier;
		}
		else {
			if (previouslyOverrun) {
				break;
			}
			newHeight = getHeight() - underrun;
		}
		newWidth = newHeight * ratio;

		resize(newWidth, newHeight);
	}

	//tidy any remaining underrun
	newHeight = getHeight() - underrun;
	resize(newWidth, newHeight);
}

void CGUIrichText::resizeByWidth() {
	int rightBorder = 10;
	int resizeX = getWidth();
	if (resizeX != longestLine) {
		resizeX = longestLine + rightBorder;
	}
	resize(resizeX, getHeight());

	int resizeGuess = overrun;
	while (overrun) {
		if (getHeight() + resizeGuess > maxHeight) {
			break;
		}
		else
			resize(getWidth(), getHeight() + resizeGuess);
		resizeGuess *= 2;
	}

	if (underrun > 0) {
		resize(getWidth(), getHeight() - underrun);
	}
}

/** Return true if the given id is an active hot text.*/
bool CGUIrichText::isActiveHotText(int hotId) {
	for (auto obj : textObjs) {
		if (obj.hotMsgId == hotId)
			return true;
	}
	return false;
}

void CGUIrichText::setResizeMode(TResizeMode mode) {
	resizeMode = mode;
}

CGUIrichText::~CGUIrichText() {
	int x = 0;
}

void CGUIrichText::resize(int w, int h) {
	if (w < 1) //because bad things happen if we make the texture 0 in height or width
		w = 1;
	if (h < 1)
		h = 1;
	setWidth(w);
	setHeight(h);
//	drawBox.size = glm::i32vec2(w, h);
	//updateAppearance();
	needsUpdate = true;
	//TO DO: see if we can get away with flag here
	//renderText();
}


TRichTextRec::TRichTextRec() {
	hotMsgId = 0;
	hotObjId = 0;
}



