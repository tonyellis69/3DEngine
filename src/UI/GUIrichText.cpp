#include "GUIrichText.h"

#include <string_view>
#include <functional> //for hash - temp~?

#include "glm\glm.hpp"
# define PI           3.14159265358979323846  /* pi */


#include "..\3DEngine\src\utils\log.h"
#include "..\3DEngine\src\UI\GUIroot.h"

using namespace glm;

std::uniform_real_distribution<> CGUIrichText::randomPeriod{ 0,1.0f };

CGUIrichText::CGUIrichText(int x, int y, int w, int h) : updateDt(0), 
														transcriptLog(NULL),
														CGUIbase(x,y,w,h) 
{

	TTempTestRec rootObj;
	rootObj.hotCall = std::make_unique< TTempHotCallRec>();
	rootObj.hotCall->dummy = 42;
	rootObj.otherParam = "77";

	tempObjs.push_back((rootObj));
	TTempTestRec newObj = tempObjs[0];
	tempObjs.push_back(newObj);


	prepForFirstText();
	prepForScrolling();
	setResizeMode(resizeNone); //was setResizeMode(resizeByWidthMode);
	
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

	//createPage(); //shouldn't have to recompile page because of one added clause!

	autoscrollingDown = true;
}

void CGUIrichText::Draw(float dT) {
	update(dT);
	CGUIbase::Draw(dT);
}

/**	Compose all the currently in-view text into a single image, ready for drawing to the screen. */
void CGUIrichText::createPage() {
	initialisePage();
	writePageToLineBuffer();
}

void CGUIrichText::applyStyleSheet() {
	//setTextColour(style::defaultFontColour); //TO DO: prob redundant now
	refreshCurrentTextStyles();
}

void CGUIrichText::DrawSelf() {
	lineBuffer2.renderSprites(dT);
	//if (uniqueID == 402) {
	//	int b = 9;// lineBuffer2.getTextBuf()->savePNG("d:\\statBuf.png");

	//}
	pDrawFuncs->drawTexture(drawBox, *lineBuffer2.getTextBuf());

	if (drawBorder) { //should be an overridable basic drawborder routine in the superclass
		pDrawFuncs->drawBorder2(drawBox, (vec4&)borderColour);
	}
}

