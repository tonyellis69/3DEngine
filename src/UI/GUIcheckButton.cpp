#include "GUIcheckButton.h"

#include "UI/uiRender.h"

using namespace glm;

const int checked = 2;
const int unChecked = 0;

int CGUIcheckButton::Count = 1;
CGUIcheckButton::CGUIcheckButton(int x, int y, int w, int h) {
	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);
	Orientation = lblFirst;
	Set = false;
	type = checkButton;
	iconset = defaultIconset;

	inset = 3;
	int boxWidth = h - (2 * inset);
	int labelWidth = w - (boxWidth + 2 * inset);

	
	checkBox.size = glm::i32vec2(boxWidth);

	label = new CGUIlabel2(0,0, labelWidth, h);
	label->setText("check");
	label->setTextAlignment(tleft);
	label->setBorderOn(false);
	add(label);

}

void CGUIcheckButton::DrawSelf( ) {


	checkBox.pos = drawBox.pos + i32vec2(drawBox.size.x - (checkBox.size.x+inset), inset);
	if (Set)
		//pDrawFuncs->drawIcon(checked, iconPos,drawBox.pos.y + (drawBox.size.y >> 1));
		uiDraw::drawRect(checkBox, style::uiBlack, style::uiBlack);
	else
		//pDrawFuncs->drawIcon(unChecked, iconPos, drawBox.pos.y + (drawBox.size.y >> 1));
		uiDraw::drawRect(checkBox, style::uiWhite, style::uiWhite);
	
	if (MouseOver == this)
		//setBorderColour(UIdarkGrey);
		uiDraw::drawBorder(checkBox, style::uiDarkGrey);
	else
		//setBorderColour(UIlightGrey);
		uiDraw::drawBorder(checkBox, style::uiLightGrey);
	//pDrawFuncs->drawCtrlBorder(*this);
}

bool CGUIcheckButton::OnClick(const  int mouseX, const  int mouseY) {
	Set = !Set;
	//call the user's event handler.
	CMessage msg;
	msg.Msg = uiClick;
	msg.value = Set;
	//pDrawFuncs->handleUImsg(*this,msg);
	parent->message(this, msg);
	return true;
}

void CGUIcheckButton::setText(std::string newText) {
	label->setText(newText);
}



