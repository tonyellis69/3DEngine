#include "GUIscrollbar.h"
#include <iostream> //for cerr

#include "UI/uiRender.h"
#include "UI/GUIroot.h"

using namespace std;

CGUIbaseScrollbar::CGUIbaseScrollbar(ScrollbarOrientationType axis, int x, int y, int length) {
	orientation = axis; 
	type = scrollbar;
	setLocalPos(x, y);
	if (orientation == horizontal) {
		setWidth(length); setHeight(barWidth); }
	else {
		setHeight(length); setWidth(barWidth); }

	SliderSize = 30; //some defaults
	Max = 100; Min = 1; Value = 1;
	clickIncrement = 10;

	lastMouseDownX = lastMouseDownY = 0;
	id = uiScrollbarID;
;
	setBackColour1(UIlightGrey);
	setBackColour2(UIlightGrey);
}

/** Make the slider size a proportion of the scrollbar's total length. */
void CGUIbaseScrollbar::setSliderSize(float ratio) {
	if (orientation == horizontal) 
		SliderSize = (int)(getWidth() * ratio);
	else 
		SliderSize = (int)(getHeight() * ratio);
	if (SliderSize < 30)
		SliderSize = 30;
}

/** If the dimensions of the bar have changed, so has the slider. */
void CGUIbaseScrollbar::updateAppearance() {
	CGUIbase::updateAppearance();
	updateSliderAppearance();
}

void CGUIbaseScrollbar::updateSliderAppearance() {
	Travel = (orientation == horizontal) ? getWidth() : getHeight();
	Travel -= SliderSize;
	float Ratio = (float) (Value - Min) / (Max - Min) ;
	SliderPos = (int)(Travel * Ratio);
}

void CGUIbaseScrollbar::setValue(int newValue) {
	Value = newValue;
	updateSliderAppearance();
}


void CGUIbaseScrollbar::DrawSelf( ) {
	uiDraw::drawRect(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);

	glm::vec4 drawColour;
	if ((MouseOver == this)||(KeyCapture == this)) {
		drawColour = (glm::vec4&)UIblack;
		
	} else {
		drawColour = (glm::vec4&)UIdarkGrey;
	}
	
	guiRect sliderBox;
	//UIcoord sliderScrnPos = screenPos;
		if (orientation == horizontal) {
			sliderBox.pos.x = drawBox.pos.x + SliderPos + 2;
			sliderBox.pos.y = drawBox.pos.y +  2;
			sliderBox.size.x = SliderSize - 4;
			sliderBox.size.y = getHeight() - 4;

		}
		else {
			sliderBox.pos.x = drawBox.pos.x + 2;
			sliderBox.pos.y = drawBox.pos.y + (getHeight() - SliderPos - SliderSize) + 2;
			sliderBox.size.x = getWidth() - 4;
			sliderBox.size.y = SliderSize - 4;
		}
		uiDraw::drawRect(sliderBox, drawColour, drawColour);
}


/** Called when user left-clicks *down* on a scrollbar. */
bool CGUIbaseScrollbar::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	MouseDown = this;
	int mousePos;

	if (orientation == horizontal) 
		mousePos = screenToLocalCoords(mouseX, mouseY).x; 
	else
		mousePos = getHeight() - screenToLocalCoords(mouseX, mouseY).y;

	if (mousePos < SliderPos) {
		SliderPos -= clickIncrement; 
		updateValue();
		return true;
	}
	if (mousePos > (SliderPos + SliderSize)) {
		SliderPos += clickIncrement;
		updateValue(); 
		return true;
	}
	mouseSliderOffset = mousePos - SliderPos;
	scrollbarHasMouse = this;
	rootUI->captureMouse(true);
	return true;
}


bool CGUIbaseScrollbar::OnMouseMove(int mouseX, int mouseY,int key) {
	if ((MouseDown == this) && ( scrollbarHasMouse  == this)) {//scrollbar dragging appears to be going on
		if (orientation == horizontal) 
			SliderPos = screenToLocalCoords(mouseX, mouseY).x - mouseSliderOffset;
		else {
			SliderPos = (getHeight() - screenToLocalCoords(mouseX, mouseY).y) - mouseSliderOffset;
		}
		updateValue();
	}
	return true;
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
	//pDrawFuncs->handleUImsg(*this,msg);

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
	msg.Msg = uiMsgSlide;
	msg.value = Value;
	parent->message(this,msg);
}

