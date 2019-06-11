#include "GUIrichText.h"

#include "glm\glm.hpp"
# define PI           3.14159265358979323846  /* pi */


#include "..\3DEngine\src\utils\log.h"

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
	shortestSpaceBreak = 0;
	noScrollMode = true; //TO DO for testing purposes!!!!
	//setBorderOn(true);
	insetX = 0;
	transcriptLog = NULL;
	suspended = false;
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
		newObj.hotId = 0; //assume we don't want to add to any preceeding hot text.
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
		newObj.hotId = 0; //assume we don't want to add to any preceeding hot text.
		//newObj.hotObjId = 0;
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
void CGUIrichText::setAppendStyleHot(bool isOn, bool unsuspended, unsigned int hotId) {
	if (textObjs[currentTextObj].hotId == hotId) 
		return;

	if (textObjs[currentTextObj].text.size() > 0) {  //don't change current obj if it already has text
													 //replace with addTextlessEndObj
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		textObjs.push_back(newObj);
		currentTextObj++;
	}
	textObjs[currentTextObj].hotId = hotId;
	std::uniform_real_distribution<> randomRange{ 0,1.0f};
	textObjs[currentTextObj].period  = randomRange(randEngine);

	if (isOn) {
		if (unsuspended)
			textObjs[currentTextObj].style = hotTextStyle;
		else {
			textObjs[currentTextObj].flags |= richSuspended;
			textObjs[currentTextObj].style = currentTextStyle;
			//TO DO: make this 'suspendedStyle'
		}
	}
	else {
		textObjs[currentTextObj].style = currentTextStyle;
		textObjs[currentTextObj].flags &= ~richSuspended;
	}
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
	if (styleName == "tempOn") {
		setTempText(true);
	}
	if (styleName == "tempOff") {
		setTempText(false);
	}

	if (styleName == "markOn") {
		setMarkedText(true);
	}
	if (styleName == "markOff") {
		setMarkedText(false);
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

void CGUIrichText::setDefaultTextStyle(std::string styleName) {
	for (auto style : *styles) {
		if (style.name == styleName)
			defaultTextStyle = style;
	}
}


/** Set the text of the current text object.*/
void CGUIrichText::setText(std::string newText) {
	firstVisibleText = 0;
	textObjs[currentTextObj].text = newText;
}

/** Append newText to the current body of text. */
void CGUIrichText::appendText(std::string newText) {
	if (transcriptLog)
		*transcriptLog << newText;
	//if (textObjs.back().tmpText == tempOff)
	//	removeTempText();
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
	overrun = 0; overrunHotTextObj = -1; underrun = 0; longestLine = 0; int indent = 0;
	textBuf.init(true);
	TLineFragDrawRec dataRec;
	i32vec2 offset(0, yPixelOffset);
	int currObjNo = firstVisibleObject;
	TLineFragment lineFragment{ firstVisibleObject,firstVisibleText,0,0,0,no,0 };
	do {
		
		lineFragment = getNextLineFragment(lineFragment);
		if (lineFragment.textObj != currObjNo) { //is this line fragment the start of a new text object?
			if (textBuf.notEmpty()) { //yes? Render the last of the old text object (if any) before we go any further
				textBuf.render();	
				textBuf.init(false);
			}
			currObjNo = lineFragment.textObj;
		}
		TRichTextRec currentObj = textObjs[lineFragment.textObj];
		CFont* currentFont = pDrawFuncs->getFont(currentObj.style.font);
		scrollHeight = currentFont->lineHeight;
		longestLine = std::max(longestLine, lineFragment.renderEndX);

		string renderLine = currentObj.text.substr(lineFragment.textPos, lineFragment.textLength);

		dataRec = { &renderLine, currentFont, currentObj.style.colour };
		offset.x = textBuf.addFragment(lineFragment.renderStartX+indent, offset.y, dataRec);
		
		if ( currentObj.hotId && !(currentObj.flags & richSuspended) && renderLine[0] != '\n') {
			makeHotFragment(lineFragment, offset, renderLine);
		}

		if (lineFragment.causesNewLine != no) {
			offset = glm::i32vec2(0, offset.y + currentFont->lineHeight);
			indent = insetX;
		}
		else
			indent = 0;


		checkOverrun(offset.y, currObjNo);
		if (offset.y > getHeight()) { //we've run past the buffer entirely, so no sense writing any more
			textBuf.render();
			return;
		}
		


	}  while(!lineFragment.finalFrag);

	textBuf.render();
}

/** Create a record of the position and composition of the given fragment of hot text. This is used to check
	for mouseovers, drawing it highlighted, etc. */
void CGUIrichText::makeHotFragment(TLineFragment& lineFragment, i32vec2& offset, std::string& renderLine) {
	int lineHeight = pDrawFuncs->getFont(textObjs[lineFragment.textObj].style.font)->lineHeight;

	THotTextFragment hotFrag = { lineFragment.renderStartX, offset.y , offset.x, offset.y + lineHeight, lineFragment.textObj };
	hotFrag.text = renderLine;
	if (offset.y + lineHeight > getHeight())
		hotFrag.overrun = true;
	else
		hotFrag.overrun = false;
	hotTextFrags.push_back(hotFrag);
}

/** Record how much the given line overruns (or underruns) the bottom of the view, if at all. */
void CGUIrichText::checkOverrun(int lineStartY, int currentObjNo) {
	TRichTextRec currentObj = textObjs[currentObjNo];
	int lineHeight = pDrawFuncs->getFont(textObjs[currentObjNo].style.font)->lineHeight;

	if (lineStartY + lineHeight > getHeight()) {
		overrun = lineStartY + lineHeight - getHeight(); //was true;
		if ((currentObj.hotId) && overrunHotTextObj == -1)
			overrunHotTextObj = currentObjNo; //TO DO: check for this in hotTextScroll instead		
	}
	underrun = getHeight() - (lineStartY + lineHeight);
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

	//catch unbreakable full stops, etc, in the next object that could wrap this fragment onto the next line
	//disqualify them, however, if they are preceeded by whitespace
	int lookAheadCharWidth = 0;
	if (textObj + 1 < textObjs.size()) {
		char lookAheadChar = textObjs[textObj + 1].text[0];
		if (ispunct(lookAheadChar) && !isspace(textObjs[textObj].text.back()) )
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
		unsigned char character = (unsigned char)richTextData->text[c];
		renderX += currentFont->table[character]->width;
		if (richTextData->text[c] == '\n') {
			c++;
			nextLineFrag.causesNewLine = newline;
			break;
		}
		if ((isspace(character) || character == '-') && renderX > shortestSpaceBreak) {
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
			else if (resizeMode == resizeNone)
			{ ///////////////////////////////Experimental! not fully tested attempt to handle overlong lines
				//Problem: prevents the pop-up menus from expanding to fit their contents
				//solution: have a no-resizing mode
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

/** Check for mouse over hot text. */
void CGUIrichText::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	if (suspended)
		return;
	i32vec2 localMouse = screenToLocalCoords(mouseX, mouseY);
	int oldSelectedHotObj = selectedHotObj;
	if (mouseMode)
		selectedHotObj = -1;
	for (auto hotTextFrag : hotTextFrags) {
		if (localMouse.y > hotTextFrag.renderStartY &&  localMouse.y < hotTextFrag.renderEndY
			&& localMouse.x > hotTextFrag.renderStartX && localMouse.x < hotTextFrag.renderEndX) {
			selectedHotObj = hotTextFrag.textObj;
			if (oldSelectedHotObj != selectedHotObj) {
				msgHighlight();
				highlight(selectedHotObj); 
			}
			break;
		}
	}
	
	if (oldSelectedHotObj > -1 && oldSelectedHotObj != selectedHotObj) {
		msgHighlight();
		unhighlight(oldSelectedHotObj);
	}
}


/** Message the client that a new hot text is being moused over, in case this is useful.  */
void CGUIrichText::msgHighlight() {
	CMessage msg;
	msg.Msg = uiMsgMouseMove;
	if (selectedHotObj != -1)
		msg.value = textObjs[selectedHotObj].hotId;
	else
		msg.value = -1;
	pDrawFuncs->handleUImsg(*this, msg);
}

/** Highlight the given text object on the text buffer - in this case by redrawing it in the 'selected' colour.*/
void CGUIrichText::highlight(int textObj) {
	CFont* font = pDrawFuncs->getFont(selectedHotTextStyle.font);
	TLineFragDrawRec dataRec = { NULL, font, selectedHotTextStyle.colour };
	for (auto hotTextFrag : hotTextFrags) {
		if (hotTextFrag.textObj == textObj) {
			dataRec.text = &hotTextFrag.text;
			textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY , dataRec);

		}
	}
}

/** Detect clicks on any menu items. */
void CGUIrichText::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (suspended)
		return;
	if (selectedHotObj > -1) {
		CMessage msg;
		msg.Msg = uiMsgHotTextClick;
		//glm::i32vec2 screen = localToScreenCoords(mouseX, mouseY);
		msg.x = mouseX; msg.y = mouseY;
		msg.value = textObjs[selectedHotObj].hotId;
		pDrawFuncs->handleUImsg(*this, msg);
	}
}

void CGUIrichText::OnLMouseUp(const int mouseX, const int mouseY, int key) {
	CMessage msg;
	msg.Msg == uiMsgLMouseUp;
	parent->message(this, msg);
}

void CGUIrichText::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgRMouseUp;
	msg.x = mouseX; msg.y = mouseY;
	pDrawFuncs->handleUImsg(*this, msg);
}



/** Check for losing mouse while hot text selected. */
void CGUIrichText::onMouseOff(const int mouseX, const int mouseY, int key) {
	if (!mouseMode)
		return;

	if (selectedHotObj >= 0) {
		unhighlight(selectedHotObj);
		selectedHotObj = -1;
		msgHighlight();
	}
}

/** Unhighlight the given text object.*/
void CGUIrichText::unhighlight(int textObj) {
	//textBuf.setTextColour(hotTextColour);

	CFont* font = pDrawFuncs->getFont(hotTextStyle.font);
	TLineFragDrawRec dataRec = { NULL, font, hotTextStyle.colour };

	for (auto hotTextFrag : hotTextFrags) {
		if (hotTextFrag.textObj == textObj) {
			dataRec.text = &hotTextFrag.text;
			textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY, dataRec);
		}
	}
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

/** Return character at positionHint previous to that given, winding back textObj if needed. */
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
	animateHotText(dT);
}


bool CGUIrichText::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	int direction = wheelDelta > 0 ? 1 : -1;

	if (selectedHotObj != -1 && mouseMode) {
		CMessage msg;
		msg.Msg = uiMouseWheel;
		msg.value = textObjs[selectedHotObj].hotId;
		msg.value2 = direction;
		pDrawFuncs->handleUImsg(*this, msg);
		return true;
	}

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

	if (noScrollMode && firstVisibleObject > 0)
		 removeScrolledOffText();
	
}

