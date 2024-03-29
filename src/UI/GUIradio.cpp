#include "GUIradio.h"


int CGUIradio::Count = 1;
CGUIradio::CGUIradio(int x , int y, int w, int h) {
	//localPos = glm::i32vec2(x, y);
	setLocalPos(x, y);
	//width = w;height = h;
	setWidth(w);
	setHeight(h);
	GenName("Radio",Count++);
	//SetText(Name);
	Orientation = lblFirst;
	Set = false;
	type = radioButton;
	iconset = defaultIconset;

	label = new CGUIlabel2(5, 5, w - (10 + iconOffset), h - 10);
	label->setText("radio");
	label->setTextAlignment(tright);
	add(label);
}


void CGUIradio::DrawSelf( ) {
///////////	pDrawFuncs->setFont(TextFont);
//	pDrawFuncs->setDrawColours(txtColour, txtColour);

	int w = 0; int reducedWidth = getWidth() - iconOffset;

	if (MouseOver == this) {
//		pDrawFuncs->setDrawColours(UIblack, UIblack);
	//	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);
	}

	if (label->getJustification() == tcentred)
		w = reducedWidth;
	if (label->getJustification() == tright)
		w = -reducedWidth;

	int lblPos, iconPos;
	if (Orientation == lblFirst) {
		iconPos = getScreenPos().x + reducedWidth + (iconOffset >> 1) ;
		lblPos = getScreenPos().x;
		label->setLocalPos(5, 5);
	} else {
		lblPos = getScreenPos().x + iconOffset;
		iconPos = getScreenPos().x + (iconOffset >> 1);
		label->setLocalPos(10 + iconOffset, 5);
	}
 
	//Write the text at the given point
	//pDrawFuncs->drawText(lblPos,screenPos.y + (height >> 1),w,(char*)Text.c_str());
	//draw the icon
	//pDrawFuncs->setIconset(iconset);
//	pDrawFuncs->setDrawColours(style::uiWhite,style::uiWhite);
	//if (Set)
//		pDrawFuncs->drawIcon(2, iconPos,screenPos.y + (height >> 1));
	//else
	//	pDrawFuncs->drawIcon(0, iconPos,screenPos.y + (height >> 1));


}

bool CGUIradio::OnClick(const  int mouseX, const  int mouseY) {
	Set = true;
	//notify the parentEntity  that this switch has been activated.
	CMessage msg;
	msg.value = true;
	parent->message(this,msg);

	//call the user's event handler.
	//pDrawFuncs->handleUImsg(*this,msg);
	return true;
}



