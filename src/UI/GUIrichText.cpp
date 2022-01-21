#include "GUIrichText.h"

#include <string_view>
#include <functional> //for hash - temp~?

#include "glm\glm.hpp"
# define PI           3.14159265358979323846  /* pi */


#include "..\3DEngine\src\utils\log.h"
#include "..\3DEngine\src\UI\GUIroot.h"

#include "UI/uiRender.h"

#include "listen/listen.h"

using namespace glm;

//std::uniform_real_distribution<> CGUIrichText::randomPeriod{ 0,1.0f };

CGUIrichText::CGUIrichText(int x, int y, int w, int h) : updateDt(0),
//CGUIbase(x,y,w,h) 
												CguiBase(x, y, w, h)
{
	prepForFirstText();
	prepForScrolling();
	setResizeMode(resizeNone); 
}

/** Append new text to the current body of text and show the updated page. */
void CGUIrichText::appendMarkedUpText(const std::string& text) {
	autoscrollingDown = true;
	bodyText.addText(text);
	createPage();
}


void CGUIrichText::Draw(float dT) {
	update(dT);
	CGUIbase::Draw(dT);
}

/**	Internally create a page from body text, ready to display. */
void CGUIrichText::createPage() {
	initialisePage();
	writePageToLineBuffer();
}

void CGUIrichText::DrawSelf() {
	lineBuffer2.renderSprites(dT);
	uiDraw::drawTexture(drawBox, *lineBuffer2.getTextBuf());

	if (drawBorder) { //should be an overridable basic drawborder routine in the superclass
		uiDraw::drawBorder(drawBox, borderColour);
	}
}

//TO DO: ugh, this is a sign of deep problems. FIXME
bool CGUIrichText::isEmpty() {
	//return textObjs.empty() || (textObjs.size() == 1 && textObjs[0].text =="");
	return bodyText.textEmpty;
}


/** Set the font to be used by subsequent text we receive. */
void CGUIrichText::setDefaultFont(const std::string& fontName) {
	defaultFontName = fontName;
	bodyText.setWriteFont(fontName);
}

/** Set the colour to be used by subsequent text we receive. */
void CGUIrichText::setColour(glm::vec4& colour) {
	bodyText.setWriteColour(colour);
}


void CGUIrichText::setAutoscrollDown(bool onOff) {
	autoscrollingDown = onOff;
}

/** Check for mouse over hot text. */
bool CGUIrichText::OnMouseMove(const  int mouseX, const  int mouseY, int key) {	
//	checkHotTextContact(mouseX, mouseY);
	return true;
}

void CGUIrichText::onMouseMove(glm::ivec2& mousePos) {
	i32vec2 localMouse = screenToLocalCoords(mousePos.x, mousePos.y);
	std::string msg = lineBuffer2.onMouseMove(localMouse);
	if (msg != lastHotTxt) {
		lastHotTxt = msg;
		CEvent e;
		e.type = eHotTextHover;
		e.data = msg;
		lis::event(e);
	}
}

/** Announce that the currently selected hot text has changed. */ 
void CGUIrichText::msgHotTextChange(glm::i32vec2& adjustedMousePos) {
//	CMessage msg = { uiMsgHotTextChange };
//	//if (selectedHotObj != -1) {
//	if (currentHotText != -1) {
//		//msg.value = textObjs[selectedHotObj].hotId; //formerly!
//		msg.value = currentHotText;// textObjs[currentHotText].hotId;
//		msg.x = adjustedMousePos.x; msg.y = adjustedMousePos.y;
//	}
//	else
//		msg.value = -1;
//	parent->message(this, msg);
}

/** Detect clicks on any menu items. */
bool CGUIrichText::OnLMouseDown(const int mouseX, const int mouseY, int key) {
	
	//TGUImessage msg2;
	//msg2.msg = uiMsgHotTextClick;
	//msg2.value = currentHotText; //NB, was done with msg.value = textObjs[selectedHotObj].hotId;
	//msgObj->GUImsg(uniqueID, msg2);

	//THotCallRec hotCall = hotCallRecs[currentHotText];

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

bool CGUIrichText::OnClick(const int mouseX, const int mouseY) {
	i32vec2 localMouse = screenToLocalCoords(mouseX, mouseY);
	std::string hotMsg = lineBuffer2.onMouseMove(localMouse);
	if (hotTextClickHandler)
		hotTextClickHandler(hotMsg);
	return true;
}


/** Check for losing mouse while hot text selected. */
void CGUIrichText::onMouseOff() {
	lineBuffer2.onMouseOff();
	lastHotTxt = "";
	CEvent e;
	e.type = eMouseOff;
	e.id = uniqueID;
	lis::event(e);
}



void CGUIrichText::update(float dT) {
	this->dT = dT;

	updateDt += dT;
	if (updateDt > correctOverrunDelay)
	{
		updateDt = 0;
		if (autoscrollingDown) {
			 autoscrollingDown = scrollDown3(smoothScrollStep);
		}
	}
	
}


bool CGUIrichText::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	int direction = wheelDelta > 0 ? 1 : -1;
	smoothScroll(direction * smoothScrollStep);

	return true;
}