/**	Delete text objects and text that has scrolled beyond the top of visibility. */
void CGUIrichText::removeScrolledOffText() {
	for (int obj = 0; obj < firstVisibleObject; obj++) {
		textObjs.erase(textObjs.begin());
		currentTextObj--;
	}
	firstVisibleObject = 0;
	textObjs[0].text = textObjs[0].text.substr(firstVisibleText, string::npos);
	firstVisibleText = 0;
	//renderText();
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
std::vector<unsigned int> CGUIrichText::purgeHotText() {
	vector<unsigned int> purgedIds;
	for (auto &textObj : textObjs) {
		//if (textObj.hotMsgId || textObj.hotObjId) {
		if (textObj.hotId != 0) {
			purgedIds.push_back(textObj.hotId);
			//textObj.hotMsgId = 0;
			//textObj.hotObjId = 0;
			textObj.hotId = 0;
			textObj.style.colour = currentTextStyle.colour;
		}
	}
	return purgedIds;
}

/** Convert hot text with the given id to standard style. */
std::vector<unsigned int> CGUIrichText::purgeHotText(unsigned int id) {
	if (id == NULL) {
		return purgeHotText();
	}
	vector<unsigned int> purgedIds;
	for (auto &textObj : textObjs) {
		if (textObj.hotId == id) {
			purgedIds.push_back(textObj.hotId);
			//textObj.hotMsgId = 0;
			//textObj.hotObjId = 0;
			textObj.hotId = 0;
			textObj.style.colour = currentTextStyle.colour;
		}
	}
	return purgedIds;
}

/** Return a vector full of the hot text ids found in the text of this control. */
std::vector<unsigned int> CGUIrichText::getHotTextIds() {
	vector<unsigned int> lostIds;
	for (auto& textObj : textObjs) {
		if (textObj.hotId != 0) 
			lostIds.push_back(textObj.hotId);
	}
	return lostIds;
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
	enum TStyleChange { styleNone, styleBold, styleHot, styleSuspendedHot, styleStyle };

	std::string writeTxt = text;
	std::string remainingTxt = text;
	std::string styleName;
	TStyleChange styleChange;
	while (remainingTxt.size()) {
		styleChange = styleNone;
		int cut = 0; int msgId = 0; int objId = 0; unsigned int hotId = 0;
		size_t found = remainingTxt.find('\\');
		if (found != std::string::npos) {
			cut = 1;
			if (remainingTxt[found + 1] == 'b') {
				styleChange = styleBold;
				bold = !bold;
				cut = 2;
			}

			if (remainingTxt[found + 1] == 'h' || remainingTxt[found + 1] == 'S') {
				hot = !hot;
				if (remainingTxt[found + 1] == 'h')
					styleChange = styleHot;
				else
					styleChange = styleSuspendedHot;

				if (remainingTxt[found + 2] == '{') {
					size_t end = remainingTxt.find("}", found);
					std::string id = remainingTxt.substr(found + 3, end - (found + 3));
					size_t sz,sz2;
					hotId = std::stoi(id,&sz);
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
			setAppendStyleHot(hot, true, hotId);
		if (styleChange == styleSuspendedHot)
			setAppendStyleHot(hot, false, hotId);
		if (styleChange == styleNone)
			setTextStyle(currentTextStyle);
		if (styleChange == styleStyle)
			setTextStyle(styleName);

	}


}

void CGUIrichText::updateAppearance() {
	CGUIbase::updateAppearance();
	//assume dimensions may have changed, eg, if this control was set to span
	textBuf.setSize(getWidth(), getHeight());
	renderText();
}



/** Resize the control vertically if text is overrunning or underrunning. Because usually you want
	a text box to just fit its damn contents. */
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

	renderText();
}

/** Resize width proportionally as we try to resize height to fit text. Stop if it looks like we're caught
	in a loop. */
void CGUIrichText::resizeByRatio() {
	float ratio = 2;// 1.618;
	int heightModifier = pDrawFuncs->getFont(textObjs[currentTextObj].style.font)->lineHeight;
	int newHeight = getHeight(); int newWidth = getWidth();
	bool previouslyOverrun = false;

	while (overrun || underrun) {
		if (overrun) {
			previouslyOverrun = true;
			newHeight = getHeight() + heightModifier; //add space for one more line and see where that gets us
		}
		else {
			if (previouslyOverrun) { //if we're now underrunning, get out to avoid loop
				break;
			}
			newHeight = getHeight() - underrun; //we know how line to shrink by
		}
		newWidth = newHeight * ratio;

		resize(newWidth, newHeight);
		renderText(); //updates the overrun/underrun registers. Does the text fit now?
	}

	//tidy any remaining underrun
	newHeight = getHeight() - underrun;
	resize(newWidth, newHeight);
}

/** Adjust width to fit longest visible line, then adjust height as needed. */
void CGUIrichText::resizeByWidth() {
	int rightBorder = 10; //don't seem to need this anymore 02/02/2019
	int resizeW = getWidth();
	if (resizeW != longestLine) {
		resizeW = longestLine; // +rightBorder;
	}
	setWidth(resizeW);

	//do we need to adjust height too?
	int resizeGuess = overrun;
	while (overrun) {
		if (getHeight() + resizeGuess > maxHeight) {  //can't resize beyond maxHeight
			break;
		}
		else
			setHeight(getHeight() + resizeGuess);
		resizeGuess *= 2;
		renderText();
	}

	if (underrun > 0) {
		setHeight(getHeight() - underrun);
	}
}

/** Return true if the given id is an active hot text.*/
/*
bool CGUIrichText::isActiveHotText(int hotId) {
	for (auto obj : textObjs) {
		//TO DO: check this still works using hotId
		if (obj.hotId == hotId)
			return true;
	}
	return false;
}*/

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
	//needsUpdate = true;
	//TO DO: see if we can get away with flag here
	//renderText();
}

/** Append a textObj marking the start or end of temporary text. */
void CGUIrichText::setTempText(bool onOff) {
	//if (textObjs[currentTextObj].text.size() > 0) {  //don't change current obj if it already has text //replace with addTextlessEndObj
		TRichTextRec newObj = textObjs[currentTextObj];
		newObj.text.clear();
		textObjs.push_back(newObj);
		currentTextObj++;
//	}
	if (onOff) {
		textObjs[currentTextObj].flags |= richTemp;
	}
	else
		textObjs[currentTextObj].flags &= ~richTemp;
}

/** Append a textObj marking the start or end of marked text. */
void CGUIrichText::setMarkedText(bool onOff) {
	TRichTextRec newObj = textObjs[currentTextObj];
	newObj.text.clear();
	textObjs.push_back(newObj);
	currentTextObj++;
	if (onOff) {
		textObjs[currentTextObj].flags |= richMarked;
	}
	else
		textObjs[currentTextObj].flags &= ~richMarked;
}

/** Remove the most recent block of tempporary text. */
void CGUIrichText::removeTempText() {
	//return;
	for ( int obj = textObjs.size() - 1; obj >= 0; obj--) {
		if (textObjs[obj].flags & richTemp) {
			textObjs.erase(textObjs.begin() + obj);	
			if (obj == 0 || !(textObjs[obj - 1].flags & richTemp))
				break;
		}
	}
	currentTextObj = textObjs.size() - 1;
}

/** Flag that normal activity is suspended/unsuspended. */
void CGUIrichText::suspend(bool isOn) {
	suspended = isOn;
}

void CGUIrichText::onDrag(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgDragging;
	msg.x = mouseX;
	msg.y = mouseY;
	parent->message(this, msg);
}

/** Remove any temporary text and update the screen. */
void CGUIrichText::collapseTempText() {
	removeTempText();
	updateText();
}

/** Make the last temporary text permanent, if any. This includes making any suspended hot text active. */
void CGUIrichText::solidifyTempText() {
	for (int obj = textObjs.size() - 1; obj > 0; obj--) {
		if (textObjs[obj].flags & richTemp) {
			textObjs[obj].flags &= ~richTemp;
			if (textObjs[obj].flags & richSuspended) {
				textObjs[obj].flags ^= richSuspended;
				textObjs[obj].style = hotTextStyle;
			}
			else {
				textObjs[obj].style = defaultTextStyle;
			}
			if (obj == 0 || !(textObjs[obj - 1].flags & richTemp)) {
				updateText();
				break;
			}
		}
	}
}

/** Run backwards throught the text, removing any duplicate hot texts. */
void CGUIrichText::unhotDuplicates() {
	vector<int> hotIds;
	for (int obj = textObjs.size() - 1; obj >= 0; obj--) {
		int hotId = textObjs[obj].hotId;
		if (hotId == 0)
			continue;
		if (find(hotIds.begin(), hotIds.end(), hotId) != hotIds.end()) {
			textObjs[obj].hotId = 0;
			textObjs[obj].style.colour = textObjs[obj-1].style.colour;
		}
		else
			hotIds.push_back(hotId);

	}
	updateText();
}

/** Remove the last section of text flagged as marked, if any. */
void CGUIrichText::removeMarked() {
	for (int obj = textObjs.size() - 1; obj >= 0; obj--) {
		if (textObjs[obj].flags & richMarked) {
			textObjs.erase(textObjs.begin() + obj);
			currentTextObj = textObjs.size() - 1;
			if (obj == 0 || !(textObjs[obj - 1].flags & richMarked))
				break;
		}
	}
	
}


/** Cause any visible hot text to animate. */
void CGUIrichText::animateHotText(float dT) {

	CFont* font = pDrawFuncs->getFont(selectedHotTextStyle.font);
	TLineFragDrawRec dataRec = { NULL, font, selectedHotTextStyle.colour };

	int objId = -1;
	for (auto hotTextFrag : hotTextFrags) {
		if (selectedHotObj == hotTextFrag.textObj)
			continue;


		if (hotTextFrag.textObj != objId) {
			objId = hotTextFrag.textObj;
			textObjs[objId].period += dT * 0.2f;
			if (textObjs[objId].period > 1)
				textObjs[objId].period = 0;
		}

		float transition;
		transition = std::max(textObjs[objId].period, 0.7f) - 0.7f;
		transition /= 0.3f;

		transition = sin(transition * PI);

		dataRec.textColour = mix(hotTextStyle.colour, selectedHotTextStyle.colour, transition);
		dataRec.text = &hotTextFrag.text;
		textBuf.renderTextAt(hotTextFrag.renderStartX, hotTextFrag.renderStartY, dataRec);
	}


}



TRichTextRec::TRichTextRec() {
	hotId = 0;
	flags = 0;
	period = 0;
}



