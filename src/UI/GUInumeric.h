#pragma once
#include "GUIbase.h"
#include "GUIiconButton.h"
#include "GUItextbox.h"

/** Control for adjusting a numeric value. */
class CGUIiconButton;
class CGUIsysTextbox;
class CGUInumericIconButton;
class CGUInumericTextbox;
class CGUInumeric : public CGUIbase {
public:
	CGUInumeric(int x , int y, int w, int h);
	void setValue(float newValue);
	void message(CGUIbase& sender, CMessage& msg);
	void setMinMax(float newMin, float mewMax);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void setEnabled(bool onOff);
	void setSuffix(const string& newSuffix);
	

	CGUInumericIconButton* downButton; ///<Points to decrement button.
	CGUInumericIconButton* upButton; ///<Points to increment button.
	CGUInumericTextbox* numBox; ///<Box for displaying number.

	float value; ///<The numeric value this control adjusts.
	float increment; ///<Amount by which value is increased or decreased.
	float valMin;	///<Minimum allowed value.
	float valMax; ///<Maximum allowed value.
	string suffix;
};


const int defaultNumericWidth  = 60;
const int defaultNumericHeight  = 16;

const int upButtonID = 10;
const int downButtonID = 20;

class CGUInumericIconButton : public CGUIsysIconButton {
public:
	CGUInumericIconButton(int x,int y) : CGUIsysIconButton(x,y) {};
	void OnClick(const  int mouseX, const  int mouseY);

};

class CGUInumericTextbox : public CGUItextbox {
public:
	CGUInumericTextbox() {};
	CGUInumericTextbox(int x, int y, int w, int h);
	void OnCharEntry(unsigned int Key, long Mod);
	void OnKeyPress(unsigned int Key, long Mod);
	void dataEnteredAlert();
};
