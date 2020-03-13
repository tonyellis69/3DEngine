

#include "GUIbutton2.h"

using namespace glm;

CGUIbutton2::CGUIbutton2(int x, int y, int w, int h) : CGUIbase(x,y,w,h) {
	type = uiButton;

	label = new CGUIlabel2(5, 5, w - 10, h - 10);
	label->setText("Button");
	label->setTextAlignment(tcentred);
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
//	pDrawFuncs->drawRect2(drawBox, style::uiOldbackColour1, style::uiOldbackColour2);

	if (MouseOver == this)
		//setBorderColour(UIdarkGrey);
		pDrawFuncs->drawBorder2(drawBox, style::uiDarkGrey);
	else
		//setBorderColour(UIlightGrey);
		pDrawFuncs->drawBorder2(drawBox, style::uiLightGrey);
//	pDrawFuncs->drawCtrlBorder(*this);
}


bool CGUIbutton2::OnClick(const int mouseX, const int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	parent->message(this, msg);
	//if (callbackObj)
		callbackObj->GUIcallback(this, msg);
	return true;
}

void CGUIbutton2::setText(const std::string & text) {
	label->setText(text);
}
