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

	inset = 3;

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

	guiRect checkBox;
	checkBox.pos = drawBox.pos + inset;
	checkBox.size = glm::i32vec2(drawBox.size.y - 2 * inset);
 

	//draw the icon
	pDrawFuncs->setIconset(iconset);
	pDrawFuncs->setDrawColours(UIwhite,UIwhite);
	if (Set)
		//pDrawFuncs->drawIcon(checked, iconPos,drawBox.pos.y + (drawBox.size.y >> 1));
		pDrawFuncs->drawRect2(checkBox, uiBlack, uiBlack);
	else
		//pDrawFuncs->drawIcon(unChecked, iconPos, drawBox.pos.y + (drawBox.size.y >> 1));
		pDrawFuncs->drawRect2(checkBox, uiWhite, uiWhite);
	
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
	parent->message(this, msg);
}

void CGUIcheckButton::setText(std::string newText) {
	//text = newText;
	//renderText();
	label->setText(newText);
}



