#include "GUIrichText.h"

#include "glm\glm.hpp"
# define PI           3.14159265358979323846  /* pi */


#include "..\3DEngine\src\utils\log.h"
#include "..\3DEngine\src\UI\GUIroot.h"

using namespace glm;

std::uniform_real_distribution<> CGUIrichText::randomPeriod{ 0,1.0f };

CGUIrichText::CGUIrichText(int x, int y, int w, int h) : updateDt(0), 
														transcriptLog(NULL),
														CGUIlabel2(x,y,w,h) {
	prepForFirstText();
	prepForScrolling();
	setResizeMode(resizeByWidthMode);
	initTextDelivery();
}

/** Append new text to the current body of text and show the updated page. */
void CGUIrichText::appendMarkedUpText(const std::string& text) {
	std::string remainingTxt = text;
	while (remainingTxt.size()) {
		TStyleRec styleRec; 
		std::string precedingText = findNextTag(remainingTxt, styleRec);	
		if (!precedingText.empty()) {
			appendText(precedingText);
		}
		setStyleChange(styleRec);
	}
	renderLineBuffer();
	autoscrollingDown = true;
}


/**	Compose all the currently in-view text into a single image, ready for drawing to the screen. */
void CGUIrichText::createPage() {
	initialisePage();
	writePageToLineBuffer();
	renderLineBuffer();
	//lineBuffer2.renderSprites(textBuf.textTexture);
}






void CGUIrichText::applyStyleSheet() {
	setTextColour(styleSheet->defaultFontColour); //TO DO: prob redundant now
	refreshCurrentTextStyles();
}

void CGUIrichText::DrawSelf() {
	lineBuffer2.renderSprites(dT);
	pDrawFuncs->drawTexture(drawBox, *lineBuffer2.getTextBuf());

	if (drawBorder) { //should be an overridable basic drawborder routine in the superclass
		pDrawFuncs->drawBorder2(drawBox, (vec4&)borderColour);
	}
}

TRichTextRec& CGUIrichText::getTexObjCallback(int objNo) {
	return textObjs[objNo];
}

void CGUIrichText::setFont(CFont* newFont) {
	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	currentTextStyle.font = newFont->name;
	textObjs[currentTextObj].style = currentTextStyle;
}


CFont * CGUIrichText::getFont() {
	return pDrawFuncs->getFont(currentTextStyle.font);
}

/** Set the current text drawing colour. */
void CGUIrichText::setTextColour(float r, float g, float b, float a) {
	currentTextStyle.colour = vec4(r, g, b, a);
	if (textObjs[currentTextObj].style.colour == vec4(r, g, b, a))
		return;

	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	textObjs[currentTextObj].style.colour = vec4(r, g, b, a);
}




/** Set hot text style for appending text on or off. */
void CGUIrichText::setAppendStyleHot(bool isOn, bool unsuspended, unsigned int hotId) {
	if (textObjs[currentTextObj].hotId == hotId) 
		return;

	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	textObjs[currentTextObj].hotId = hotId;
	

	if (isOn) {
		//std::uniform_real_distribution<> randomRange{ 0,1.0f };
		textObjs[currentTextObj].period = randomPeriod(randEngine);

		if (unsuspended)
			textObjs[currentTextObj].style = hotTextStyle;
		else {
			textObjs[currentTextObj].flags |= richSuspended;
			textObjs[currentTextObj].style = currentTextStyle;
			//TO DO: make this 'suspendedStyle'
		}
	}
	else {
		textObjs[currentTextObj].period = 0;
		textObjs[currentTextObj].style = currentTextStyle;
		textObjs[currentTextObj].flags &= ~richSuspended;
	}
}


void CGUIrichText::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}

void CGUIrichText::setTextColour(glm::vec4& colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}



/** Set the style for any text appended after this. */
void CGUIrichText::setTextStyle(TtextStyle & newStyle) {

	currentTextStyle = newStyle;
	if (textObjs[currentTextObj].style == newStyle)
		return;

	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	textObjs[currentTextObj].style = newStyle;
}

