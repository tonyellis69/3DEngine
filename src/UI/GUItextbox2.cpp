#include "GUItextbox2.h"

using namespace glm;

CGUItextbox2::CGUItextbox2(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	setText("textbox");
	textBuf.setFont(defaultFont);
	textBuf.setSize(w, h);

	type = uiTextbox;
	setTextColour(0, 0, 0, 1);

	//drawBorder = false;

	pDrawFuncs->registerControl(*this);

}

void CGUItextbox2::setFont(CFont& font) {
	textBuf.setFont(font);
	textBuf.renderText();
}

void CGUItextbox2::setText( std::string newText) {
	textBuf.setText(newText);
}

void CGUItextbox2::setTextColour(float r, float g, float b, float a) {
	textColour.r = r; textColour.g = g; textColour.b = b; textColour.a = a;
	textBuf.setTextColour(textColour);
	textBuf.renderText();
}

void CGUItextbox2::setTextColour(UIcolour  colour) {
	setTextColour(colour.r, colour.g, colour.b, colour.a);
}

void CGUItextbox2::setHorizontalAlignment(TTextAlign align) {
	textBuf.setHorizontalAlignment(align);
	textBuf.renderText();
}

void CGUItextbox2::DrawSelf() {
	pDrawFuncs->setClip(Clipbox);

	//draw box
	setBackColour1(UIwhite);
	setBackColour2(UIwhite);
	pDrawFuncs->drawCtrlRect(*this);

	if (KeyCapture == this) {
		//draw cursor
		pDrawFuncs->drawCursor(*this, *textBuf.getCursorPos());
	}

	pDrawFuncs->drawTexture(*this, textBuf.textTexture);

	//draw border
	if (MouseOver == this || KeyCapture == this) {
		setBorderColour(UIdarkGrey);
	}
	else {
		setBorderColour(UIlightGrey);
	}
	pDrawFuncs->drawCtrlBorder(*this);
	

}

/**User has clicked on the textbox, so give it focus, allowing it to be edited. */
void CGUItextbox2::OnClick(const  int mouseX, const  int mouseY) {
	captureKeyboard();
}

/**User has pressed a key, and we have focus. Use this to catch cursor keys, backspace,etc*/
void CGUItextbox2::OnKeyPress(unsigned int Key, long Mod) {
	switch (Key) {
	case GLFW_KEY_LEFT:	textBuf.moveCursor(-1, 0);
					break;
	case GLFW_KEY_RIGHT:	textBuf.moveCursor(1, 0);
					break;
	case  GLFW_KEY_BACKSPACE:	textBuf.backSpace();
					break;
	case GLFW_KEY_ENTER:	dataEnteredAlert();
		break;
	};
}

/** Alert the user that Enter has been pressed or keycapture lost, making the current text 'entered'. */
void CGUItextbox2::dataEnteredAlert() {
	CMessage msg;
	msg.Msg = uiDataEntered;
	pDrawFuncs->handleUImsg(*this, msg);
}

/** User has pressed a character key, and we have focus. */
void CGUItextbox2::OnCharEntry(unsigned int Key, long Mod) {
	if (iscntrl(Key))
		return;
	std::string inString(1,char(Key));
	textBuf.insert(inString);
}

void CGUItextbox2::message(CGUIbase& sender, CMessage& msg) {
	if (msg.Msg == uiLostKeyboard)
		dataEnteredAlert();
}