#pragma once
#include "GUIbase.h"
#include "GUIicons.h"
#include "GUIlabel.h"



/** Simple check button control. */

class CGUIcheckButton :  public CGUIbase, public CGUIbaseLbl {
public:
	CGUIcheckButton(int x, int y, int w, int h);
	void DrawSelf();
	void OnClick(const  int mouseX, const  int mouseY);

	static int Count;
	RadioOrientationType Orientation; ///<Which way around the label and icon go.
	bool Set; ///<Whether this button is selected or not.
	int iconset; ///<Identifies what tileset icons will be taken from.
};

