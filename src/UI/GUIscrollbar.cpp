#include "GUIscrollbar.h"
#include <iostream> //for cerr

using namespace std;

CGUIbaseScrollbar::CGUIbaseScrollbar(ScrollbarOrientationType axis, int x, int y, int length) {
	orientation = axis; 
	type = scrollbar;
	//localPos = glm::i32vec2(x,y);
	setPos(x, y);
	if (orientation == horizontal) {
		setWidth(length); setHeight(barWidth); }
	else {
		setHeight(length); setWidth(barWidth); }

	SliderSize = 30; //some defaults
	Max = 100; Min = 1; Value = 1;
	clickIncrement = 10;

	lastMouseDownX = lastMouseDownY = 0;
	id = uiScrollbarID;

	//drawBox.pos = glm::i32vec2(x, y); 
	//drawBox.size = glm::i32vec2(width, height);
	//pDrawFuncs->registerControl(*this);
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
	//pDrawFuncs->setClip(Clipbox);

	//pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
	//pDrawFuncs->drawRect(screenPos,width,height);


	pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);

	glm::vec4 drawColour;
	if ((MouseOver == this)||(KeyCapture == this)) {
		drawColour = (glm::vec4&)UIblack;
		
	} else {
		drawColour = (glm::vec4&)UIdarkGrey;
	}
	
	guiRect sliderBox;
	//UIcoord sliderScrnPos = screenPos;
		if (orientation == horizontal) {
			//sliderScrnPos.x += SliderPos+2; sliderScrnPos.y += 2;
			sliderBox.pos.x = drawBox.pos.x + SliderPos + 2;
			sliderBox.pos.y = drawBox.pos.y +  2;
			sliderBox.size.x = SliderSize - 4;
			sliderBox.size.y = getHeight() - 4;
			//sliderBox = {sliderScrnPos,SliderSize-4,height-4);
		}
		else {
			//sliderScrnPos.x += 2; sliderScrnPos.y += (height - SliderPos - SliderSize) +2;
			sliderBox.pos.x = drawBox.pos.x + 2;
			sliderBox.pos.y = drawBox.pos.y + (getHeight() - SliderPos - SliderSize) + 2;
			//pDrawFuncs->drawRect(sliderScrnPos,width-4,SliderSize-4);
			sliderBox.size.x = getWidth() - 4;
			sliderBox.size.y = SliderSize - 4;
		}
		pDrawFuncs->drawRect2(sliderBox, drawColour, drawColour);
}


/** Called when user left-clicks *down* on a scrollbar. */
void CGUIbaseScrollbar::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	MouseDown = this;
	int mousePos;

	if (orientation == horizontal) 
		mousePos = calcLocalPos(mouseX, mouseY).x; 
	else
		mousePos = getHeight() - calcLocalPos(mouseX, mouseY).y;

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
			SliderPos = calcLocalPos(mouseX, mouseY).x - mouseSliderOffset;
		else {
			SliderPos = (getHeight() - calcLocalPos(mouseX, mouseY).y) - mouseSliderOffset;
		}
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
	msg.Msg = uiMsgSlide;
	msg.value = Value;
	parent->message(this,msg);
}

