#pragma once
#include "GUIbase.h"

/** A basic label class for other controls to use. */
class CGUIbaseLbl  {
public: 
	CGUIbaseLbl() {TextFont = 1; TextAlign = tleft; multiLine = false;}
	void SetText(const std::string&  TextStr);
	std::string Text; ///<The text of this label.
	int TextFont; ///<Represents the font this label uses.
	TTextAlign TextAlign; ///<Records whether the text is centred, etc, in its area.
	UIcolour txtColour; ///<Drawing colour for text
	bool multiLine; ///<If true, text is not height-centred.
};

/** Label class for internal use, ie, group headers.*/
class CGUIsysLbl : public CGUIbase, public CGUIbaseLbl {
public:
	void DrawSelf( );

	static int Count;
	
};


/** A UI class for displaying lines of text. */
class CGUIlabel : public CGUIsysLbl {
public:
	CGUIlabel(int x, int y, int w, int h);
	static int Count;
	
};