TRichTextRec* CGUIrichText::getTexObjCallback(int objNo) {
	return &textObjs[objNo];
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

/** Set the style for any text appended after this. */
bool CGUIrichText::setTextStyle(const std::string& styleName) {
	/*if (styles == NULL)
		return false;*/

	//is this a named style in the current theme?
	for (auto style : *styles) {
		if (style.first == styleName) {
			setTextStyle(style.second);
			return true;
		}
	}

	//no? then it may be one of these kludgy psuedo-styles I should totally get rid of
	
	if (styleName == "tempOn") {
		setTempText(true); return true;
	}
	if (styleName == "tempOff") {
		setTempText(false); return true;
	}

	if (styleName == "markOn") {
		setMarkedText(true); return true;
	}
	if (styleName == "markOff") {
		setMarkedText(false); return true;
	}


	if (styleName == "fadeOn") {
		setFadeText(true); return true;
	}
	if (styleName == "fadeOff") {
		setFadeText(false); return true;
	}

	if (styleName == "bookmark") {
		insertBookmark(); return true;
	}

	fatalLog << "\nText style " << styleName << " not found.";

	return false;
}


/** Update the set of text styles to use */
void CGUIrichText::refreshCurrentTextStyles() {
	styles = (std::map<std::string, TtextStyle> *) &style::themes2.at(currentTheme).styles;

	for (auto style : *styles) {
		if (style.first == "default")
			;// defaultTextStyle = style.second;
		else if (style.first == "hot")
			hotTextStyle = style.second;
		else if (style.first == "hotSelected")
			selectedHotTextStyle = style.second;
	}

	setTextStyle("default");
}


/** Set the current theme, and load the styles of that theme, for use when requested. */
void CGUIrichText::setTextTheme(const std::string& themeName) {
	if (style::themes2.find(themeName) == style::themes2.end())
		fatalLog << "\nUnable to load text theme " << themeName;

	currentTheme = themeName;
	refreshCurrentTextStyles();
}


/** Set the text of the current text object.*/
void CGUIrichText::setText(std::string newText) {
	firstVisibleText = 0;
	textObjs[currentTextObj].text = newText;
}

void CGUIrichText::setAutoscrollDown(bool onOff) {
	autoscrollingDown = onOff;
}




/** Starting with the given line fragment, compile our text objects into line fragments 
	until we run out, or overflow the page. */
void CGUIrichText::compileFragmentsToEnd(TPagePos fragmentStart) {
	TLineFragment lineFragment = { fragmentStart.textPos.textObj,fragmentStart.textPos.pos,0,
		0,0,fragmentStart.screenPos.x,no};
	
	do {
		lineFragment = compileSingleLine(lineFragment);
	} while (!endOfText(readPoint));  //check if rwHead = eof here instead
}


/** Create one line of line fragments from our text, starting after the given one. */
TLineFragment CGUIrichText::compileSingleLine(TLineFragment lineFragment) {
	readPoint = { lineFragment.textObj, lineFragment.textPos + lineFragment.textLength };

	//check for rwHead = eof 

	do { 
		lineFragment = getNextLineFragment(lineFragment);

		if (endOfText(readPoint) && lineFragment.textLength == 0)
			break;

		lineBuffer2.addTextSprite(lineFragment);
	} while (lineFragment.causesNewLine == no); //move check inside loop?
	return lineFragment;
}


/** Return the text fragment following the given one. */
TLineFragment CGUIrichText::getNextLineFragment(const TLineFragment& currentLineFrag) {
	TLineFragment nextLineFrag = findNextFragmentStart(currentLineFrag);

	if (endOfText(readPoint)) {
		 return nextLineFrag;
	}	

	nextLineFrag = findFragmentEnd(nextLineFrag);
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
	//if (selectedHotObj != -1) {
	if (currentHotText != -1) {
		//msg.value = textObjs[selectedHotObj].hotId; //formerly!
		msg.value = currentHotText;// textObjs[currentHotText].hotId;
		msg.x = adjustedMousePos.x; msg.y = adjustedMousePos.y;
	}
	else
		msg.value = -1;
	parent->message(this, msg);
}

/** Detect clicks on any menu items. */
bool CGUIrichText::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	if (suspended || busy)
		return true;
	
	TGUImessage msg2;
	msg2.msg = uiMsgHotTextClick;
	msg2.value = currentHotText; //NB, was done with msg.value = textObjs[selectedHotObj].hotId;
	msgObj->GUImsg(uniqueID, msg2);

	THotCallRec hotCall = hotCallRecs[currentHotText];

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
	if (currentHotText == -1) //no hot text selected, so mousing off doesn't matter
		return true;
	clearSelection();
	CMessage msg = { uiMsgHotTextChange };
	msg.value = -1;
	parent->message(this, msg);

	return true;
}


/** Return the first lineFragment of the previous line, if any. */
TCharacterPos CGUIrichText::getPreviousLine(TPagePos& startText) {
	int origStartText = startText.textPos.pos;
	int origStartObject = startText.textPos.textObj;

	//find previous newline - we can work forward from ther
	TCharacterPos prevNewline = getPrevNewline(origStartObject, origStartText - 1);//-1 ensures we skip a newline that caused this line

	TCharacterPos lastLineStart = prevNewline;// { origStartObject, origStartText };


	//any word wraps between here and where we started?
	TLineFragment lineFragment{ prevNewline.textObj,prevNewline.pos,0,0,0,0,no };
	do {
		if (lineFragment.causesNewLine == wordwrap) {
			lastLineStart.pos = lineFragment.textPos + lineFragment.textLength;
			lastLineStart.textObj = lineFragment.textObj;
		}

		readPoint.textObj = lineFragment.textObj;
		readPoint.pos = lineFragment.textPos;
		lineFragment = getNextLineFragment(lineFragment); //TO DO should always be next wordwrap

	} while (lineFragment.textObj < origStartObject ||
		(lineFragment.textObj == origStartObject && (lineFragment.textPos + lineFragment.textLength) < (origStartText - 1)));

	return lastLineStart;
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
	this->dT = dT;

	updateDt += dT;
	if (updateDt > correctOverrunDelay)
	{
		updateDt = 0;
		if (autoscrollingDown) {
			 autoscrollingDown = scrollDown2(smoothScrollStep);
		}
	}
	

	//if (!fadeFrags2.empty())
		//animateFadeText(dT);
	//if (lineFadeInOn) 
	//	animateLineFadeIn(dT);
}


bool CGUIrichText::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	int direction = wheelDelta > 0 ? 1 : -1;
	smoothScroll(direction * smoothScrollStep);

	return true;
}


