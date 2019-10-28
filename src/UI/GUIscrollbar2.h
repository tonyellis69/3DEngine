#pragma once
#include "GUIbase.h"

const int barWidth2 = 12;
enum ScrollbarOrientationType2 {scrlHorizontal, scrlVertical};

/** Simple scrollbar control for internal use, ie, containers. */
class CGUIbaseScrollbar2 :  public CGUIbase {
public:
//	CGUIbaseScrollbar() {};
	CGUIbaseScrollbar2(ScrollbarOrientationType2 axis, int x, int y, int length);
	CGUIbaseScrollbar2(ScrollbarOrientationType2 axis);
	void applyStyleSheet();
	void updateAppearance();
	void updateSliderAppearance();
	void setValue(int newValue);
	void DrawSelf( );
	void OnMouseMove(const int mouseX,const  int mouseY,int key);
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	virtual void updateValue();
	void setMax(int newMax); 
	void setMin(int newMin); 
	void setSliderSize(float ratio);
	
	ScrollbarOrientationType2 orientation; ///<Whether this scrollbar is horizonal or scrlVertical.
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
class CGUIscrollbar2 : public CGUIbaseScrollbar2 {
public:
	CGUIscrollbar2(ScrollbarOrientationType2 axis, int x, int y, int length) ;

	static int Count;
};

/** Scrollbar used by other controls, eg, scrolling containers. */
class CGUIsysScrollbar2: public CGUIbaseScrollbar2 {
public:
	CGUIsysScrollbar2::CGUIsysScrollbar2(ScrollbarOrientationType2 axis, int x, int y, int length) : CGUIbaseScrollbar2(axis,x,y,length) {};
	CGUIsysScrollbar2(ScrollbarOrientationType2 axis, unsigned int styleWord);
	void updateValue();

};


