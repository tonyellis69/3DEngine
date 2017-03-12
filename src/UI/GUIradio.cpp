#include "GUIradio.h"


int CGUIradio::Count = 1;
CGUIradio::CGUIradio(int x , int y, int w, int h) {
	xPos = x;yPos = y;width = w;height = h;
	GenName("Radio",Count++);
	SetText(Name);
	Orientation = iconFirst;
	Set = false;
	type = radioButton;
	iconset = defaultIconset;
}


void CGUIradio::DrawSelf( ) {
	pDrawFuncs->setFont(TextFont);
	pDrawFuncs->setDrawColours(txtColour, txtColour);

	int w = 0; int reducedWidth = width - iconOffset;

	if (MouseOver == this) {
		pDrawFuncs->setDrawColours(UIblack, UIblack);
		pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);
	}

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
		pDrawFuncs->drawIcon(2, iconPos,screenPos.y + (height >> 1));
	else
		pDrawFuncs->drawIcon(0, iconPos,screenPos.y + (height >> 1));


}

void CGUIradio::OnClick(const  int mouseX, const  int mouseY) {
	Set = true;
	//notify the parent  that this switch has been activated.
	CMessage msg;
	msg.value = true;
	parent->message(*this,msg);

	//call the user's event handler.
	pDrawFuncs->handleUImsg(*this,msg);
}



