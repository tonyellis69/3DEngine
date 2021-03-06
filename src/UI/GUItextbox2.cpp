#include "GUItextbox2.h"

using namespace glm;

CGUItextbox2::CGUItextbox2(int x, int y, int w, int h) {
	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);
	renderOffset = i32vec2(2,0);
	font = defaultFont;
	cursorTextPos = 0;

	textBuf.setSize(w, h);

	type = uiTextbox;
	setTextColour(0, 0, 0, 1);
	setText("textbox");
	setBackColour1(style::uiWhite);
	setBackColour2(style::uiWhite);
}

void CGUItextbox2::setFont(CFont* newFont) {
	font = newFont;
}

void CGUItextbox2::setText( std::string newText) {
	text = newText;
	cursorTextPos = text.size();
	renderText();
}

void CGUItextbox2::setTextColour(float r, float g, float b, float a) {
	textColour.r = r; textColour.g = g; textColour.b = b; textColour.a = a;
}

void CGUItextbox2::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}



void CGUItextbox2::DrawSelf() {
	pDrawFuncs->setClip(Clipbox);

	//draw box
	//setBackColour1(style::uiWhite);
//	setBackColour2(style::uiWhite);
//	pDrawFuncs->drawCtrlRect(*this);
	pDrawFuncs->drawRect2(drawBox, (vec4&)backColour1, (vec4&)backColour2);

	if (KeyCapture == this) {
		//draw cursor
		pDrawFuncs->drawCursor2(cursor);
	}

	pDrawFuncs->drawTexture(drawBox, textBuf.textTexture);

	//draw border
	if (drawBorder) {
		if (MouseOver == this || KeyCapture == this) {
			//setBorderColour(UIdarkGrey);
			pDrawFuncs->drawBorder2(drawBox, style::uiDarkGrey);
		}
		else {
			//setBorderColour(UIlightGrey);
			pDrawFuncs->drawBorder2(drawBox, style::uiLightGrey);
		}
		//pDrawFuncs->drawCtrlBorder(*this);
	}
}

guiRect& CGUItextbox2::getCursorPos() {
	return cursor;
}

void CGUItextbox2::calcCursorPosition() {
	//lazily assuming single line text for now
	float dist = renderOffset.x;
	for (int c = 0; c < cursorTextPos; c++) {
		dist += font->table[text[c]]->width;
	}

	float yLength = font->lineHeight;
	float xPos = dist;

	cursor.pos = drawBox.pos + glm::i32vec2( xPos, (getHeight() - yLength)/2 );
	cursor.size = {1,  yLength };
}

/** Determine offset due to centering, etc. */
void CGUItextbox2::calcLineOffset() {
	renderOffset.y = (getHeight() - font->lineHeight + 4) / 2.0;
}


/**User has clicked on the textbox, so give it focus, allowing it to be edited. */
bool CGUItextbox2::OnClick(const  int mouseX, const  int mouseY) {
	captureKeyboard();
	calcCursorPosition();
	return true;
}

/**User has pressed a key, and we have focus. Use this to catch cursor keys, backspace,etc*/
void CGUItextbox2::onKeyPress(unsigned int Key, long Mod) {
	switch (Key) {
	case GLFW_KEY_LEFT:	moveCursor(-1, 0);
					break;
	case GLFW_KEY_RIGHT:	moveCursor(1, 0);
					break;
	case  GLFW_KEY_BACKSPACE:	backSpace();
					break;
	case GLFW_KEY_ENTER:	dataEnteredAlert();
		break;
	};
}

/** For now, lazily assume one-line text. */
void CGUItextbox2::moveCursor(int x, int y) {
	if (x < 0 && cursorTextPos > 0)
		cursorTextPos--;
	if (x > 0 && cursorTextPos < text.size())
		cursorTextPos++;

	calcCursorPosition();
}

/** Delete the character at the cursor positionHint. */
void CGUItextbox2::backSpace() {
	if (cursorTextPos > 0) {
		text.erase(cursorTextPos - 1, 1);
		cursorTextPos--;
		renderText();
	}
}

/** Insert the given string at the cursor positionHint. */
void CGUItextbox2::insert(std::string inText) {
	text.insert(cursorTextPos, inText);
	cursorTextPos++;
	renderText();
}

/** Alert the user that Enter has been pressed or keycapture lost, making the current text 'entered'. */
void CGUItextbox2::dataEnteredAlert() {
	CMessage msg;
	msg.Msg = uiDataEntered;
	parent->message(this, msg);
}

/** User has pressed a character key, and we have focus. */
void CGUItextbox2::OnCharEntry(unsigned int Key, long Mod) {
	if (iscntrl(Key))
		return;
	std::string inString(1,char(Key));
	insert(inString);
}

void CGUItextbox2::message(CGUIbase* sender, CMessage& msg) {
	if (msg.Msg == uiLostKeyboard)
		dataEnteredAlert();
}

/** Render the text according to current settings. */
void CGUItextbox2::renderText() {
	textBuf.clearBuffer();
	calcLineOffset();
	TLineFragDrawRec dataRec = { &text,font,textColour };
	textBuf.renderTextAt(renderOffset.x, renderOffset.y, dataRec);
	calcCursorPosition();
}

std::string & CGUItextbox2::getText() {
	return text;
}

void CGUItextbox2::updateAppearance() {
	CGUIbase::updateAppearance();
	textBuf.setSize(getWidth(), getHeight());
	renderText();
}