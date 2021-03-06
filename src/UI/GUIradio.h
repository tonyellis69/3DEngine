#pragma once
#include "GUIbase.h"
#include "GUIicons.h"
#include "GUIlabel2.h"



/** Simple radio button control. */

class CGUIradio :  public CGUIbase {
public:
	CGUIradio(int x , int y, int w, int h);
	void DrawSelf( );
	bool OnClick(const  int mouseX, const  int mouseY);

	static int Count;
	RadioOrientationType Orientation; ///<Which way around the label and icon go.
	bool Set; ///<Whether this button is selected or not.
	int iconset; ///<Identifies what tileset icons will be taken from.

	CGUIlabel2* label;
};