/** Scroll the visible text by the given number of pixels, if possible. */
void CGUIrichText::smoothScroll(int dist) {
	if (dist > 0) {
		scrollUp3(dist);
	}
	else {
		scrollDown3(abs(dist));
	}
}


/** Remove all text from the control. */
void CGUIrichText::clear() {
	//textObjs.clear();
	//hotCallRecs.clear();
	//firstVisibleText = 0;
	//TRichTextRec defaultStyle;
	//defaultStyle.style = currentTextStyle;
	//textObjs.push_back(defaultStyle);
	//currentTextObj = 0;
	//firstVisibleObject = 0;
	//selectedHotObj = -1;
	bodyText.clear();
	if (!defaultFontName.empty())
		bodyText.setWriteFont(defaultFontName);
	lineBuffer2.clear();
	createPage();
}

/** Unselect any selected hot text. */
void CGUIrichText::clearSelection() {
	//if (currentHotText == -1)
	//	return;
	//currentHotText = -1;
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
	int heightModifier = fnt::get(bodyText.getReadStyle().fontName)->lineHeight;
	//uiDraw::getFont(textObjs[currentTextObj].style.font)->lineHeight;

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



/**	Set everything up for the first text to be appended.*/
void CGUIrichText::prepForFirstText() {

	drawBorder = false;

	//lineBuffer2.setCallbackObj(this);
	lineBuffer2.setPageSize(getWidth(), getHeight());
	autoscrollingDown = false;
}

/** Ready the system to handle scrolling. */
void CGUIrichText::prepForScrolling() {
	correctOverrunDelay = 0.001f;
	overrunCorrectionOn = true;
	smoothScrollStep = 8;
}

void CGUIrichText::initialisePage() {
	longestLine = 0;
	lineBuffer2.setPageSize(getWidth(), getHeight());
}

/** Send a pageful of text to the lineBuffer for rendering. */
void CGUIrichText::writePageToLineBuffer() {
	bodyText.setReadPos(0); //TO DO: set to a stored start pos
	bool pageOverrun = false;

	while (!bodyText.endOfText()) {
		if (pageOverrun) //TO DO: may wabt to scroll rather than stop
			return;

		pageOverrun = writeLineToLineBuffer();
	}
}

/** Send the line at the current read position to lineBuffer, 
	updating the read position in the process. */ 
bool CGUIrichText::writeLineToLineBuffer() {
	if (bodyText.endOfText())
		return false;

	TextStyle currentStyle = bodyText.getReadStyle();
	CFont* currentFont = fnt::get(currentStyle.fontName.data());
	int leadingPixelY = currentFont->lineHeight;

	int breakOnChar = findLastBreakableChar();
	int fragTextStart = bodyText.getReadPos();
	int fragCharCount = 1;
	int totalCharCount = 0;

	TFragment2 fragment = { currentStyle,"",currentFont->lineHeight,false, fragTextStart};

	char c = bodyText.readNextChar();
	while (!bodyText.endOfText()) {
		if (bodyText.styleChange()) {
			fragment.text = bodyText.getStringAt(fragTextStart, fragCharCount);

			if (totalCharCount+1 == breakOnChar) {
				fragment.causesNewline = true;
				return lineBuffer2.addTextSprite(fragment);
			}
			else
				lineBuffer2.addTextSprite(fragment);

			currentStyle = bodyText.getReadStyle();
			currentFont = fnt::get(currentStyle.fontName.data());
			leadingPixelY = max(leadingPixelY, currentFont->lineHeight);
			fragCharCount = 0;
			fragTextStart = bodyText.getReadPos();
			fragment = { currentStyle,"",currentFont->lineHeight,false };
			fragment.textStart = fragTextStart; //021221 added to correct lineBuffer.getPageTextEnd(), but keep an eye on
		}

		totalCharCount++;

		if (totalCharCount == breakOnChar) {
			fragment.text = bodyText.getStringAt(fragTextStart, fragCharCount);
			fragment.textStart = fragTextStart;
			fragment.causesNewline = true;
			return lineBuffer2.addTextSprite(fragment);
		}

		c = bodyText.readNextChar();
		fragCharCount++;
	}


	fragment.text = bodyText.getStringAt(fragTextStart, fragCharCount);
	return lineBuffer2.addTextSprite(fragment);
}

/** Send to lineBuffer the line ending immediately before the current start of page position.*/
void CGUIrichText::writePrevLineToLineBuffer() {
	int pageTextStart = lineBuffer2.getPageTextStart();
	if (pageTextStart == 0)
		return;

	int currentPos = bodyText.getReadPos();
	int prevPara = bodyText.getPrevPara(pageTextStart);

	//find start of line before page text starts
	bodyText.setReadPos(prevPara);
	int lineStart = prevPara;
	int newStart = prevPara;
	int lineEnd = lineStart + findLastBreakableChar();
	while (lineEnd < pageTextStart) {
		newStart = lineEnd;
		lineEnd += findLastBreakableChar();
	}

	
	bodyText.setReadPos(newStart);
	writeLineToLineBuffer();

	bodyText.setReadPos(currentPos);
}

/** Send to lineBuffer the line starting immediately after the current last line of the page.*/
void CGUIrichText::writeNextLineToLineBuffer() {
	int pageTextEnd = lineBuffer2.getPageTextEnd();
	bodyText.setReadPos(pageTextEnd);
	writeLineToLineBuffer();
}

/** Return the position of the last breakable character in the
	line that can be composed from the current read position. */
int CGUIrichText::findLastBreakableChar() {
	int start = bodyText.getReadPos();
	
	CFont* currentFont = fnt::get(bodyText.getReadStyle().fontName);

	int lastBreakableChar = 0; int totalLineChars = 0;
	int leadingPixelX = 0;
	char c = bodyText.readNextChar();
	while (!bodyText.endOfText()) {
		totalLineChars++;
		if (bodyText.styleChange()) {
			currentFont = fnt::get(bodyText.getReadStyle().fontName);
		}

		leadingPixelX += currentFont->table[c]->width;
		if (leadingPixelX > getWidth()) {
			break;
		}

		if (isspace(c) || c == '-') {
			lastBreakableChar = bodyText.getReadPos() - start;
		}

		if (c == '\n') {
			lastBreakableChar = bodyText.getReadPos() - start;
			break;
		}

		c = bodyText.readNextChar();
	}

	if (bodyText.endOfText()) {
		lastBreakableChar = totalLineChars+1;
	}

	bodyText.setReadPos(start);
	return lastBreakableChar;
}

/** Message the assigned handler with any mouse action on hotText. */
void CGUIrichText::checkHotTextContact(const  int mouseX, const  int mouseY) {
	i32vec2 localMouse = screenToLocalCoords(mouseX, mouseY);
	std::string msg = lineBuffer2.onMouseMove(localMouse);
		if (hotTextMouseoverHandler)
			hotTextMouseoverHandler(msg);
}



bool CGUIrichText::scrollDown3(int dist) {
	int result = lineBuffer2.scrollDown3(dist);

	if (result < dist && result > 0) {
		writeNextLineToLineBuffer();
		result = lineBuffer2.scrollDown3(result);
	}
	return result;
}


bool CGUIrichText::scrollUp3(int dist) {
	int result = lineBuffer2.scrollUp3(dist);

	if (result < dist) {
		lineBuffer2.setaddFragmentsAtTop(true);
		writePrevLineToLineBuffer();
		lineBuffer2.setaddFragmentsAtTop(false);
		result = lineBuffer2.scrollUp3(result);
	}
	return result;
}


/** Respond to hot text being moused over (or off) .*/
//void CGUIrichText::hotTextMouseOver(int hotId) {
//	//currentHotText = hotId;
//
//	//if (currentHotText == oldHotText)
//	//	return;
//
//	//oldHotText = currentHotText;
//
//	//i32vec2 localMouse = screenToLocalCoords(rootUI->mousePos.x, rootUI->mousePos.y);
//	//int distToFragTop = 10;
//	//msgHotTextChange(i32vec2{ localMouse.x,localMouse.y - distToFragTop });
//
//}


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


void CGUIrichText::onDrag(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiMsgDragging;
	msg.x = mouseX;
	msg.y = mouseY;
	parent->message(this, msg);
}




