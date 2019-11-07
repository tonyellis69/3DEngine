#pragma once

#include "GUItextbox2.h"
#include "GUIiconButton.h"


/** New class for a numeric control. */
class CGUIiconButton;
class CGUItextbox2;
class CGUInumericIconButton;
class CGUInumericTextbox2;
class CGUInumeric2 : public CGUIbase {
public:
	CGUInumeric2(int x, int y, int w, int h);
	void setValue(float newValue);
	void setValue(int newValue);
	void message(CGUIbase* sender, CMessage& msg);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void setEnabled(bool onOff);
	void setMinMax(float newMin, float mewMax);
	void setSuffix(const std::string& newSuffix);
	void setIncrement(float increment);


	CGUInumericIconButton* downButton; ///<Points to decrement button.
	CGUInumericIconButton* upButton; ///<Points to increment button.
	CGUInumericTextbox2* numBox; ///<Box for displaying number.

	float value; ///<The numeric value this control adjusts.
	float increment; ///<Amount by which value is increased or decreased.
	float valMin;	///<Minimum allowed value.
	float valMax; ///<Maximum allowed value.
	std::string suffix;
};

const int defaultNumericWidth = 60;
const int defaultNumericHeight = 16;

const int upButtonID = 10;
const int downButtonID = 20;

class CGUInumericIconButton : public CGUIsysIconButton {
public:
	CGUInumericIconButton(int x, int y) : CGUIsysIconButton(x, y) {};
	CGUInumericIconButton(int x, int y, int w, int h) : CGUIsysIconButton(x, y,w,h) {};
	bool OnClick(const  int mouseX, const  int mouseY);
	void onDoubleClick(int mouseX, int mouseY);

};

class CGUInumericTextbox2 : public CGUItextbox2 {
public:
	//CGUInumericTextbox() {};
	CGUInumericTextbox2(int x, int y, int w, int h);
	void OnCharEntry(unsigned int Key, long Mod);
	void onKeyPress(unsigned int Key, long Mod);
	//void dataEnteredAlert();
};