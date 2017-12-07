#pragma once
#include "GUIbase.h"

class CGUIpanel : public CGUIbase {
public:
	CGUIpanel() {};
	CGUIpanel(int x, int y, int w, int h);
	void OnMouseMove(const int mouseX, const int mouseY, int key) {};
	void DrawSelf();


	UIcolour borderColour; ///<Colour for the border of this control.
	bool drawBorder; ///<true if we draw this control's border.
};