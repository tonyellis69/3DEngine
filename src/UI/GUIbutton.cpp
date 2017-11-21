#include "GUIbutton.h"


int CGUIbutton::Count = 1;

CGUIbutton::CGUIbutton(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	backColour1 = oldbackColour1;
	backColour2 = oldbackColour2;
	GenName("Button",Count++);
	SetText(Name);
	type = button;
}


void CGUIbutton::DrawSelf( ) {
///////////////	pDrawFuncs->setFont(TextFont);
	if (MouseDown == this)
		pDrawFuncs->setDrawColours(backColour2, backColour1);
	else 
		pDrawFuncs->setDrawColoursConditional(backColour1, backColour2,enabled);

	//draw button 
	pDrawFuncs->drawRect(screenPos,width,height);
	if (MouseOver == this)
		pDrawFuncs->setDrawColours(UIdarkGrey, UIdarkGrey);
	else
		pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);

	//Write button label
	pDrawFuncs->setDrawColoursConditional(txtColour, txtColour,enabled);
////////////////	pDrawFuncs->drawText(screenPos.x,screenPos.y + (height/2),width,(char*)Text.c_str());
}


void CGUIbutton::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	pDrawFuncs->handleUImsg(*this,msg);
}