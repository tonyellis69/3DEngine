#include "GUIcheckButton.h"

using namespace glm;

const int checked = 2;
const int unChecked = 0;

int CGUIcheckButton::Count = 1;
CGUIcheckButton::CGUIcheckButton(int x, int y, int w, int h) {
	setPos(x, y);
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
	label->setHorizontalAlignment(tleft);
	label->setBorderOn(false);
	Add(label);

}

void CGUIcheckButton::DrawSelf( ) {


	checkBox.pos = drawBox.pos + i32vec2(drawBox.size.x - (checkBox.size.x+inset), inset);
	if (Set)
		//pDrawFuncs->drawIcon(checked, iconPos,drawBox.pos.y + (drawBox.size.y >> 1));
		pDrawFuncs->drawRect2(checkBox, uiBlack, uiBlack);
	else
		//pDrawFuncs->drawIcon(unChecked, iconPos, drawBox.pos.y + (drawBox.size.y >> 1));
		pDrawFuncs->drawRect2(checkBox, uiWhite, uiWhite);
	
	if (MouseOver == this)
		//setBorderColour(UIdarkGrey);
		pDrawFuncs->drawBorder2(checkBox, uiDarkGrey);
	else
		//setBorderColour(UIlightGrey);
		pDrawFuncs->drawBorder2(checkBox, uiLightGrey);
	//pDrawFuncs->drawCtrlBorder(*this);
}

void CGUIcheckButton::OnClick(const  int mouseX, const  int mouseY) {
	Set = !Set;
	//call the user's event handler.
	CMessage msg;
	msg.Msg = uiClick;
	msg.value = Set;
	pDrawFuncs->handleUImsg(*this,msg);
	parent->message(this, msg);
}

void CGUIcheckButton::setText(std::string newText) {
	label->setText(newText);
}



