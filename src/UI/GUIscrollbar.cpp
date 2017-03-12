#include "GUIscrollbar.h"
#include <iostream> //for cerr

using namespace std;

CGUIbaseScrollbar::CGUIbaseScrollbar(ScrollbarOrientationType axis, int x, int y, int length) {
	orientation = axis; 
	type = scrollbar;
	xPos = x; yPos = y;
	if (orientation == horizontal) {
		width = length; height = barWidth; }
	else {
		height = length; width = barWidth; }

	SliderSize = 30; //some defaults
	Max = 100; Min = 1; Value = 1;
	clickIncrement = 10;

	lastMouseDownX = lastMouseDownY = 0;
	id = uiScrollbarID;
}

/** Make the slider size a proportion of the scrollbar's total length. */
void CGUIbaseScrollbar::setSliderSize(float ratio) {
	if (orientation == horizontal) 
		SliderSize = (int)(width * ratio);
	else 
		SliderSize = (int)(height * ratio);
	if (SliderSize < 30)
		SliderSize = 30;
}


void CGUIbaseScrollbar::updateAppearance() {
	CGUIbase::updateAppearance();
	updateSliderAppearance();
}

void CGUIbaseScrollbar::updateSliderAppearance() {
	Travel = (orientation == horizontal) ? width : height;
	Travel -= SliderSize;
	float Ratio = (float) (Value - Min) / (Max - Min) ;
	SliderPos = (int)(Travel * Ratio);
}


void CGUIbaseScrollbar::DrawSelf( ) {
	pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
	pDrawFuncs->drawRect(screenPos,width,height);

	if ((MouseOver == this)||(KeyCapture == this)) {
		pDrawFuncs->setDrawColours(UIblack, UIblack);
	} else {
		pDrawFuncs->setDrawColours(UIdarkGrey, UIdarkGrey);
	}

	UIcoord sliderScrnPos = screenPos;
		if (orientation == horizontal) {
			sliderScrnPos.x += SliderPos+2; sliderScrnPos.y += 2;
			pDrawFuncs->drawRect(sliderScrnPos,SliderSize-4,height-4);
		}
		else {
			sliderScrnPos.x += 2; sliderScrnPos.y += (height - SliderPos - SliderSize) +2;
			pDrawFuncs->drawRect(sliderScrnPos,width-4,SliderSize-4);
		}
}


/** Called when user left-clicks *down* on a scrollbar. */
void CGUIbaseScrollbar::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	MouseDown = this;
	int mousePos;

	if (orientation == horizontal) 
		mousePos = mouseX;
	else
		mousePos = height - mouseY;

	if (mousePos < SliderPos) {
		SliderPos -= clickIncrement; 
		updateValue();
		return;
	}
	if (mousePos > (SliderPos + SliderSize)) {
		SliderPos += clickIncrement;
		updateValue(); 
		return;
	}
	mouseSliderOffset = mousePos - SliderPos;
	scrollbarHasMouse = this;
	pDrawFuncs->mouseCaptured(true);
}


void CGUIbaseScrollbar::OnMouseMove(int mouseX, int mouseY,int key) {
	if ((MouseDown == this) && ( scrollbarHasMouse  == this)) {//scrollbar dragging appears to be going on
		if (orientation == horizontal) 
			SliderPos = mouseX - mouseSliderOffset;
		else
			SliderPos = (height - mouseY) - mouseSliderOffset;
		updateValue();
	}
}

void CGUIbaseScrollbar::updateValue() {
	if (SliderPos < 0) 
		SliderPos = 0;
	if ( (SliderPos ) > Travel)
		SliderPos = Travel;

	float ratio = (float)SliderPos / Travel;

	Value =(int) ((Max - Min) * ratio) + Min;

	//call the user's event handler.
	CMessage msg;
	msg.Msg = change;
	pDrawFuncs->handleUImsg(*this,msg);

}

/** Set the maximum value of the slider.*/
void CGUIbaseScrollbar::setMax(int newMax) {
	Max = newMax;
}


/* Set the minimum value of the slider.*/
void CGUIbaseScrollbar::setMin(int newMin) {
	Min = newMin;
}

int CGUIscrollbar::Count = 1;
CGUIscrollbar::CGUIscrollbar(ScrollbarOrientationType axis, int x, int y, int length) : CGUIbaseScrollbar(axis,x,y,length){
	GenName("Scrollbar",Count++);
}

/** Overloaded version of updateValue that notifies the parent that Value has changed. */
void CGUIsysScrollbar::updateValue() {
	if (SliderPos < 0) 
		SliderPos = 0;
	if ( (SliderPos ) > Travel)
		SliderPos = Travel;

	float ratio = (float)SliderPos / Travel;

	Value =(int) ((Max - Min) * ratio) + Min;

	CMessage msg;
	msg.value = Value;
	parent->message(*this,msg);
}

