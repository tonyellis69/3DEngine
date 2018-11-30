

#include "GUIbutton2.h"

using namespace glm;

CGUIbutton2::CGUIbutton2(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y);
	//width = w; height = h;
	//drawBox.pos = i32vec2(x, y);
	drawBox.size = i32vec2(w, h);

	type = uiButton;

	//pDrawFuncs->registerControl(*this);

	label = new CGUIlabel2(5, 5, w - 10, h - 10);
	label->setText("Button");
	label->setHorizontalAlignment(tcentred);
	Add(label);
	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);
}

void CGUIbutton2::DrawSelf() {

	//draw rectangle
	if (MouseDown == this) {
		//setBackColour1(oldbackColour2);
		//setBackColour2(oldbackColour1);
		pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour2, (glm::vec4&)backColour1);
	}
	else {
		//setBackColour1(oldbackColour1);
		//setBackColour2(oldbackColour2);
		pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);
	}
	//pDrawFuncs->drawCtrlRect(*this);
//	pDrawFuncs->drawRect2(drawBox, uiOldbackColour1, uiOldbackColour2);

	if (MouseOver == this)
		//setBorderColour(UIdarkGrey);
		pDrawFuncs->drawBorder2(drawBox, uiDarkGrey);
	else
		//setBorderColour(UIlightGrey);
		pDrawFuncs->drawBorder2(drawBox, uiLightGrey);
//	pDrawFuncs->drawCtrlBorder(*this);
}


void CGUIbutton2::OnClick(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	pDrawFuncs->handleUImsg(*this, msg);
	parent->message(this, msg);
	if (callbackObj)
		callbackObj->GUIcallback(this, msg);
}

void CGUIbutton2::setText(const std::string & text) {
	label->setText(text);
}
