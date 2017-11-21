#include "GUInumeric2.h"

#include "glm\glm.hpp"
#include <sstream> 


using namespace glm;

CGUInumeric2::CGUInumeric2(int x, int y, int w, int h) {
	if (!h)
		h = defaultNumericHeight;

	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	pDrawFuncs->registerControl(*this);
	//SetPos(x, y, w, h);
	

	//create the two buttons.
	downButton = new CGUInumericIconButton(0, 0);
	downButton->icon = leftIconArrow;
	downButton->id = downButtonID;
	upButton = new CGUInumericIconButton(0, 0);
	upButton->hFormat = hRight;
	upButton->icon = rightIconArrow;
	upButton->id = upButtonID;
	Add(downButton);
	Add(upButton);

	//create numBox
	numBox = new CGUInumericTextbox2(downButton->width, 0,
		width - (downButton->width + upButton->width), defaultNumericHeight);
	////////////////numBox->TextAlign = tcentred;
	Add(numBox);

	valMin = 0; valMax = 10;

	increment = 1;
	type = uiNumeric;

	//numBox->textBuf.setFont(defaultFont);
	//numBox->textBuf.setSize(w, h);
	//numBox->setTextColour(0, 0, 0, 1);

	setValue(0);

	//drawBorder = false;


}

/** Directly sets the numeric value of the control. */
void CGUInumeric2::setValue(float newValue) {
	value = newValue;
	string numStr; //now update the text:
	ostringstream convert;
	convert << value;
	numStr = convert.str();
	numStr += suffix;
	numBox->setText(numStr);
}


/** Handle any internal messages telling us the control has been spun or otherwise updated.*/
void  CGUInumeric2::message(CGUIbase& sender, CMessage& msg) {
	float tmpValue = value;
	if (msg.Msg == uiSpin) {
		if (msg.value == -1)
			tmpValue -= increment;
		else
			tmpValue += increment;
		setValue(tmpValue);
	}

	if (value < valMin)
		value = valMin;
	if (value > valMax)
		value = valMax;
	setValue(value); //lazy way to add suffix

	CMessage userMsg;
	userMsg.fValue = value;
	pDrawFuncs->handleUImsg(*this, userMsg);
}

/** Overloads the default handler to scroll the numeric control. */
bool CGUInumeric2::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	//first, see if any child controls can handle this mousewheel
	if (CGUIbase::MouseWheelMsg(mouseX, mouseY, wheelDelta, key))
		return true;

	//float direction = (wheelDelta > 0)? 1 : -1;
	int direction = (wheelDelta > 0) ? 1 : -1;
	CMessage msg;
	msg.Msg = uiSpin;
	msg.value = direction;
	message(*this, msg);
	return true;
}

void CGUInumeric2::setEnabled(bool onOff) {
	enabled = onOff;
	numBox->setEnabled(onOff);
	upButton->setEnabled(onOff);
	downButton->setEnabled(onOff);
}

void CGUInumeric2::setMinMax(float newMin, float mewMax) {
	valMin = newMin; valMax = mewMax;
}

void CGUInumeric2::setSuffix(const string& newSuffix) {
	suffix = newSuffix;
}



CGUInumericTextbox2::CGUInumericTextbox2(int x, int y, int w, int h) 
	: CGUItextbox2(x,y,w,h) {
	SetPos(x, y, w, h);
}

void CGUInumericTextbox2::OnCharEntry(unsigned int Key, long Mod) {
	if ((!isdigit(Key)) && (Key != '.'))
		return;
	CGUItextbox2::OnCharEntry(Key, Mod);
	std::string str = textBuf.getText();
	((CGUInumeric2*)parent)->value = (float)atof(str.c_str());
}

void CGUInumericTextbox2::OnKeyPress(unsigned int Key, long Mod) {
	CGUItextbox2::OnKeyPress(Key, Mod);
	std::string str = textBuf.getText();
	((CGUInumeric2*)parent)->value = (float)atof(str.c_str());
}

/** Alert the parent device that Enter has been pressed or keycapture lost, making the current text 'entered'. */
void CGUInumericTextbox2::dataEnteredAlert() {
	CMessage msg;
	msg.Msg = uiDataEntered;
	parent->message(*this, msg);
}

/** Overload the usual onclick message, to alert the numeric control instead. */
void CGUInumericIconButton::OnClick(const  int mouseX, const  int mouseY) {
	//float direction = (id == downButtonID)? -1 : 1;
	int direction = (id == downButtonID) ? -1 : 1;
	CMessage msg;
	msg.value = direction;
	msg.Msg = uiSpin;
	parent->message(*this, msg);
}