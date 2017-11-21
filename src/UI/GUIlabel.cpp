#include "GUIlabel.h"


void CGUIbaseLbl::SetText(const std::string& TextStr) {
	Text = TextStr;
	txtColour = UIblack; 
}


void CGUIsysLbl::DrawSelf( ) {
	//Set the font
///////////	pDrawFuncs->setFont(TextFont);

	//Set the colour
	pDrawFuncs->setDrawColoursConditional(txtColour, txtColour,enabled);

	int w = 0;
	if (TextAlign == tcentred)
		w = width;
	if (TextAlign == tright)
		w = -width;
	//Write the text at the given point
	if (multiLine)
		;///////////		pDrawFuncs->drawTextMultiLine(screenPos.x,screenPos.y,w,(char*)Text.c_str());
	else
		;/////////////	pDrawFuncs->drawText(screenPos.x,screenPos.y+(height/2),w,(char*)Text.c_str());

	//uncomment this to see where label is.
//	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);

}

int CGUIlabel::Count = 1;

CGUIlabel::CGUIlabel(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	GenName("Label",Count++);
	SetText(Name);
	type = label;
}