

#include "GUIbutton2.h"

using namespace glm;

CGUIbutton2::CGUIbutton2(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);

	type = uiButton;

	pDrawFuncs->registerControl(*this);

	label = new CGUIlabel2(5, 5, w - 10, h - 10);
	label->setText("Button");
	label->setHorizontalAlignment(tcentred);
	Add(label);
}

void CGUIbutton2::DrawSelf() {
	
	if (MouseOver == this)
		setBorderColour(UIdarkGrey);
	else
		setBorderColour(UIlightGrey);
	pDrawFuncs->drawCtrlBorder(*this);

	if (MouseDown == this) {
		setBackColour1(oldbackColour2);
		setBackColour2(oldbackColour1);
	}
	else {
		setBackColour1(oldbackColour1);
		setBackColour2(oldbackColour2);
	}
	
	pDrawFuncs->drawCtrlRect(*this);
}


void CGUIbutton2::OnClick(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	pDrawFuncs->handleUImsg(*this, msg);

}