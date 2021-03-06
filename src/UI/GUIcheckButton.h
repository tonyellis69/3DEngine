#pragma once
#include "GUIbase.h"
#include "GUIicons.h"

#include "GUIlabel2.h"

/** Simple check button control. */

class CGUIcheckButton :  public CGUIbase {
public:
	CGUIcheckButton(int x, int y, int w, int h);
	void DrawSelf();
	bool OnClick(const  int mouseX, const  int mouseY);

	void setText(std::string newText);

	static int Count;
	RadioOrientationType Orientation; ///<Which way around the label and icon go.
	bool Set; ///<Whether this button is selected or not.
	int iconset; ///<Identifies what tileset icons will be taken from.

	int inset; //<margin between drawbox and actual on/off inner box.

	CGUIlabel2* label;
	guiRect checkBox;
};

