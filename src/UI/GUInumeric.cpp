#include "GUInumeric.h" 
#include <sstream> 

CGUInumeric::CGUInumeric(int x , int y, int w, int h){
	if (!h)
		h=defaultNumericHeight;
	SetPos(x,y,w,h);

	//create the two buttons.
	downButton = new CGUInumericIconButton(0,0);
	downButton->icon = leftIconArrow;
	downButton->id = downButtonID;
	upButton = new CGUInumericIconButton(0,0);
	upButton->hFormat = hRight;
	upButton->icon = rightIconArrow;
	upButton->id = upButtonID;
	Add(downButton);
	Add(upButton);

	//create numBox
	numBox = new CGUInumericTextbox(downButton->width,0, 
		width - (downButton->width +upButton->width),defaultNumericHeight);
	numBox->TextAlign = tcentred;
	Add(numBox);

	valMin = 0; valMax = 10;
	setValue(0);
	increment = 1;
}

/** Directly sets the numeric value of the control. */
void CGUInumeric::setValue(float newValue) {
	value = newValue;
	string numStr; //now update the text:
	ostringstream convert;  
	convert << value;
	numStr = convert.str(); 
	numStr += suffix;
	numBox->SetText(numStr);
}

/** Handle any internal messages telling us the control has been spun or otherwise updated.*/
void  CGUInumeric::message(CGUIbase& sender,CMessage& msg) {
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
	pDrawFuncs->handleUImsg(*this,userMsg);
}

/** Overloads the default handler to scroll the numeric control. */
bool CGUInumeric::MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key) {
	//first, see if any child controls can handle this mousewheel
	if (CGUIbase::MouseWheelMsg(mouseX,mouseY,wheelDelta, key))
		return true;
	
	//float direction = (wheelDelta > 0)? 1 : -1;
	int direction = (wheelDelta > 0)? 1 : -1;
	CMessage msg;
	msg.Msg = uiSpin;
	msg.value = direction;
	message(*this,msg);	
	return true;
}


void CGUInumeric::setEnabled(bool onOff) {
	enabled = onOff;
	numBox->setEnabled(onOff);
	upButton->setEnabled(onOff);
	downButton->setEnabled(onOff);
}

void CGUInumeric::setMinMax(float newMin, float mewMax) {
	valMin = newMin; valMax = mewMax;
}

void CGUInumeric::setSuffix(const string& newSuffix) {
	suffix = newSuffix;
}



CGUInumericTextbox::CGUInumericTextbox(int x, int y, int w, int h) {
	SetPos(x,y,w,h);
}


void CGUInumericTextbox::OnCharEntry(unsigned int Key, long Mod) {
	if ((!isdigit(Key)) && (Key != '.'))
		return;
	CGUItextbox::OnCharEntry(Key,Mod);
	((CGUInumeric*)parent)->value = (float)atof(Text.c_str());
}

void CGUInumericTextbox::OnKeyPress(unsigned int Key, long Mod) {
	CGUItextbox::OnKeyPress(Key,Mod);
	((CGUInumeric*)parent)->value = (float)atof(Text.c_str());
}

/** Alert the parent device that Enter has been pressed or keycapture lost, making the current text 'entered'. */
void CGUInumericTextbox::dataEnteredAlert() {
	CMessage msg;
	msg.Msg = uiDataEntered;
	parent->message(*this,msg);
}




/** Overload the usual onclick message, to alert the numeric control instead. */
void CGUInumericIconButton::OnClick(const  int mouseX, const  int mouseY) {
	//float direction = (id == downButtonID)? -1 : 1;
	int direction = (id == downButtonID)? -1 : 1;
	CMessage msg;
	msg.value = direction;
	msg.Msg = uiSpin;
	parent->message(*this,msg);
}



