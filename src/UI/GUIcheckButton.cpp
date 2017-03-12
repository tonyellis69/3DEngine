#include "GUIcheckButton.h"

const int checked = 2;
const int unChecked = 0;

int CGUIcheckButton::Count = 1;
CGUIcheckButton::CGUIcheckButton(int x, int y, int w, int h) {
	SetPos(x,y,w,h);
	GenName("checkButton",Count++);
	SetText(Name);
	Orientation = lblFirst;
	Set = false;
	type = checkButton;
	iconset = defaultIconset;
}

void CGUIcheckButton::DrawSelf( ) {
	pDrawFuncs->setFont(TextFont);
	pDrawFuncs->setDrawColours(txtColour, txtColour);

	int w = 0; int reducedWidth = width - iconOffset;

	if (TextAlign == tcentred)
		w = reducedWidth;
	if (TextAlign == tright)
		w = -reducedWidth;

	int lblPos, iconPos;
	if (Orientation == lblFirst) {
		iconPos =  screenPos.x + reducedWidth + (iconOffset >> 1) ;
		lblPos = screenPos.x;
	} else {
		lblPos =  screenPos.x + iconOffset;
		iconPos = screenPos.x + (iconOffset >> 1);
	}
 
	//Write the text at the given point
	pDrawFuncs->drawText(lblPos,screenPos.y + (height >> 1),w,(char*)Text.c_str());
	//draw the icon
	pDrawFuncs->setIconset(iconset);
	pDrawFuncs->setDrawColours(UIwhite,UIwhite);
	if (Set)
		pDrawFuncs->drawIcon(checked, iconPos,screenPos.y + (height >> 1));
	else
		pDrawFuncs->drawIcon(unChecked, iconPos,screenPos.y + (height >> 1));
}

void CGUIcheckButton::OnClick(const  int mouseX, const  int mouseY) {
	Set = !Set;
	//call the user's event handler.
	CMessage msg;
	msg.Msg = uiClick;
	msg.value = Set;
	pDrawFuncs->handleUImsg(*this,msg);
}