bool CGUIrichText::setTextStyle(std::string styleName) {
//	if (busy)
//		return false;
	if (styles == NULL)
		return false;
	for (auto style : *styles) {
		/*if (style.name == styleName) {
			setTextStyle(style);
			return true;
		}*/

		if (style.first == styleName) {
			setTextStyle(style.second);
			return true;
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


	if (styleName == "fadeOn") {
		setFadeText(true);
	}
	if (styleName == "fadeOff") {
		setFadeText(false);
	}

	if (styleName == "bookmark")
		insertBookmark();

	return true;
}


/** Update the set of text styles to use */
void CGUIrichText::refreshCurrentTextStyles() {
	styles = &styleSheet->getTheme(currentTheme).styles;


	///////////////////
	//cache hot/selected styles if supplied
	/*for (auto style : *styles) {
		if (style.name == "default")
			defaultTextStyle = style;
		else if (style.name == "hot")
			hotTextStyle = style;
		else if (style.name == "hotSelected")
			selectedHotTextStyle = style;
	}*/

	for (auto style : *styles) {
		if (style.first == "default")
			;// defaultTextStyle = style.second;
		else if (style.first == "hot")
			hotTextStyle = style.second;
		else if (style.first == "hotSelected")
			selectedHotTextStyle = style.second;
	}
}


/** Set the current theme, and load the styles of that theme, for use when requested. */
void CGUIrichText::setTextTheme(const std::string& themeName) {
	currentTheme = themeName;
	refreshCurrentTextStyles();
}

//!!!!!!!!!!!!!!!!TO DO: get rid of
void CGUIrichText::setDefaultTextStyle(std::string styleName) {
	for (auto style : *styles) {
		//if (style.name == styleName)
		//	defaultTextStyle = style;
		if (style.first == styleName)
			;// defaultTextStyle = style.second;

	}
}


/** Set the text of the current text object.*/
void CGUIrichText::setText(std::string newText) {
	firstVisibleText = 0;
	textObjs[currentTextObj].text = newText;
}



/** Scroll down by the given amount if possible, returning true if the page needs to be redrawn. */
bool CGUIrichText::scrollDownSuccessful(int dist) {
	if (!isOverrun()) //nowhere to scroll down
		return false;
	//TO DO: check if this is redundant


	int bottomLineOver = lineBuffer.getYextent() - getHeight();

	while (abs(dist) > bottomLineOver) {
		TLine newLine = compileSingleLine(lineBuffer.finalFrag());
		if (newLine.fragments.empty())
			break;
		lineBuffer.appendLine(newLine);
		bottomLineOver += newLine.height;
	}

	if (bottomLineOver < 1) //no overmatter, no scroll
		return false;

	int trim = std::min(abs(dist), bottomLineOver);
	TLineFragment firstFrag = lineBuffer.trimTop(trim);
	firstVisibleObject = firstFrag.textObj;
	firstVisibleText = firstFrag.textPos;
	updateFragmentPositions();
	return true;
}





/** Update the position details of each fragment, based on line number, line height, and scroll offset. */
void CGUIrichText::updateFragmentPositions() {
	lineBuffer.updateFragmentPositions();
}

/** Draw all current line fragments to the text display buffer. */
void CGUIrichText::renderLineBuffer() {
	overrunningPixels = 0;
	if (lineBuffer.isEmpty())
		return;

	updateFragmentPositions();//TO DO: temp!!!! only do after a change, not every time we update text buffer
	hotFrags.clear();
	fadeFrags2.clear();
	gapObj = -1; 
	textBuf.init(CLEAR_EXISTING_IMAGE);
	int currObjNo = lineBuffer.frags[lineBuffer.getLine(0).fragments[0]].textObj;
	int finalY = 0; int lineNo = 0;
	for (auto line : lineBuffer.lines) {
		for (auto fragId: line.fragments) {
			TLineFragment& frag = lineBuffer.getFragment(fragId);
			if (frag.textObj != currObjNo) { //is this line fragment the start of a new text object?
				if (textBuf.notEmpty()) { //yes? Render the last of the old text object (if any) before we go any further
					textBuf.render();
					textBuf.init(KEEP_EXISTING_IMAGE);
				}
				currObjNo = frag.textObj;
			}

			TRichTextRec currentObj = textObjs[frag.textObj];
			std::string renderLine = currentObj.text.substr(frag.textPos, frag.textLength);
			CFont* currentFont = pDrawFuncs->getFont(currentObj.style.font);
			TLineFragDrawRec dataRec = { &renderLine, currentFont, currentObj.style.colour * line.fadeInX };
			textBuf.addFragment(frag.renderStartX, frag.renderStartY, dataRec);
			finalY = frag.renderStartY + frag.height;

			//create an index of hot text fragments
			if (currentObj.hotId && !(currentObj.flags & richSuspended) && renderLine[0] != '\n') {
				
				hotFrags.push_back({ fragId,renderLine, frag.textObj });
				textObjs[currentTextObj].period = randomPeriod(randEngine);
			}

			//create an index of fade-in fragments
			if (currentObj.flags & richFadeIn && currentObj.period < 1 && renderLine[0] != '\n') {
				fadeFrags2.push_back({ fragId,renderLine, frag.textObj });
				textObjs[currentTextObj].period = 0;
			}

			//track gaps - lazily assuming there's only one for now
			if (currentObj.flags & richGap) {
				gapObj = frag.textObj;
				currentObj.lineRef = lineNo;
			}
		}
		lineNo++;

	}
	if (finalY > getHeight()) {
		overrunningPixels = finalY - getHeight();
	}


	textBuf.render();
}

/** Starting with the given line fragment, compile our text objects into line fragments 
	until we run out or overflow the page. */
void CGUIrichText::compileFragmentsToEnd(TLineFragment lineFragment) {
	SCRAP_THISoverrunHotTextObj = -1; underrun = 0;
	do {
		int currObjNo = lineFragment.textObj;
		bool startOnNewLine = false;
		if (lineFragment.causesNewLine != no) {
			startOnNewLine = true;
		}

		int fragId = processNextFragment(lineFragment);
		
		if (fragId == -1) {
			checkLineOverrun(lineBuffer.getYextent());
			return;
		}

		if (startOnNewLine) {
			lineBuffer.addLine();
		}

		lineBuffer.appendFragment(fragId);
		lineBuffer2.addTextSprite(lineFragment);

		int yExtent = lineBuffer.getYextent();
		checkLineOverrun(yExtent);

		if (yExtent > getHeight() && lineFragment.causesNewLine != no) { //we've run past the buffer entirely, so no sense writing any more
			return;
		}
	

	} while (!lineFragment.finalFrag);
}

/** Return the fragment following the given one, having registered any hot text or other special cases. */
int CGUIrichText::processNextFragment(TLineFragment& lineFragment) {
	
	bool displaced = false;
	int indent = 0;
	int fragId = -1;

	//start at the given fragment's y position (plus scrolling) and x reset for now
	i32vec2 offset(0, lineFragment.renderStartY); 

	bool startOnNewLine = false;
	if (lineFragment.causesNewLine != no) {
		startOnNewLine = true;
	}

	lineFragment = getNextLineFragment(lineFragment);
	//if there is no more text, lineFragment is now zeroed 
	//otherwise it has been set to the next fragment
	//either way we have modified the parameter 
	//but in the latter case, we create a new frags[] entry before we leave
	
	//didn't get here

	if (lineFragment.finalFrag && lineFragment.textLength == 0)
		return fragId; //final fragment has no text? Go home. (TO DO: but should this happen?)
	int currObjNo = lineFragment.textObj;
	TRichTextRec currentObj = textObjs[lineFragment.textObj];
	CFont* currentFont = pDrawFuncs->getFont(currentObj.style.font);
	scrollHeight = currentFont->lineHeight; //TO DO can probably remove - investigate!
	longestLine = std::max(longestLine, lineFragment.renderEndX);

	std::string renderLine = currentObj.text.substr(lineFragment.textPos, lineFragment.textLength);

	TLineFragDrawRec dataRec = { &renderLine, currentFont, currentObj.style.colour };

	if (currentObj.flags & richGap) { 
		lineFragment.height = currentObj.gap; 
		lineFragment.causesNewLine = newline;
	}

	
	

	offset.x = lineFragment.renderStartX + indent;
	for (int c = 0; c < renderLine.size(); c++) { //find the length of the fragment
		if (renderLine[c] == '\n')
			continue;
		offset.x += currentFont->table[renderLine[c]]->width; //get the x-extent of the fragment in offset
		//TO DO: not very intuitive, needs its own variable
	}

	if (startOnNewLine) { //TO DO scrap asap!
		offset.y += currentFont->lineHeight; //ensure starting y is *below* last line if the last frag caused a new line
	}


	//NB!!! we don't even use offset to create this fragment! It's used for making hotFrags etc and thus entirely replaceable
	lineFragment.renderStartY = offset.y;
	fragId = lineBuffer.copyFragment(lineFragment);
	
	if (lineFragment.causesNewLine != no) {
		offset = glm::i32vec2(0, offset.y + currentFont->lineHeight);
		indent = 0;// was insetX; an adjustment hack no longer used. Tidy!
	}
	else {
		indent = 0;
	}

	
	return fragId;
}

/** Create one line of line fragments from our text, starting after the given one. */
TLine CGUIrichText::compileSingleLine(TLineFragment lineFragment) {
	TLine line;
	do {
		int id = processNextFragment(lineFragment);
		if (id == -1)
			break;
		line.fragments.push_back(id);
		line.height = std::max(line.height, lineFragment.height);
		line.width = std::max(line.width, lineFragment.renderEndX);
		lineBuffer2.addTextSprite(lineFragment);
	} while (lineFragment.causesNewLine == no);

	return line;
}


/** Record how much the given line overruns (or underruns) the bottom of the view, if at all. */
void CGUIrichText::checkLineOverrun(int lineEndY) {
	overrunningPixels = 0;
	if (lineEndY > getHeight()) {
		overrunningPixels = lineEndY - getHeight(); //was true;
	}
	underrun = getHeight() - lineEndY;
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

	nextLineFrag.height = currentFont->lineHeight;

	//didn't get here

	//catch unbreakable full stops, etc, in the next object that could wrap this fragment onto the next line
	//disqualify them, however, if they are preceeded by whitespace
	int lookAheadCharWidth = 0;
	if (textObj + 1 < textObjs.size()) {
		char lookAheadChar = textObjs[textObj + 1].text[0];
		if (ispunct(lookAheadChar) && textObjs[textObj].text.size() > 0 
			&& !isspace(textObjs[textObj].text.back())   )
			lookAheadCharWidth = currentFont->table[lookAheadChar]->width;
	}


	switch(currentLineFrag.causesNewLine) {
		case(wordwrap): nextLineFrag.renderStartX = 0; break;
		case(newline): nextLineFrag.renderStartX = richTextData->firstLineIndent; break;
		case(no): nextLineFrag.renderStartX = currentLineFrag.renderEndX; break;
	}
	int renderX = nextLineFrag.renderStartX;

	//didn't get here
	

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
bool CGUIrichText::OnMouseMove(const  int mouseX, const  int mouseY, int key) {	
	checkHotTextContact(mouseX, mouseY);
	return true;
}

/** Announce that the currently selected hot text has changed. */
void CGUIrichText::msgHotTextChange(glm::i32vec2& adjustedMousePos) {
	CMessage msg = { uiMsgHotTextChange };
	if (selectedHotObj != -1) {
		msg.value = textObjs[selectedHotObj].hotId;
		msg.x = adjustedMousePos.x; msg.y = adjustedMousePos.y;
	}
	else
		msg.value = -1;
	parent->message(this, msg);
}


/** Message the client that a new hot text is being moused over, in case this is useful.  */
///TO DO: looks like I can scrap
void CGUIrichText::msgHighlight() {
	CMessage msg;
	msg.Msg = uiMsgMouseMove;
	if (selectedHotObj != -1)
		msg.value = textObjs[selectedHotObj].hotId;
	else
		msg.value = -1;
	//pDrawFuncs->handleUImsg(*this, msg); //TO DO: phase out!
}



/** Detect clicks on any menu items. */
bool CGUIrichText::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (suspended || busy)
		return true;
	if (selectedHotObj > -1) {
		CMessage msg;
		msg.Msg = uiMsgHotTextClick;
		msg.x = mouseX; msg.y = mouseY;
		msg.value = textObjs[selectedHotObj].hotId;
		//pDrawFuncs->handleUImsg(*this, msg);
		parent->message(this, msg);
	}
	return true;
}



bool CGUIrichText::OnLMouseUp(const int mouseX, const int mouseY, int key) {
	CMessage msg;
	msg.Msg == uiMsgLMouseUp;
	parent->message(this, msg);
	return true;
}

bool CGUIrichText::onRMouseUp(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgRMouseUp;
	msg.x = mouseX; msg.y = mouseY;
	parent->message(this, msg);
	return true;
}



/** Check for losing mouse while hot text selected. */
bool CGUIrichText::onMouseOff(const int mouseX, const int mouseY, int key) {
	if (selectedHotObj == -1) //no hot text selected, so mousing off doesn't matter
		return true;
	clearSelection();
	CMessage msg = { uiMsgHotTextChange };
	msg.value = -1;
	parent->message(this, msg);

	return true;
}


/** Return the first lineFragment of the previous line, if any. */
TCharacterPos CGUIrichText::getPreviousLine(TCharacterPos& startText) {
	int origStartText = startText.pos;
	int origStartObject = startText.textObj;

	//find previous newline - we can work forward from ther
	TCharacterPos prevNewline = getPrevNewline(origStartObject, origStartText - 1);//-1 ensures we skip a newline that caused this line

	TCharacterPos lastLineStart = prevNewline;// { origStartObject, origStartText };


	//any word wraps between here and where we started?
	TLineFragment lineFragment{ prevNewline.textObj,prevNewline.pos,0,0,0,0,0,no,0 };
	do {
		if (lineFragment.causesNewLine == wordwrap) {
			lastLineStart.pos = lineFragment.textPos + lineFragment.textLength;
			lastLineStart.textObj = lineFragment.textObj;
		}

		lineFragment = getNextLineFragment(lineFragment); //TO DO should always be next wordwrap

	} while (lineFragment.textObj < origStartObject ||
		(lineFragment.textObj == origStartObject && (lineFragment.textPos + lineFragment.textLength) < (origStartText - 1)));

	return lastLineStart;
}

/** Attempt to scroll up by the given distance. Return true if the page now needs to be redrawn. */
bool CGUIrichText::scrollUpSuccessful(int dist) {
	while (dist > abs(lineBuffer.yOffset)) {  //attempting to scroll beyond current top visible line
		//can we find a previous line?
		TCharacterPos currentLine = { firstVisibleObject,firstVisibleText };
		TCharacterPos prevNewline = getPreviousLine(currentLine);
		if (prevNewline == currentLine) {
			break;
		}
		//found previous line so make it our new first visible line
		firstVisibleObject = prevNewline.textObj;
		firstVisibleText = prevNewline.pos;
		//insert line in line buffer
		TLine newFirstLine = compileSingleLine(TLineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,0,no,0 });
		lineBuffer.insert(0, newFirstLine);
		lineBuffer.adjustYoffset(-lineBuffer.getLine(0).height);
	}

	if (lineBuffer.yOffset >= 0)
		return false; //we wer at the top of the text all along!

	lineBuffer.adjustYoffset(dist);
	updateFragmentPositions();
	int finalRow = lineBuffer.getFinalRow();
	if (lineBuffer.getLineTop(finalRow) > getHeight()) { //bottom line scrolled out?
		lineBuffer.removeLine(finalRow);
	}



	return true;
}



TCharacterPos CGUIrichText::getPrevNewline(int textObj, int pos) {
	do {
		if (pos <= 0) {
			if (textObj == 0)
				return TCharacterPos(0,0);
			textObj--;
			pos = textObjs[textObj].text.size() - 1 + pos;
		}
		else
			pos--;
	} while (textObjs[textObj].text[pos] != '\n');
	return TCharacterPos{textObj, pos+1};
}


void CGUIrichText::update(float dT) {
		
	//if (gapObj != -1)
	//	collapseGap(dT);
	this->dT = dT;

	updateDt += dT;
	if (updateDt > correctOverrunDelay)
	{
		updateDt = 0;
		if (autoscrollingDown) {
			 autoscrollingDown = scrollDown2(smoothScrollStep);
		}
	}
	
	if (!hotFrags.empty() && !lineFadeInOn)
		;// animateHotText(dT);
	if (!fadeFrags2.empty())
		animateFadeText(dT);
	if (lineFadeInOn) 
		animateLineFadeIn(dT);
}


bool CGUIrichText::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	int direction = wheelDelta > 0 ? 1 : -1;
	if (selectedHotObj != -1) {
		CMessage msg;
		msg.Msg = uiMouseWheel;
		msg.value = textObjs[selectedHotObj].hotId;
		msg.value2 = direction;
		pDrawFuncs->handleUImsg(*this, msg);
		return true;
	}

	if (mouseWheelMode == scroll) {
		if (!attemptHotTextScroll(direction))
			smoothScroll(direction * smoothScrollStep);
	}
	else {
		hotTextScroll(direction);
	}
	return true;
}

