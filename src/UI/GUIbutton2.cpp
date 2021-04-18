
#include "GUIbutton2.h"

#include "UI/uiRender.h"

using namespace glm;

CGUIbutton2::CGUIbutton2(int x, int y, int w, int h) : CGUIbase(x,y,w,h) {
	type = uiButton;

	label = new CGUIlabel2(5, 5, w - 10, h - 10);
	label->setText("Button");
	label->setTextAlignment(tcentred);
	add(label);
}

void CGUIbutton2::DrawSelf() {

	//draw rectangle
	if (MouseDown == this) {
		//setBackColour1(oldbackColour2);
		//setBackColour2(oldbackColour1);
		//uiDraw::drawRect(drawBox, (glm::vec4&)backColour2, (glm::vec4&)backColour1);
		uiDraw::drawRect(drawBox, style::uiOldbackColour2, style::uiOldbackColour1);
	}
	else {
		//setBackColour1(oldbackColour1);
		//setBackColour2(oldbackColour2);
		//uiDraw::drawRect(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);
		uiDraw::drawRect(drawBox, style::uiOldbackColour1, style::uiOldbackColour2);
	}
	//pDrawFuncs->drawCtrlRect(*this);
//	uiDraw::drawRect(drawBox, style::uiOldbackColour1, style::uiOldbackColour2);

	if (MouseOver == this)
		//setBorderColour(UIdarkGrey);
		uiDraw::drawBorder(drawBox, style::uiDarkGrey);
		
	else
		//setBorderColour(UIlightGrey);
		//uiDraw::drawBorder(drawBox, style::uiLightGrey);
		uiDraw::drawBorder(drawBox, style::uiLightGrey);
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
