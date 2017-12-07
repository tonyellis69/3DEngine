#pragma once
#include "GUIbase.h"
#include "GUIlabel.h"

class CGUIbutton :  public CGUIbase, public CGUIbaseLbl {
public:
	CGUIbutton(void) {};
	CGUIbutton(int x, int y, int w, int h);
	void DrawSelf();
	void OnClick(const  int mouseX, const  int mouseY);

	static int Count;
};