/** Scroll the visible text by the given number of pixels, if possible. */
void CGUIrichText::smoothScroll(int dist) {
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


/** Convert all existing hot text objects to standard style. */
std::vector<unsigned int> CGUIrichText::purgeHotText() {
	std::vector<unsigned int> purgedIds;
	for (auto &textObj : textObjs) {
		if (textObj.hotId != 0) {
			purgedIds.push_back(textObj.hotId);
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
	hotCallRecs.clear();
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
	if (currentHotText == -1)
		return;
	currentHotText = -1;
}

void CGUIrichText::updateAppearance() {
	CGUIbase::updateAppearance();
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

	int overrun = lineBuffer2.getOverlap();
	int underrun2 = lineBuffer2.getUnderrun();

	while (overrun || underrun2) {
		if (overrun) {
			previouslyOverrun = true;
			newHeight = getHeight() + heightModifier; //add space for one more line and see where that gets us
		}
		else {
			if (previouslyOverrun) { //if we're now underrunning, get out to avoid loop
				break;
			}
			newHeight = getHeight() - underrun2; //we know how line to shrink by
		}
		newWidth = newHeight * ratio;

		resize(newWidth, newHeight);
		createPage(); //updates the overrunningPixels/underrun registers. Does the text fit now?
		overrun = lineBuffer2.getOverlap();
		underrun2 = lineBuffer2.getUnderrun();
	}

	//tidy any remaining underrun
	newHeight = getHeight() - underrun2;
	resize(newWidth, newHeight);
}

/** Adjust width to fit longest visible line, then adjust height as needed. */
void CGUIrichText::resizeByWidth() {
	int resizeW = getWidth();
	if (resizeW != longestLine) {
		resizeW = longestLine; // +rightBorder;
	}
	resizeW = min(resizeW, resizeMax.x);
	setWidth(resizeW);
	createPage();

	//do we need to adjust height too?
	int overrun = lineBuffer2.getOverlap();	
	int resizeGuess = overrun;
	while (overrun) {
		if (getHeight() + resizeGuess > maxResizeHeight) {  //can't resize beyond maxResizeHeight
			break;
		}
		else
			setHeight(getHeight() + resizeGuess);
		resizeGuess *= 2;
		createPage();
		overrun = lineBuffer2.getOverlap();
	}

	int underrun2 = lineBuffer2.getUnderrun();
	if (underrun2 > 0) {
		setHeight(getHeight() - underrun2);
	}
}

void CGUIrichText::setResizeMode(TResizeMode mode) {
	resizeMode = mode;
	maxResizeHeight = 1000;
	longestLine = 0;
}

/** Assign the virtual machine to send hot text calls to. */
void CGUIrichText::setHotTextVM(Ivm* vm) {
	pVM = vm;
}

CGUIrichText::~CGUIrichText() {

}

/**	Set everything up for the first text to be appended.*/
void CGUIrichText::prepForFirstText() {
	TRichTextRec rootObj;
	textObjs.push_back(rootObj);
	currentTextObj = 0;
	firstVisibleText = 0;
	firstVisibleObject = 0;
	currentHotText = -1;
	shortestSpaceBreak = 0;
	suspended = false;
	setBusy(false);
	drawBorder = false;
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
	smoothScrollStep = 8;
}

std::string CGUIrichText::findNextTag( std::string& remainingTxt, TStyleRec& styleRec) {
	int cut = 0;
	styleRec = { styleNone, 0, "" };
	std::string_view remainingTxtView = remainingTxt;
	size_t found = remainingTxtView.find('\\');
	if (found != std::string::npos) {
		cut = 1;
		if (remainingTxtView[found + 1] == 'h' || remainingTxtView[found + 1] == 'S') {
		
			if (remainingTxtView.size() > found + 2 && remainingTxtView[found + 2] == '{') {
				std::tie(styleRec.hotId, cut) = extractHotParams(remainingTxtView.substr(found));

				if (remainingTxtView[found + 1] == 'h')
					styleRec.styleChange = styleHotOn;
				else
					styleRec.styleChange = styleSuspendedHotOn;
			}
			else {
				cut = 2;
				if (remainingTxtView[found + 1] == 'h')
					styleRec.styleChange = styleHotOff;
				else
					styleRec.styleChange = styleSuspendedHotOff;
			}
		}

		//other markup checks here
		if (remainingTxtView.substr(found + 1, 6) == "style{") {
			styleRec.styleChange = styleStyle;
			size_t end = remainingTxtView.find("}", found);
			styleRec.styleName = remainingTxtView.substr(found + 7, end - (found + 7));
			cut = 8 + styleRec.styleName.size();
		}
	}

	std::string writeTxt(remainingTxtView.substr(0, found));
	remainingTxt = remainingTxtView.substr(writeTxt.size() + cut, std::string::npos);

	return writeTxt;
}

std::tuple<unsigned int, size_t> CGUIrichText::extractHotParams(std::string_view& textView) {
	int tagStartOffset = 3; //for '\h{'
	size_t endTagPos = textView.substr(tagStartOffset).find('}');

	std::string idStr(textView.substr(tagStartOffset, 25)); //25 = short string optimisation
	size_t sz;
	unsigned int hotId = std::stoi(idStr, &sz);

	if (endTagPos != sz) { //trap for 1-digit hot texts, which I should probably scrap anyway
		int offset = sz;
		unsigned int objId = std::stoi(idStr.substr(offset), &sz);
		offset += sz;
		unsigned int memberId = std::stoi(idStr.substr(offset), &sz);
		offset += sz+1;
		std::string params = idStr.substr(offset, endTagPos - offset);
		hotCallRecs[hotId] = { objId, memberId, params };
		textObjs[currentTextObj].hotCall = std::make_shared<THotCallRec>( objId, memberId, params );
	}

	return { hotId, tagStartOffset + endTagPos + 1 };
}

/** Append newText to the current body of text and show the updated page. */
void CGUIrichText::appendText(const std::string& newText) {
	textObjs.back().text += newText;
	compileFragmentsToEnd(lineBuffer2.getPageEnd());

	if (transcriptLog)
		*transcriptLog << newText;
}

TLineFragment CGUIrichText::findNextFragmentStart(const TLineFragment& currentFragment) {
	TLineFragment nextLineFrag{}; //initialise all to 0
	int textObj = currentFragment.textObj;
	unsigned int textStartPos = currentFragment.textPos + currentFragment.textLength;

	//readPoint.pos += currentFragment.textLength;
	readPoint.pos = currentFragment.textPos + currentFragment.textLength;
	if (readPoint.pos >= textObjs[textObj].text.size()) {
		readPoint.pos = 0;
		readPoint.textObj++;
		if (endOfText(readPoint)) {
			return nextLineFrag;
		}
	}

	if (textStartPos >= textObjs[textObj].text.size()) {
		textObj++;
		textStartPos = 0;
	}

	nextLineFrag.textObj = textObj;
	nextLineFrag.textPos = textStartPos;
	nextLineFrag.renderStartX = findNextFragmentXPos(currentFragment);
	return nextLineFrag;
}

/** Catch unbreakable full stops, etc, in the next object.
	disqualify them, however, if they are preceeded by whitespace. */
int CGUIrichText::findNextUnbreakableChar(int textObj, CFont* font) {
	int unbreakableCharWidth = 0;
	if (textObj + 1 < textObjs.size()) {
		char lookAheadChar = textObjs[textObj + 1].text[0];
		if (ispunct(lookAheadChar) && textObjs[textObj].text.size() > 0
			&& !isspace(textObjs[textObj].text.back()))
			unbreakableCharWidth = font->table[lookAheadChar]->width;
	}
	return unbreakableCharWidth;
}

int CGUIrichText::findNextFragmentXPos(const TLineFragment& currentLineFrag) {
	switch (currentLineFrag.causesNewLine) {
	case(wordwrap): return 0;
	case(newline): return  textObjs[currentLineFrag.textObj].firstLineIndent;
	case(no): return currentLineFrag.renderEndX;
	}
}

TLineFragment CGUIrichText::findFragmentEnd( TLineFragment fragment) {
	int textObj = fragment.textObj;
	unsigned int textStartPos = fragment.textPos;
	int renderX = fragment.renderStartX;

	CFont* font = pDrawFuncs->getFont(textObjs[textObj].style.font);

	std::string& text = textObjs[textObj].text;

	if (text == "Test1")
		int b = 0;

	int unbreakableCharWidth = findNextUnbreakableChar(textObj, font);
	int breakPoint = textStartPos; int breakPointX = renderX; unsigned int c;
	for (c = textStartPos; c < text.size(); c++) {
		unsigned char character = (unsigned char)text[c];
		renderX += font->table[character]->width;
		if (text[c] == '\n') {
			c++;
			fragment.causesNewLine = newline;
		//	fragment.renderStartX = 0;
			break;
		}
		if ((isspace(character) || character == '-') && renderX > shortestSpaceBreak) {
			breakPoint = c + 1;
			breakPointX = renderX;
			//break; //pretty sure this works without break as long as we go to next clause
		}
		if (renderX > getWidth() || renderX + unbreakableCharWidth > getWidth()) {

			if (breakPointX > 0) {
				c = breakPoint;
				renderX = breakPointX;
				fragment.causesNewLine = wordwrap;
				break;
			}
			else if (resizeMode == resizeNone)
			{ ///////////////////////////////Experimental! not fully tested attempt to handle overlong lines
				//Problem: prevents the pop-up menus from expanding to fit their contents
				//solution: have a no-resizing mode
				renderX = breakPointX;
				fragment.causesNewLine = wordwrap;
				break;
			}
		}
	}

	fragment.textLength = c - textStartPos;
	fragment.renderEndX = renderX;

	fragment.height = font->lineHeight;

	longestLine = std::max(longestLine, fragment.renderEndX);

	readPoint.pos += c - textStartPos;
	readPoint.textObj = textObj;

	return fragment;
}


void CGUIrichText::initialisePage() {
	longestLine = 0;
	lineBuffer2.setPageSize(getWidth(), getHeight());
	//hotCallRecs.clear();
}

void CGUIrichText::writePageToLineBuffer() {
	TPagePos pageStart;
	pageStart.textPos = { firstVisibleObject, firstVisibleText };
	compileFragmentsToEnd(pageStart);
}

void CGUIrichText::checkHotTextContact(const  int mouseX, const  int mouseY) {
	if (suspended || busy) {
		return;
	}

	i32vec2 localMouse = screenToLocalCoords(mouseX, mouseY);
	lineBuffer2.onMouseMove(localMouse);
}

/** Attempt to scroll down by the given distance in pixels. */
bool CGUIrichText::scrollDown2(int dist) {
	while (lineBuffer2.getOverlap() < dist) {
		
		//add lines to bottom of lineBuffer if we can
		//else break
		
		TPagePos endPos = lineBuffer2.getPageEnd();
		//we recreate the previous fragment to provide a starting point
		TLineFragment recreatedFrag;
		recreatedFrag.causesNewLine = lineBuffer2.doesPageEndWithNewline() ? newline : no;
		recreatedFrag.textObj = endPos.textPos.textObj;
		recreatedFrag.textPos = endPos.textPos.pos;
		recreatedFrag.textLength = 0;
	
		recreatedFrag = compileSingleLine(recreatedFrag);
		if (endOfText(readPoint))
			break;

		//TO DO all that needs to be returned is whether we've run out of text Objs
		//can this be simplified?
	}

	int result = lineBuffer2.scrollDown(dist);
	return result;
}

/** Attempt to scroll up by the given distance in pixels. */
bool CGUIrichText::scrollUp2(int dist) {
	lineBuffer2.setAddFragmentsAtTop(true);
	while (lineBuffer2.getTopOverlap() < dist) {

		TCharacterPos prevNewline = getPreviousLine(lineBuffer2.getPageStart());
		if (prevNewline == lineBuffer2.getPageStart().textPos) {
			break;
		}
		//found previous line so make it our new first visible line
		firstVisibleObject = prevNewline.textObj;
		firstVisibleText = prevNewline.pos;
		//insert line in line buffer
		compileSingleLine(TLineFragment{ firstVisibleObject,firstVisibleText,0,0,0,0,no});
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

/** Respond to hot text being moused over (or off) .*/
void CGUIrichText::hotTextMouseOver(int hotId) {
	currentHotText = hotId;

	if (currentHotText == oldHotText)
		return;

	oldHotText = currentHotText;

	i32vec2 localMouse = screenToLocalCoords(rootUI->mousePos.x, rootUI->mousePos.y);
	int distToFragTop = 10;
	msgHotTextChange(i32vec2{ localMouse.x,localMouse.y - distToFragTop });
	

}

/** Return true if this character position is beyond
	our existing text. */
bool CGUIrichText::endOfText(TCharacterPos textPos) {
	if (textPos.textObj >= textObjs.size())
		return true;
	if (textPos.textObj == textObjs.size() - 1 && textPos.pos >= textObjs[textPos.textObj].text.size())
		return true;
	return false;
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
		//TRichTextRec newObj = std::move(textObjs[objNo]);
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
	//renderLineBuffer();
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
				//renderLineBuffer();
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
	//renderLineBuffer();
}

//TO DO: scrap asap
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
		//insertGapObj(beforeObj, afterObj);
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
	//renderLineBuffer();
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
	//if (lineFadeInOn)
	//	return false;
	return true;
}