/** If we're scrolling the right way to convert to a hot text scroll, do so. */
bool CGUIrichText::attemptHotTextScroll(int direction) {
	if (selectedHotObj == -1 || hotFrags.size() < 2)
		return false;
	if ( (direction < 0 && selectedHotObj == hotFrags[0].textObj) ||
		(direction > 0 && selectedHotObj == hotFrags.back().textObj) ) {
		mouseWheelMode = hotText;
		hotTextScroll(direction);
		return true;
	}
	return false;
}



/** Scroll the visible text by the given number of pixels, if possible. */
void CGUIrichText::smoothScroll(int dist) {
/*	if (dist < 0) { //scrolling down to the end...
		if (scrollDownSuccessful(dist))
			renderLineBuffer();
	}
	else if (scrollUpSuccessful(dist)) {
		renderLineBuffer();		
	}*/

	//lineBuffer2 stuff////////////////
	if (dist > 0) {
		scrollUp2(dist);
		
	}
	else {
		scrollDown2(abs(dist));
	}
}


/**	Delete text objects and text that has scrolled beyond the top of visibility. */
void CGUIrichText::removeScrolledOffText() {
	for (int obj = 0; obj < firstVisibleObject; obj++) {
		textObjs.erase(textObjs.begin());
		currentTextObj--;
	}
	firstVisibleObject = 0;
	textObjs[0].text = textObjs[0].text.substr(firstVisibleText, std::string::npos);
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
//TO DO: scrap this. Janky hot text scroll we don't use anymore
void CGUIrichText::updateHotTextSelection() {
	if (hotFrags.size() == 0) { //no hot text
		selectedHotObj = -1;
		if (mouseWheelMode == hotText) {
			mouseWheelMode = scroll;
		}
	}
	else { //visible hot text
		if (selectedHotObj != -1)
			;// highlight(selectedHotObj);
		else {
			selectedHotObj = hotFrags[0].textObj;
			//highlight(selectedHotObj);
		}
		mouseWheelMode = hotText;
	}
}

/** Scroll among hot text items, or out of them. */
//TO DO: kill
void CGUIrichText::hotTextScroll(int direction) {
	int nextHotOb = -1;
	if (direction < 0) { //down/right	
		for (auto hotTextFrag : hotFrags) {
			if (hotTextFrag.textObj > selectedHotObj) {
				nextHotOb = hotTextFrag.textObj;
				break;
			}
		}
	} 
	else { //up/left
			for (auto hotTextFrag : hotFrags) {
				if (hotTextFrag.textObj < selectedHotObj) {
					nextHotOb = hotTextFrag.textObj;
				}
			}
	}

	if (nextHotOb == -1) { //escape from hot text selection mode.	
		mouseWheelMode = scroll;
		return;
	}


	//unhighlight(selectedHotObj);
	selectedHotObj = nextHotOb;
	//highlight(selectedHotObj);
	/////////////////////////////////////////
	//check here if the hot text we've selected is overruning
	if (SCRAP_THISoverrunHotTextObj == selectedHotObj) {
		SCRAP_THISoverrunHotTextObj = -1;
		smoothScroll(-scrollHeight);
	}
}

/** Select the topmost visible hot text. */
void CGUIrichText::selectTopHotText() {
	if (hotFrags.size() > 0) {
		selectedHotObj = hotFrags[0].textObj;
		//highlight(selectedHotObj);
		mouseWheelMode = hotText;
	}
}

/** Convert all existing hot text objects to standard style. */
std::vector<unsigned int> CGUIrichText::purgeHotText() {
	std::vector<unsigned int> purgedIds;
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
	std::vector<unsigned int> purgedIds;
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
	std::vector<unsigned int> lostIds;
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
	createPage();
}

/** Unselect any selected hot text. */
void CGUIrichText::clearSelection() {
	if (selectedHotObj == -1)
		return;
	selectedHotObj = -1;
}





void CGUIrichText::updateAppearance() {
	CGUIbase::updateAppearance();
	//assume dimensions may have changed, eg, if this control was set to span
	textBuf.setSize(getWidth(), getHeight());
	createPage();
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

	createPage();
}

/** Resize width proportionally as we try to resize height to fit text. Stop if it looks like we're caught
	in a loop. */
void CGUIrichText::resizeByRatio() {
	float ratio = 2;// 1.618;
	int heightModifier = pDrawFuncs->getFont(textObjs[currentTextObj].style.font)->lineHeight;
	int newHeight = getHeight(); int newWidth = getWidth();
	bool previouslyOverrun = false;


	while (overrunningPixels || underrun) {
		if (overrunningPixels) {
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
		createPage(); //updates the overrunningPixels/underrun registers. Does the text fit now?
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
	int resizeGuess = overrunningPixels;
	while (overrunningPixels) {
		if (getHeight() + resizeGuess > maxResizeHeight) {  //can't resize beyond maxResizeHeight
			break;
		}
		else
			setHeight(getHeight() + resizeGuess);
		resizeGuess *= 2;
		createPage();
	}

	if (underrun > 0) {
		setHeight(getHeight() - underrun);
	}
}

void CGUIrichText::setResizeMode(TResizeMode mode) {
	resizeMode = mode;
	maxResizeHeight = 1000;
	longestLine = 0;
}

CGUIrichText::~CGUIrichText() {
	int x = 0;
}

/**	Set everything up for the first text to be appended.*/
void CGUIrichText::prepForFirstText() {
	TRichTextRec rootObj;
	textObjs.push_back(rootObj);
	currentTextObj = 0;
	firstVisibleText = 0;
	firstVisibleObject = 0;
	selectedHotObj = -1;
	shortestSpaceBreak = 0;
	suspended = false;
	setBusy(false);
	styles = NULL;
	currentTheme = "defaultTheme";
	applyStyleSheet();
	setTextStyle("default"); //set initial text style
	lineBuffer2.setCallbackObj(this);
	lineBuffer2.setPageSize(getWidth(), getHeight());
	autoscrollingDown = false;
}

/** Ready the system to handle scrolling. */
void CGUIrichText::prepForScrolling() {
	correctOverrunDelay = 0.001f;
	overrunCorrectionOn = true;
	overrunningPixels = 0;
	smoothScrollStep = 8;
	setMouseWheelMode(scroll);
}

/** Initialise line fade-in etc. */
void CGUIrichText::initTextDelivery() {
	charInterval = 0;
	charDelay = 0.01f;
	enableLineFadeIn = false; // true; //TO DO scrap!!!
	lineFadeInOn = false;
	lineFadeSpeed = 1.0f;// 30;// 20.0f;
}

std::string CGUIrichText::findNextTag( std::string& remainingTxt, TStyleRec& styleRec) {
	int cut = 0;
	styleRec = { styleNone, 0, "" };
	size_t found = remainingTxt.find('\\');
	if (found != std::string::npos) {
		cut = 1;
		if (remainingTxt[found + 1] == 'h' || remainingTxt[found + 1] == 'S') {
		


			if (remainingTxt[found + 2] == '{') {
				size_t end = remainingTxt.find("}", found);
				std::string id = remainingTxt.substr(found + 3, end - (found + 3));
				size_t sz, sz2;
				styleRec.hotId = std::stoi(id, &sz);
				cut = 4 + id.size();

				if (remainingTxt[found + 1] == 'h')
					styleRec.styleChange = styleHotOn;
				else
					styleRec.styleChange = styleSuspendedHotOn;
			}
			else {
				cut = 2;
				if (remainingTxt[found + 1] == 'h')
					styleRec.styleChange = styleHotOff;
				else
					styleRec.styleChange = styleSuspendedHotOff;
			}
		}

		//other markup checks here
		if (remainingTxt.substr(found + 1, 6) == "style{") {
			styleRec.styleChange = styleStyle;
			size_t end = remainingTxt.find("}", found);
			styleRec.styleName = remainingTxt.substr(found + 7, end - (found + 7));
			cut = 8 + styleRec.styleName.size();
		}
	}

	std::string writeTxt = remainingTxt.substr(0, found);
	remainingTxt = remainingTxt.substr(writeTxt.size() + cut, std::string::npos);

	return writeTxt;
}

/** Append newText to the current body of text and show the updated page. */
void CGUIrichText::appendText(const std::string& newText) {
	requestLineFadeIn(true); //move to renderLineBuffer, that's where it happens

	addText(newText);

	if (enableLineFadeIn) //ditto
		setBusy(true); //because it will take a little while to fade-in this text.
	//renderLineBuffer();

	if (transcriptLog)
		*transcriptLog << newText;
}

/** Append the given text to the current body of text. */
void CGUIrichText::addText(std::string newText) {
	textObjs.back().text += newText;

	compileFragmentsToEnd(lineBuffer.finalFrag());

	//overrunCorrectionOn = true;
	//Important! Here we signal that we want any overrun caused by this adding this text to be corrected.
	//ie, by scrolling.
}

void CGUIrichText::initialisePage() {
	longestLine = 0;
	lineBuffer.clear();
	textBuf.init(CLEAR_EXISTING_IMAGE);
	lineBuffer2.setPageSize(getWidth(), getHeight());
}

void CGUIrichText::writePageToLineBuffer() {
	TLineFragment startingFragment = { firstVisibleObject,firstVisibleText,0,0,0,0,0,no,0 };
	compileFragmentsToEnd(startingFragment);
}

void CGUIrichText::checkHotTextContact(const  int mouseX, const  int mouseY) {
	if (suspended || busy) {
		return;
	}

	i32vec2 localMouse = screenToLocalCoords(mouseX, mouseY);
	int oldSelectedHotObj = selectedHotObj;
	selectedHotObj = -1;
	int distToFragTop = 0;
	for (auto hotTextFrag : hotFrags) {
		TLineFragment& hotFrag = lineBuffer.getFragment(hotTextFrag.fragId);
		if (all(greaterThan(localMouse,i32vec2(hotFrag.renderStartX, hotFrag.renderStartY))) &&
					all(lessThan(localMouse, i32vec2(hotFrag.renderEndX, hotFrag.renderStartY + hotFrag.height))) ) { 
			selectedHotObj = hotFrag.textObj;
			distToFragTop = (localMouse.y - hotFrag.renderStartY);
			break;
		}
	}

	if (oldSelectedHotObj != selectedHotObj) {
		msgHotTextChange(i32vec2{ mouseX,mouseY - distToFragTop });
	}
}

/** Attempt to scroll down by the given distance in pixels. */
bool CGUIrichText::scrollDown2(int dist) {
	while (lineBuffer2.getOverlap() < dist) {
		//add lines to bottom of lineBuffer if we can
		//else break


		TLineFragment finalFrag = lineBuffer2.getFinalFrag();

		// textPos = 70ish, endPos = 50ish, hence prob?

		TCharacterPos endPos = lineBuffer2.getPageEnd();

		

		TLine newLine = compileSingleLine(finalFrag);

		if (newLine.fragments.empty())
			break;
	}

	int result = lineBuffer2.scrollDown(dist);
	return result;
}

/** Attempt to scroll up by the given distance in pixels. */
bool CGUIrichText::scrollUp2(int dist) {
	lineBuffer2.setAddFragmentsAtTop(true);
	while (lineBuffer2.getTopOverlap() < dist) {
		//TCharacterPos currentLine = { firstVisibleObject,firstVisibleText };
		TCharacterPos prevNewline = getPreviousLine(lineBuffer2.getPageStart());
		if (prevNewline == lineBuffer2.getPageStart()) {
			break;
		}
		//found previous line so make it our new first visible line
		firstVisibleObject = prevNewline.textObj;
		firstVisibleText = prevNewline.pos;
		//insert line in line buffer
		TLine newFirstLine = compileSingleLine(TLineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,0,no,0 });
	}
	lineBuffer2.setAddFragmentsAtTop(false);
	int result = lineBuffer2.scrollUp(dist);
	return result;
}


glm::vec4& CGUIrichText::getHotTextColour() {
	return hotTextStyle.colour;
}

glm::vec4& CGUIrichText::getHotTextSelectedColour() {
	return selectedHotTextStyle.colour;
}

void CGUIrichText::setStyleChange(TStyleRec& styleRec) {
	if (styleRec.styleChange == styleHotOn)
		setAppendStyleHot(true, true, styleRec.hotId);
	if (styleRec.styleChange == styleHotOff)
		setAppendStyleHot(false, true, styleRec.hotId);
	if (styleRec.styleChange == styleSuspendedHotOn)
		setAppendStyleHot(true, false, styleRec.hotId);
	if (styleRec.styleChange == styleSuspendedHotOff)
		setAppendStyleHot(false, false, styleRec.hotId);
	if (styleRec.styleChange == styleNone)
		setTextStyle(currentTextStyle);
	if (styleRec.styleChange == styleStyle)
		setTextStyle(styleRec.styleName);
}

void CGUIrichText::resize(int w, int h) {
	if (w < 1) //because bad things happen if we make the texture 0 in height or width
		w = 1;
	if (h < 1)
		h = 1;
	setWidth(w);
	setHeight(h);

	lineBuffer2.setPageSize(w, h);
	//TO DO: shouldn't we resize the textBuffer here too?
}

/** Append a textObj marking the start or end of temporary text. */
bool CGUIrichText::setTempText(bool onOff) {
	if (busy)
		return false;
	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	if (onOff) {
		textObjs[currentTextObj].flags |= richTemp;
	}
	else
		textObjs[currentTextObj].flags &= ~richTemp;
	return true;
}

/** Append a textObj marking the start or end of marked text. */
void CGUIrichText::setMarkedText(bool onOff) {
	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	if (onOff) {
		textObjs[currentTextObj].flags |= richMarked;
	}
	else
		textObjs[currentTextObj].flags &= ~richMarked;
}

/** Append a textObj marking the start or end of fade-in text. */
void CGUIrichText::setFadeText(bool onOff) {
	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	if (onOff) {
		textObjs[currentTextObj].flags |= richFadeIn;
	}
	else
		textObjs[currentTextObj].flags &= ~richFadeIn;
}

/** If objNo is a text obj with text, create a new textObj carrying over its attributes and return it.
	If objNo is textless, just return it. */
int CGUIrichText::getStyleChangeTextObjAt(int objNo) {
	if (textObjs[currentTextObj].text.size() > 0 || textObjs[currentTextObj].flags & richBookmark) {
		TRichTextRec newObj = textObjs[objNo];
		newObj.text.clear();
		newObj.period = 0;
		newObj.flags &= ~richBookmark; //we never want to carry over this
		textObjs.push_back(newObj);
		//TO DO: change to insert
		currentTextObj++;
	}
	return currentTextObj;
}

/** Remove the most recent block of text objs flagged as temporary. */
void CGUIrichText::removeTempText() {
	for ( int obj = textObjs.size() - 1; obj >= 0; obj--) {
		if (textObjs[obj].flags & richTemp) {
			textObjs.erase(textObjs.begin() + obj);	
			/////////////lineBuffer.removeObjLine(obj);
			if (obj == 0 || !(textObjs[obj - 1].flags & richTemp))
				break;
		}
	}
	currentTextObj = textObjs.size() - 1;
	
	createPage();
}

/** At the current text point, insert a bookmark textObj. */
void CGUIrichText::insertBookmark() {
	currentTextObj = getStyleChangeTextObjAt(currentTextObj);
	textObjs[currentTextObj].flags |= richBookmark;
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
bool CGUIrichText::collapseTempText() {
	if (busy)
		return false;
	removeTempText();
	//createPage();
	renderLineBuffer();
	return true;
}

/** Make the last temporary text permanent, if any. This includes making any suspended hot text active. */
bool CGUIrichText::solidifyTempText() {
	if (busy)
		return false;
	for (int obj = textObjs.size() - 1; obj > 0; obj--) {
		if (textObjs[obj].flags & richTemp) {
			textObjs[obj].flags &= ~richTemp;
			if (textObjs[obj].flags & richSuspended) {
				textObjs[obj].flags ^= richSuspended;
				textObjs[obj].style = hotTextStyle;

			}
			else {
				//////////textObjs[obj].style = defaultTextStyle;
				textObjs[obj].style = currentTextStyle;
				//TO DO: pretty sure this should be set to currenTextStyle
				//and the defaultTextStyle member can be scrapped
				//We have the stylesheet now to say what the default style is
				//we don't and shouldn't record it internally.
			}
			if (obj == 0 || !(textObjs[obj - 1].flags & richTemp)) {
				renderLineBuffer();
				break;
			}
		}
	}
	return true;
}

/** Run backwards throught the text, removing any duplicate hot texts. */
void CGUIrichText::unhotDuplicates() {
	std::vector<int> hotIds;
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
	//updatePage();
	renderLineBuffer();
}

/** Remove the last section of text flagged as marked, if any. */
void CGUIrichText::removeMarked() {
	int beforeObj = -1; int  afterObj = -1;
	for ( int obj = textObjs.size() - 1; obj >= 0; obj--) {
		if (textObjs[obj].flags & richMarked) {
			textObjs.erase(textObjs.begin() + obj);
			if (afterObj == -1)
				afterObj = obj + 1;
			beforeObj = obj - 1;
			currentTextObj = textObjs.size() - 1;
			if (obj == 0 || !(textObjs[obj - 1].flags & richMarked))
				break;
		}
	}

	if (beforeObj != -1 ) {
		setBusy(true);
		insertGapObj(beforeObj, afterObj);
	}
}

/** Insert a special text object after the before object, defining a gap equal to the distance
	between the last line to use beforeObj and the first line to use afterObj. */
void CGUIrichText::insertGapObj(int beforeObj, int afterObj) {
	TFragPos lastBeforeFrag = lineBuffer.getLastFrag(beforeObj);
	TFragPos firstAfterFrag = lineBuffer.getFirstFrag(afterObj);

	TRichTextRec gapObj = textObjs[beforeObj];
	gapObj.gap = lineBuffer.getFragment(firstAfterFrag.fragId).renderStartY -
		(lineBuffer.getFragment(lastBeforeFrag.fragId).renderStartY /*- lineBuffer.lines[lastBeforeFrag.lineNo].height*/);
	gapObj.text = "";
	gapObj.flags |= richGap;
	textObjs.insert(textObjs.begin() + beforeObj + 1, gapObj);
	currentTextObj++;
	createPage();
	textObjs[beforeObj + 1].lineRef = lineBuffer.getFirstFrag(beforeObj + 1).lineNo;
}


/** Cause any visible hot text to animate. */
void CGUIrichText::animateHotText(float dT) {
	//TO DO:!!!!!!Massive slow down - mostly in renderTextAt FIND A BETTER WAY
	CFont* font = pDrawFuncs->getFont(selectedHotTextStyle.font);
	TLineFragDrawRec dataRec = { NULL, font, selectedHotTextStyle.colour };

	int objId = -1;
	
	for (auto hotTextFrag : hotFrags) {
		TLineFragment& frag = lineBuffer.getFragment(hotTextFrag.fragId);

		if (hotTextFrag.textObj != objId) {
			objId = hotTextFrag.textObj;
			textObjs[objId].period += dT * 0.2f;
			if (textObjs[objId].period > 1)
				textObjs[objId].period = 0; // -= floor(textObjs[objId].period);
		}

		float transition = 1;
		
		if (selectedHotObj != hotTextFrag.textObj) {
			transition = std::max(textObjs[objId].period, 0.8f) - 0.8f;
			transition /= 0.2f;
			transition = sin(transition * PI);
		}
		
		dataRec.textColour = mix(hotTextStyle.colour, selectedHotTextStyle.colour, transition);
		dataRec.text = &hotTextFrag.text;
		
		textBuf.renderTextAt(frag.renderStartX, frag.renderStartY, dataRec);
	}
}

/** Cause any fading-in text to fade further in. */
void CGUIrichText::animateFadeText(float dT) {
	int objId = -1;
	TLineFragDrawRec dataRec; 
	CFont* font;

	for (auto fadeFrag = fadeFrags2.begin(); fadeFrag != fadeFrags2.end();) {
		TLineFragment& frag = lineBuffer.getFragment(fadeFrag->fragId);
		if (fadeFrag->textObj != objId) {
			objId = fadeFrag->textObj;
			textObjs[objId].period += dT * 1.5f;
			if (textObjs[objId].period > 1) {
				textObjs[objId].period = 1;
				textObjs[objId].flags &= ~richFadeIn;
			}
		}


		dataRec.font = pDrawFuncs->getFont(textObjs[objId].style.font);
			
		dataRec.text = &fadeFrag->text;
		dataRec.textColour = textObjs[objId].style.colour;
		dataRec.textColour.a = textObjs[objId].period;
		textBuf.renderTextAt(frag.renderStartX, frag.renderStartY, dataRec);

		if (textObjs[objId].period == 1)
			fadeFrag = fadeFrags2.erase(fadeFrag);
		else
			fadeFrag++;

	}
}

/** If any lines aren't at full alpha, update their alpha and redraw them. */
void CGUIrichText::animateLineFadeIn(float dT) {
	//float proportion = 790.0f / textBuf.textTexture.width;
	float proportion = lineFadeSpeed / textBuf.textTexture.width;
	bool done = true;

	int lineNo = 0;
	for (auto& line : lineBuffer.lines) {
		lineNo++;


		//////////////////////////////////////////////////////
		TLineFragment& frag = lineBuffer.getFragment(line.fragments[0]);
		int currObjNo = frag.textObj;
		TRichTextRec currentObj = textObjs[frag.textObj];
		//string renderLine = currentObj.text.substr(frag.textPos, 20);
		




		if (line.fadeInX >= 1.0) {
			continue;
		}
		done = false;
		setBusy (true);





		//line.fadeInX += 0.08 * proportion;
		line.fadeInX += proportion;
		if (line.fadeInX * textBuf.textTexture.width > line.width) {
			line.fadeInX = 1;
		}

		//draw line
		for (auto fragId : line.fragments) {
			TLineFragment& frag = lineBuffer.getFragment(fragId);
			int currObjNo = frag.textObj;

			TRichTextRec currentObj = textObjs[frag.textObj];
			std::string renderLine = currentObj.text.substr(frag.textPos, frag.textLength);
			CFont* currentFont = pDrawFuncs->getFont(currentObj.style.font);
			TLineFragDrawRec dataRec = { &renderLine, currentFont, currentObj.style.colour };
			textBuf.renderFadeInTextAt(frag.renderStartX, frag.renderStartY, dataRec, line.fadeInX);
		}

		return;
	}
	if (done) {
		requestLineFadeIn(false);
		setBusy(false);
	} 
	
}

/** If there's currently displaced text (caused by removing marked text), shrink the gap.*/
void CGUIrichText::collapseGap(float dT) {
	setBusy(true);
	auto gapObjIt = textObjs.begin() + gapObj;
	gapObjIt->period += dT;
	if (gapObjIt->period > 0.01f) { //was 0.005f;
		gapObjIt->period = 0;
		gapObjIt->gap -= 2;
		lineBuffer.lines[gapObjIt->lineRef].height = gapObjIt->gap;
		if (gapObjIt->gap < 0) {
			textObjs.erase(gapObjIt);
			currentTextObj = textObjs.size() - 1;
			setBusy(false);
			gapObj = -1;
		}
		if (gapObj != -1) {
			renderLineBuffer();
		}
		else
			createPage();
	}
	
}

/** Break the text in deliveryBuffer into characters and send them individually to the rich text control. */
void CGUIrichText::deliverByCharacter(float dT) {
	std::string text;
	charInterval += dT;
	if (charInterval < charDelay)
		return;


	text = deliveryBuffer.substr(0, 1);
	charInterval = 0;
	
	deliveryBuffer = deliveryBuffer.substr(text.size(), std::string::npos);

	//appendMarkedUpText(text);
	textObjs.back().text += text;
	renderLineBuffer();
}

/** Returns true if there's any text beyond the bottom of the page. */
bool CGUIrichText::isOverrun() {
	if (overrunningPixels > 0)
		return true;
	TLineFragment finalFrag = lineBuffer.finalFrag();
	//if (textObjs.size() > finalFrag.textObj + 1 && !textObjs.back().text.empty())
	//	return true;
	if (textObjs.size() == (finalFrag.textObj + 1) &&
		textObjs.back().text.size() > (finalFrag.textPos + finalFrag.textLength)) {
		return true;
	}
	for (unsigned int obj = finalFrag.textObj + 1; obj < textObjs.size(); obj++) {
		if (!textObjs[obj].text.empty())
			return true;
	}


	return false;
}

/** Activate/deactivate line fade-in mode, if it is enabled. */
void CGUIrichText::requestLineFadeIn(bool isOn) {
	if (!enableLineFadeIn) //TO DO: why???? Just do it!
		return;
	lineFadeInOn = isOn;
	if (lineFadeInOn) {
		lineBuffer.setLineFadeIn(0); //create lines intitially zero faded-in
		//busy = true;
	}
	else
		lineBuffer.setLineFadeIn(1.0); //create lines fully drawn
}

/** Clear all text after the last bookmark, if it exists. */
bool CGUIrichText::clearToBookMark() {
	if (busy)
		return false;
	for (int obj = textObjs.size() - 1; obj >= 0; obj--) {
		if (textObjs[obj].flags & richBookmark) {
			textObjs.erase(textObjs.begin() + obj, textObjs.end());
			currentTextObj = textObjs.size() - 1;
			createPage();
			break;
		}

	}
	return true;
}

/** Returns true only if all current text has finished rendering. */
bool CGUIrichText::isDisplayFinished() {
	if (lineFadeInOn)
		return false;
	return true;
}


TRichTextRec::TRichTextRec() {
	hotId = 0;
	flags = 0;
	period = 0;
	gap = 0;
	lineRef = 0;
}



