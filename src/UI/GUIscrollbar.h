#pragma once
#include "GUIbase.h"

const int barWidth = 12;
enum ScrollbarOrientationType {horizontal, vertical};

/** Simple scrollbar control for internal use, ie, containers. */
class CGUIbaseScrollbar :  public CGUIbase {
public:
//	CGUIbaseScrollbar() {};
	CGUIbaseScrollbar(ScrollbarOrientationType axis, int x, int y, int length);
	void updateAppearance();
	void updateSliderAppearance();
	void setValue(int newValue);
	void DrawSelf( );
	bool OnMouseMove(const int mouseX,const  int mouseY,int key);
	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	virtual void updateValue();
	void setMax(int newMax); 
	void setMin(int newMin); 
	void setSliderSize(float ratio);
	
	ScrollbarOrientationType orientation; ///<Whether this scrollbar is horizonal or vertical.
	int Max; ///<Maximum scrollable value.
	int Min; ///<Minimum scrollable value.
	int Value; ///<Current value the scrollbar is scrolled to.
	int SliderSize; ///<Length of the slider in pixels.
	int clickIncrement; ///<Amount of scrolling when bar clicked

	int Travel; ///<The length in pixels that the slider can travel.

	int SliderPos; ///<Position of the slider long the scrollbrar.
	int mouseSliderOffset; ///<Position of the mouse within the slider.
	int lastMouseDownX; ///<Last mouse positionHint when left button went down.
	int lastMouseDownY; ///<Last mouse positionHint when left button went down.
};


/** Standard user scrollbar. */
class CGUIscrollbar : public CGUIbaseScrollbar {
public:
	CGUIscrollbar(ScrollbarOrientationType axis, int x, int y, int length) ;

	static int Count;
};

/** Scrollbar used by other controls, eg, scrolling containers. */
class CGUIsysScrollbar: public CGUIbaseScrollbar {
public:
	CGUIsysScrollbar::CGUIsysScrollbar(ScrollbarOrientationType axis, int x, int y, int length) : CGUIbaseScrollbar(axis,x,y,length) {};
	void updateValue();

};


