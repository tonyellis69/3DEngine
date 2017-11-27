#include "GUItextbox.h"
#include <sstream>
#include <ctype.h>

CGUIsysTextbox::CGUIsysTextbox(void) {
	type = textbox;
	CursorPos = Text.length();
}

void CGUIsysTextbox::DrawSelf( ) {
	pDrawFuncs->setClip(Clipbox);

	//draw box
	pDrawFuncs->setDrawColoursConditional(UIwhite,UIwhite,enabled);
	pDrawFuncs->drawRect(screenPos,width,height);

	//Set the font
//////////////	pDrawFuncs->setFont(TextFont);
	//Set the colour
	pDrawFuncs->setDrawColoursConditional(txtColour, txtColour,enabled);
	//draw text

	int w = 0;
	if (TextAlign == tcentred)
		w = width - (textIndent*2);
	if (TextAlign == tright)
		w = - (width - (textIndent*2));

	if (KeyCapture == this) {
		pDrawFuncs->setCursor(CursorPos);
		/////////////////////////		pDrawFuncs->drawText(screenPos.x + textIndent,screenPos.y + (height/2),w,(char*)Text.c_str());;

		pDrawFuncs->setCursor(NONE);
	}
	else
		;///////////////////////////		pDrawFuncs->drawText(screenPos.x + textIndent, screenPos.y + (height / 2), w, (char*)Text.c_str());

	if ((MouseOver == this)||(KeyCapture == this)) {
		pDrawFuncs->setDrawColours(UIdarkGrey, UIdarkGrey);
	} else {
		pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
	}

	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);


}

/**User has clicked on the textbox, so give it focus, allowing it to be edited. */
void CGUIsysTextbox::OnClick(const  int mouseX, const  int mouseY) {
	captureKeyboard();
	CursorPos = Text.length();
}

/**User has pressed a key, and we have focus. Use this to catch cursor keys, backspace,etc*/
void CGUIsysTextbox::OnKeyPress(unsigned int Key, long Mod) {


	switch (Key) {
	case GLFW_KEY_LEFT :	if (CursorPos > 0) 
						CursorPos--; 
		break;
	case GLFW_KEY_RIGHT :	if (CursorPos < (int)Text.length())
						CursorPos++; 
		break;
	case  GLFW_KEY_BACKSPACE   :	if (CursorPos > 0) {
							Text.erase(CursorPos-1,1); 
							CursorPos--;
						}
						break;
	case GLFW_KEY_ENTER:	dataEnteredAlert();
						break;
	};
}

/** Alert the user that Enter has been pressed or keycapture lost, making the current text 'entered'. */
void CGUIsysTextbox::dataEnteredAlert() {
	CMessage msg;
	msg.Msg = uiDataEntered;
	pDrawFuncs->handleUImsg(*this,msg);
}

/** User has pressed a character key, and we have focus. */
void CGUIsysTextbox::OnCharEntry(unsigned int Key, long Mod) {
	if (iscntrl(Key))
		return;
	Text.insert(Text.begin()+CursorPos,Key);
	CursorPos++;
}
void CGUIsysTextbox::message(CGUIbase& sender, CMessage& msg) {
	if (msg.Msg == uiLostKeyboard)
		dataEnteredAlert();
}


int CGUItextbox::Count = 1;
CGUItextbox::CGUItextbox(int x, int y, int w, int h) {
	SetPos(x,y,w,h);
	GenName("Textbox",Count++);
	SetText(Name);

}