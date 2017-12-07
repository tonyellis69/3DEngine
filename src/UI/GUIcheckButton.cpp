#include "GUIcheckButton.h"



const int checked = 2;
const int unChecked = 0;

int CGUIcheckButton::Count = 1;
CGUIcheckButton::CGUIcheckButton(int x, int y, int w, int h) {
	pDrawFuncs->registerControl(*this);
	SetPos(x,y,w,h);
	Orientation = lblFirst;
	Set = false;
	type = checkButton;
	iconset = defaultIconset;

	label = new CGUIlabel2(5, 5, w - (10 + iconOffset), h - 10);
	label->setText("check");
	label->setHorizontalAlignment(tright);
	//label->borderOn(true);
	Add(label);

}

void CGUIcheckButton::DrawSelf( ) {
////////////	pDrawFuncs->setFont(TextFont);
	//pDrawFuncs->setDrawColours(txtColour, txtColour);

	int w = 0; int reducedWidth = drawBox.size.x - iconOffset;

	if (label->getJustification() == tcentred)
		w = reducedWidth;
	if (label->getJustification() == tright)
		w = -reducedWidth;

	int lblPos, iconPos;
	if (Orientation == lblFirst) {
		iconPos = drawBox.pos.x + reducedWidth + (iconOffset >> 1) ;
		lblPos = drawBox.pos.x;
		label->setPos(5,5);
	} else {
		lblPos = drawBox.pos.x + iconOffset;
		iconPos = drawBox.pos.x + (iconOffset >> 1);
		label->setPos(10 + iconOffset, 5);
	}
 
	//Write the text at the given point
	//pDrawFuncs->drawText(lblPos,screenPos.y + (height >> 1),w,(char*)Text.c_str());
	//draw the icon
	pDrawFuncs->setIconset(iconset);
	pDrawFuncs->setDrawColours(UIwhite,UIwhite);
	if (Set)
		pDrawFuncs->drawIcon(checked, iconPos,drawBox.pos.y + (drawBox.size.y >> 1));
	else
		pDrawFuncs->drawIcon(unChecked, iconPos, drawBox.pos.y + (drawBox.size.y >> 1));
	
	if (MouseOver == this)
		setBorderColour(UIdarkGrey);
	else
		setBorderColour(UIlightGrey);
	pDrawFuncs->drawCtrlBorder(*this);
}

void CGUIcheckButton::OnClick(const  int mouseX, const  int mouseY) {
	Set = !Set;
	//call the user's event handler.
	CMessage msg;
	msg.Msg = uiClick;
	msg.value = Set;
	pDrawFuncs->handleUImsg(*this,msg);
}

void CGUIcheckButton::setText(std::string newText) {
	//text = newText;
	//renderText();
	label->setText(newText);
}



