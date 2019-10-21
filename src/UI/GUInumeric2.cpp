#include "GUInumeric2.h"

#include "glm\glm.hpp"
#include <sstream> 


using namespace glm;

CGUInumeric2::CGUInumeric2(int x, int y, int w, int h) {
	if (!h)
		h = defaultNumericHeight;

	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);

	//create the two buttons.
	downButton = new CGUInumericIconButton(0, 0,h,h);
	downButton->icon = leftIconArrow;
	downButton->id = downButtonID;
	upButton = new CGUInumericIconButton(0, 0,h,h);
	upButton->hFormat = hRight;
	upButton->icon = rightIconArrow;
	upButton->id = upButtonID;
	Add(downButton);
	Add(upButton);

	//create numBox
	numBox = new CGUInumericTextbox2(h, 0,
		getWidth() - (h * 2), h);
	////////////////numBox->TextAlign = tcentred;
	
	Add(numBox);

	valMin = 0; valMax = 10;

	increment = 1;
	type = uiNumeric;

	setValue(0);

	//drawBorder = false;
}

/** Directly sets the numeric value of the control. */
void CGUInumeric2::setValue(float newValue) {
	value = newValue;
	std::string numStr; //now update the text:
	std::ostringstream convert;
	convert << value;
	numStr = convert.str();
	numStr += suffix;
	numBox->setText(numStr);
}

void CGUInumeric2::setValue(int newValue) {
	setValue((float)newValue);
}

/** Handle any internal messages telling us the control has been spun or otherwise updated.*/
void  CGUInumeric2::message(CGUIbase* sender, CMessage& msg) {
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

	//CMessage userMsg;
	msg.fValue = value;
	parent->message(this, msg);
}

/** Overloads the default handler to scroll the numeric control. */
bool CGUInumeric2::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	//first, see if any child controls can handle this mousewheel
	if (CGUIbase::MouseWheelMsg(mouseX, mouseY, wheelDelta, key))
		return true;

	int direction = (wheelDelta > 0) ? 1 : -1;
	CMessage msg;
	msg.Msg = uiSpin;
	msg.value = direction;
	message(this, msg);
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

void CGUInumeric2::setSuffix(const std::string& newSuffix) {
	suffix = newSuffix;
}

void CGUInumeric2::setIncrement(float increment) {
	this->increment = increment;
}



CGUInumericTextbox2::CGUInumericTextbox2(int x, int y, int w, int h) 
	: CGUItextbox2(x,y,w,h) {
}

void CGUInumericTextbox2::OnCharEntry(unsigned int Key, long Mod) {
	if ((!isdigit(Key)) && (Key != '.'))
		return;
	CGUItextbox2::OnCharEntry(Key, Mod);
	std::string str = text;
	((CGUInumeric2*)parent)->value = (float)atof(str.c_str());
}

void CGUInumericTextbox2::onKeyPress(unsigned int Key, long Mod) {
	CGUItextbox2::onKeyPress(Key, Mod);
	std::string str = text;
	((CGUInumeric2*)parent)->value = (float)atof(str.c_str());
}


/** Overload the usual onclick message, to alert the numeric control instead. */
void CGUInumericIconButton::OnClick(const  int mouseX, const  int mouseY) {
	int direction = (id == downButtonID) ? -1 : 1;
	CMessage msg;
	msg.value = direction;
	msg.Msg = uiSpin;
	parent->message(this, msg);
}

void CGUInumericIconButton::onDoubleClick(int mouseX, int mouseY) {
	OnClick(mouseX, mouseY);
}