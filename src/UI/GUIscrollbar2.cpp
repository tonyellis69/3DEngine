#include "GUIscrollbar2.h"
#include <iostream> //for cerr

using namespace std;

CGUIbaseScrollbar2::CGUIbaseScrollbar2(ScrollbarOrientationType2 axis, int x, int y, int length) {
	orientation = axis; 
	type = scrollbar;
	setLocalPos(x, y);
	if (orientation == scrlHorizontal) {
		setWidth(length); setHeight(barWidth2); }
	else {
		setHeight(length); setWidth(barWidth2); }

	SliderSize = 30; //some defaults
	Max = 100; Min = 1; Value = 1;
	clickIncrement = 10;

	lastMouseDownX = lastMouseDownY = 0;
	id = uiScrollbarID;
	applyStyleSheet();

}

/** Construct default scrollbar. */
CGUIbaseScrollbar2::CGUIbaseScrollbar2(ScrollbarOrientationType2 axis) {
	orientation = axis;
	type = scrollbar;
	//setLocalPos(x, y);
	if (orientation == scrlHorizontal) {
		setWidth(styleSheet->scrollbarLength); setHeight(styleSheet->scrollbarWidth);
	}
	else {
		setHeight(styleSheet->scrollbarLength); setWidth(styleSheet->scrollbarWidth);
	}

	SliderSize = styleSheet->scrollbarSliderSize;
	Max = 100; Min = 1; Value = 1;
	clickIncrement = 10;

	lastMouseDownX = lastMouseDownY = 0;
	id = uiScrollbarID;
	
	setBackColour1(styleSheet->defaultScrollbarBackColour);
	setBackColour2(styleSheet->defaultScrollbarBackColour);
}

void CGUIbaseScrollbar2::applyStyleSheet() {
	setBackColour1(UIlightGrey);
	setBackColour2(UIlightGrey);
}

/** Make the slider size a proportion of the scrollbar's total length. */
void CGUIbaseScrollbar2::setSliderSize(float ratio) {
	if (orientation == scrlHorizontal) 
		SliderSize = (int)(getWidth() * ratio);
	else 
		SliderSize = (int)(getHeight() * ratio);
	if (SliderSize < 30)
		SliderSize = 30;
}

/** If the dimensions of the bar have changed, so has the slider. */
void CGUIbaseScrollbar2::updateAppearance() {
	CGUIbase::updateAppearance();
	updateSliderAppearance();
}

void CGUIbaseScrollbar2::updateSliderAppearance() {
	Travel = (orientation == scrlHorizontal) ? getWidth() : getHeight();
	Travel -= SliderSize;
	float Ratio = (float) (Value - Min) / (Max - Min) ;
	SliderPos = (int)(Travel * Ratio);
}

void CGUIbaseScrollbar2::setValue(int newValue) {
	Value = newValue;
	updateSliderAppearance();
}


void CGUIbaseScrollbar2::DrawSelf( ) {
	pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);

	glm::vec4 drawColour;
	if ((MouseOver == this)||(KeyCapture == this)) {
		drawColour = (glm::vec4&)UIblack;
		
	} else {
		drawColour = (glm::vec4&)UIdarkGrey;
	}
	
	guiRect sliderBox;
	//UIcoord sliderScrnPos = screenPos;
		if (orientation == scrlHorizontal) {
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
		pDrawFuncs->drawRect2(sliderBox, drawColour, drawColour);
}


/** Called when user left-clicks *down* on a scrollbar. */
void CGUIbaseScrollbar2::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	MouseDown = this;
	int mousePos;

	if (orientation == scrlHorizontal) 
		mousePos = screenToLocalCoords(mouseX, mouseY).x; 
	else
		mousePos = getHeight() - screenToLocalCoords(mouseX, mouseY).y;

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


void CGUIbaseScrollbar2::OnMouseMove(int mouseX, int mouseY,int key) {
	if ((MouseDown == this) && ( scrollbarHasMouse  == this)) {//scrollbar dragging appears to be going on
		if (orientation == scrlHorizontal) 
			SliderPos = screenToLocalCoords(mouseX, mouseY).x - mouseSliderOffset;
		else {
			SliderPos = (getHeight() - screenToLocalCoords(mouseX, mouseY).y) - mouseSliderOffset;
		}
		updateValue();
	}
}

void CGUIbaseScrollbar2::updateValue() {
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
void CGUIbaseScrollbar2::setMax(int newMax) {
	Max = newMax;
}


/* Set the minimum value of the slider.*/
void CGUIbaseScrollbar2::setMin(int newMin) {
	Min = newMin;
}

int CGUIscrollbar2::Count = 1;
CGUIscrollbar2::CGUIscrollbar2(ScrollbarOrientationType2 axis, int x, int y, int length) : CGUIbaseScrollbar2(axis,x,y,length){
	GenName("Scrollbar",Count++);
}

/** Create default scrollbar. */
CGUIsysScrollbar2::CGUIsysScrollbar2(ScrollbarOrientationType2 axis, unsigned int styleWord) : CGUIbaseScrollbar2( axis) {
	positionHint.expansive = true;
	positionHint.vAlignment = uiVcentred;
	setPositionStyles(styleWord);	
}

/** Overloaded version of updateValue that notifies the parent that Value has changed. */
void CGUIsysScrollbar2::updateValue() {
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

