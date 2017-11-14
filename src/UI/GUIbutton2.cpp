#include "GUIbutton2.h"

using namespace glm;

CGUIbutton2::CGUIbutton2(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	type = uiButton;

	pDrawFuncs->registerControl(*this);

	label = new CGUIlabel2(5, 5, w - 5, h - 5);
	label->setText("Button");
	Add(label);
}

void CGUIbutton2::DrawSelf() {
	
	if (MouseOver == this)
		setBorderColour(UIdarkGrey);
	else
		setBorderColour(UIlightGrey);
	pDrawFuncs->drawCtrlBorder(*this);
	
	//draw button
	pDrawFuncs->drawCtrlRect(*this);
}
